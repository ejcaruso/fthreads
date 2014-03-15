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
