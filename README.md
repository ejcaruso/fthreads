femtothreads
============

Small threading library to demonstrate how one might implement threads on
an x86 CPU. Not meant to be used for any heavy-duty lifting, and is meant
for learning.

Try it out
----------

Run `make` to get a binary called `echo_server` in the example directory,
which shows how you can use `fthreads` to write a simple, multithreaded
network server without having to change much from a `pthreads` implementation.

Use it yourself
---------------

Wanna take it for a spin? Just `#include <fthread.h>` in your project, and
compile with `-lfthread`.

Interesting tidbits
-------------------

In addition to userland context switching, `fthreads` provides a more elegant
solution to IO multiplexing than standard calls to `poll` or `select` because
you can still write code in a style that looks like threads. The scheduler
is doing the multiplexing for you behind the scenes.
