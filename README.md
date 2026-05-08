# Zeta Tokio — Complete Async Runtime for Zeta

**A Tokio-compatible async runtime for Zeta.** Uses the epoll reactor, waker pipe,
and timerfd runtime built into the Zeta bootstrap runtime (v0.11.0+).

All async primitives are declared as `extern fn` in Zeta source, backed by the
Zeta bootstrap runtime library. No external dependencies, no separate C library.

## Architecture

```
Zeta Tokio (.z sources)       Zeta Bootstrap Runtime
  │                                    │
  ├─ runtime/mod.z ── extern fn ────→  reactor_create / add / poll
  ├─ net/tcp.z     ── extern fn ────→  set_nonblocking
  ├─ time/mod.z    ── extern fn ────→  timerfd_create / set / read
  ├─ sync/mutex.z  ── extern fn ────→  waker_create / wake / consume
  └─ ...                               monotonic_ns
                                       epoll_ctl / epoll_wait
                                       pipe2 / timerfd_settime
                                       fcntl(O_NONBLOCK) / clock_gettime
```

## Quick Start
```bash
zetac src/main.z -o zeta_tokio
./zeta_tokio
```

## Modules
| Module | Purpose | Backend |
|--------|---------|---------|
| runtime | Reactor, waker, timerfd | epoll, pipe2, timerfd |
| net::tcp | TcpListener, TcpStream | set_nonblocking |
| time | sleep, timeout, sleep_until | timerfd |
| sync | Mutex, RwLock, Barrier, Semaphore, Notify, Watch | waker |
| channel | oneshot, mpsc, broadcast | waker |
| task | spawn, JoinHandle | runtime |
