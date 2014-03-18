#include <dlfcn.h>
#include <errno.h>
#include <poll.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "fthread.h"
#include "ft_sched.h"
#include "ft_queue.h"

/* Dynamic linking for interpositioning. */

#define LIBC "libc.so.6"

static void *libc;

static void __attribute__((constructor)) async_init() {
  libc = dlopen(LIBC, RTLD_LAZY);
  if (!libc) {
    exit(1);
  }
}

static void __attribute__((destructor)) async_fini() {
  dlclose(libc);
}

/* This information will be passed to poll(2) in wait_For_event. */

#define MAX_IN_FLIGHT 64

struct pollfd in_flight[MAX_IN_FLIGHT];
int next_req_pos = 0;
fthread_cond_t in_flight_cond = FTHREAD_COND_INITIALIZER;
fthread_mutex_t in_flight_mutex = FTHREAD_MUTEX_INITIALIZER;

/* These are used by tfind to determine what requests are already in flight.
 * We can only have one in-flight request for a particular FD at once, and we
 * need to wake up the right thread when it is done. */

enum io_type {
  READ,
  WRITE
};
static void *io_map[2] = {0, 0};

struct io_entry {
  int             fd;
  fthread_mutex_t mutex;
  queue_t         waitq;
  int             waiters;
};

static int fdcmp(const void *pa, const void *pb) {
  struct io_entry *io_ent_a = (struct io_entry *)pa,
                  *io_ent_b = (struct io_entry *)pb;
  if (io_ent_a->fd < io_ent_b->fd) {
    return -1;
  } else if (io_ent_a->fd > io_ent_b->fd) {
    return 1;
  } else {
    return 0;
  }
}

/* The general submission function for submitting and waiting on IO. */

static int submit_io_req(int fd, enum io_type type) {
  // Find the io_entry corresponding to the FD and type we want.
  struct io_entry *io_ent =
    (struct io_entry *)malloc(sizeof(struct io_entry));
  if (!io_ent) {
    return EAGAIN;
  }

  io_ent->fd = fd;
  void *nodep = tfind(io_ent, &io_map[type], fdcmp);
  if (!nodep) {
    // If it's not in the tree, put it there.
    nodep = tsearch(io_ent, &io_map[type], fdcmp);
    if (!nodep) {
      return EAGAIN;
    }

    // At this point, *(struct io_entry **)nodep == io_ent
    fthread_mutex_init(&io_ent->mutex);
    queue_init(&io_ent->waitq);
    io_ent->waiters = 1;
  } else {
    free(io_ent);
    io_ent = *(struct io_entry **)nodep;
    io_ent->waiters++;
  }
  
  // Get the mutex for the io_entry and try to submit a request.
  fthread_mutex_lock(&io_ent->mutex);
  
  fthread_mutex_lock(&in_flight_mutex);
  while (next_req_pos == MAX_IN_FLIGHT) {
    fthread_cond_wait(&in_flight_cond, &in_flight_mutex);
  }
  fthread_mutex_unlock(&in_flight_mutex);
  
  int req = next_req_pos++;
  struct pollfd *pfd = &in_flight[req];
  pfd->fd = fd;
  switch(type) {
    case READ: pfd->events = POLLIN; break;
    case WRITE: pfd->events = POLLOUT; break;
    default: {
      fthread_mutex_unlock(&io_ent->mutex);
      return EINVAL;
    }
  }

  sched_sleep_on(&io_ent->waitq);

  // IO is ready
  fthread_mutex_unlock(&io_ent->mutex);

  if (!--io_ent->waiters) {
    tdelete(io_ent, &io_map[type], fdcmp);
    free(io_ent);
  }
  return 0;
}

static void wakeup_for_io(int fd, enum io_type type) {
  struct io_entry io_ent;

  io_ent.fd = fd;
  void *nodep = tfind(&io_ent, &io_map[type], fdcmp);
  if (!nodep) {
    return;
  }

  sched_wakeup_on(&(*(struct io_entry **)nodep)->waitq);
}

ssize_t read(int fd, char *buf, size_t count) {
  static ssize_t (*sys_read)(int, char *, size_t) = 0;

  int ret = submit_io_req(fd, READ);
  if (ret) {
    errno = ret;
    return -1;
  }

  if (!sys_read) {
    sys_read = dlsym(libc, "read");
  }
  return sys_read(fd, buf, count);
}

ssize_t write(int fd, const char *buf, size_t count) {
  static ssize_t (*sys_write)(int, const char *, size_t) = 0;

  int ret = submit_io_req(fd, WRITE);
  if (ret) {
    errno = ret;
    return -1;
  }

  if (!sys_write) {
    sys_write = dlsym(libc, "write");
  }
  return sys_write(fd, buf, count);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  static int (*sys_accept)(int, struct sockaddr *, socklen_t *) = 0;
  
  int ret = submit_io_req(sockfd, READ);
  if (ret) {
    errno = ret;
    return -1;
  }

  if (!sys_accept) {
    sys_accept = dlsym(libc, "accept");
  }
  return sys_accept(sockfd, addr, addrlen);
}

void sched_wait_for_event() {
  fthread_mutex_lock(&in_flight_mutex);
  // first, poll across all the in-flight requests
  int saved_errno = errno;
  
  while (poll(in_flight, next_req_pos, -1) < 0);
  errno = saved_errno;
  
  // check the revents fields of each pollfd struct
  // also, compress the events which haven't triggered
  int new_req_pos = 0;
  int i;
  for (i = 0; i < next_req_pos; ++i) {
    struct pollfd *req = &in_flight[i];
    
    if (req->events & req->revents || req->events & POLLHUP) {
      wakeup_for_io(req->fd, (req->events == POLLIN ? READ : WRITE));
    } else {
      memcpy(&in_flight[new_req_pos++], req, sizeof(struct pollfd));
    } 
  }

  // if we have managed to make some room, let any waiting threads know
  next_req_pos = new_req_pos;
  if (next_req_pos < MAX_IN_FLIGHT) {
    fthread_cond_broadcast(&in_flight_cond);
  }
  fthread_mutex_unlock(&in_flight_mutex);
}
