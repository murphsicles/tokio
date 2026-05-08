# Zeta Tokio — Async Runtime for Zeta

Epoll-based multi-threaded async runtime. Reactor, waker, timerfd all
wired through the Zeta bootstrap runtime (v0.11.2+).

## Quick Start
```bash
zetac --jit src/main.z        # JIT mode (requires v0.11.2+)
zetac src/main.z -o tokio.o   # AOT — links with tokio_runtime.o
gcc -no-pie tokio.o tokio_runtime.o -lc -o zeta_tokio
```

## Verified Working (JIT)
- epoll_create → reactor_create()
- epoll_ctl → reactor_add() / reactor_modify() / reactor_remove()
- epoll_wait → reactor_poll() / scheduler_run_reactor()
- pipe2 → waker_create() / waker_wake() / waker_consume()
- timerfd → timerfd_create() / timerfd_set() / timerfd_read()
- fcntl → set_nonblocking()
- clock_gettime → monotonic_ns()
- thread pool → blocking_spawn()
- scheduler → scheduler_register_waker()
- Runtime struct with block_on() + reactor loop

## Examples
- hello_tokio.z — block_on returns 42
- timer_example.z — timed reactor polls (50+30+20ms)
- echo_server.z — TCP echo with reactor-based accept/read/write
