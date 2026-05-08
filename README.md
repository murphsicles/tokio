# Zeta Tokio — Complete Async Runtime for Zeta

[<img alt="Zeta Logo" width="24px" src="https://z-lang.org/assets/images/z72.png" />](https://z-lang.org) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**A Tokio-compatible async runtime for Zeta** with epoll-based reactor, timerfd timers, waker pipes, and full async I/O. Built on Zeta v1.0.2+.

## Architecture

```
Zeta Tokio (.z sources)
  │
  ├── Runtime         — multi-threaded, epoll reactor
  ├── Task            — spawn, spawn_blocking, JoinHandle
  ├── Net             — TcpListener, TcpStream, UdpSocket
  ├── Time            — sleep, timeout, interval (timerfd)
  ├── I/O             — BufReader, DuplexStream, split
  ├── Sync            — Mutex, RwLock, Barrier, Semaphore, Notify
  ├── Channel         — oneshot, mpsc, broadcast
  ├── Signal          — Ctrl+C, SIGINT, SIGTERM
  ├── FS              — read, write, create, copy, remove
  └── Tokio Runtime C (.o)
       ├── epoll_create1 / epoll_ctl / epoll_wait
       ├── pipe2 (waker)
       ├── timerfd_create / timerfd_settime
       ├── fcntl (O_NONBLOCK)
       └── clock_gettime (monotonic)
```

## Modules

| Module | Files | Features |
|--------|-------|----------|
| `runtime` | `mod.z` | Reactor, waker, epoll poll, timerfd, block_on, spawn |
| `task` | `mod.z`, `join.z` | spawn, spawn_blocking, JoinHandle |
| `time` | `mod.z` | sleep, timeout, interval, Duration/Instant |
| `net::tcp` | `tcp.z` | TcpListener, TcpStream (async connect/accept/read/write) |
| `net::udp` | `udp.z` | UdpSocket (send_to, recv_from) |
| `io` | `mod.z` | BufReader, DuplexStream, split |
| `sync::mutex` | `mutex.z` | Async Mutex with waker queue |
| `sync::rwlock` | `rwlock.z` | Async RwLock (readers/writer) |
| `sync::barrier` | `barrier.z` | Barrier with generational wait |
| `sync::semaphore` | `semaphore.z` | Semaphore with acquire_many |
| `sync::notify` | `notify.z` | Notify one/many |
| `sync::watch` | `watch.z` | Watch channel (state observation) |
| `channel::oneshot` | `oneshot.z` | One-shot with waker |
| `channel::mpsc` | `mpsc.z` | MPSC bounded/unbounded |
| `channel::broadcast` | `broadcast.z` | Fan-out broadcast |
| `signal` | `mod.z` | Ctrl+C, SIGINT, SIGTERM |
| `fs` | `mod.z` | read, write, create_dir, remove, copy, rename |

## Runtime Library

The C runtime (`tokio-runtime/tokio_runtime.c`) provides the low-level async primitives:

- **reactor** — epoll-based event loop (add/modify/remove/poll)
- **waker** — pipe-based task notification (create/wake/consume/destroy)
- **timerfd** — high-precision async timers (create/set/set_absolute/read)
- **non-blocking I/O** — fcntl O_NONBLOCK, monotonic clock

Compiled to `tokio_runtime.o` and linked into every Zeta Tokio program.

## Quick Start

```bash
# Build the runtime library
cd tokio-runtime && gcc -c -O2 tokio_runtime.c -o tokio_runtime.o

# Compile a Zeta Tokio program
zetac src/main.z -o zeta_tokio.o
gcc -no-pie zeta_tokio.o tokio-runtime/tokio_runtime.o -lc -o zeta_tokio

# Run
./zeta_tokio
```

## Examples

- `echo_server.z` — TCP echo server using reactor-based accept
- `timer_example.z` — Precise async sleep via timerfd
- `channel_example.z` — Oneshot and MPSC channel usage
- `notify_example.z` — Notify primitive demo
- `hello_tokio.z` — Hello world
