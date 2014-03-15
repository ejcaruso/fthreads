#include <fthread.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 160
#define MAX_WAITING 16

static void *server_thread(void *arg) {
  int sock = (int)arg;
  char buf[MAX_BUF];

  int bytes;
  while ((bytes = read(sock, buf, MAX_BUF)) > 0) {
    write(sock, buf, bytes);
  }

  close(sock);
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s port\n", argv[0]);
    return 1;
  }

  // set up server socket
  char *endp;
  long port = strtol(argv[1], &endp, 10);
  if (*endp != 0) {
    fprintf(stderr, "Invalid characters in port number\n");
    return 1;
  } else if (port < 0 || port > 65535) {
    fprintf(stderr, "Port number out of range\n");
    return 1;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short) port);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sockfd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror("bind");
    return 1;
  }
  if (listen(sockfd, MAX_WAITING) < 0) {
    perror("listen");
    return 1;
  }
  
  for (;;) {
    int client = accept(sockfd, 0, 0);
    if (client < 0) {
      perror("accept");
      return 1;
    }

    fthread_t next;
    fthread_create(&next, server_thread, (void *)client);
    fthread_detach(next);
  }
  return 0;
}
