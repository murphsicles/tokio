# Zeta Tokio — Complete Async Runtime for Zeta

[<img alt="Zeta Logo" width="24px" src="https://z-lang.org/assets/images/z72.png" />](https://z-lang.org) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**A Tokio-compatible async runtime for Zeta.** Uses the epoll reactor, waker pipe, and timerfd runtime built into the Zeta bootstrap compiler (v0.11.0+).

## Architecture

```
  Zeta Tokio (.z sources)      Zeta Bootstrap Runtime (Rust, in compiler)
  ┌────────────────────┐       ┌──────────────────────────────┐
  │ runtime/mod.z      │ ──→   │ reactor_create / add / poll  │
  │ net/tcp.z          │ ──→   │ set_nonblocking              │
  │ time/mod.z         │ ──→   │ timerfd_create / set / read  │
  │ sync/mutex.z       │ ──→   │ waker_create / wake  / consume│
  │ ...                │       │ monotonic_ns                 │
  └────────────────────┘       └──────────────────────────────┘
                                          │
                                    epoll / timerfd / pipe2
                                    (Linux kernel syscalls)
```

## Quick Start

```bash
# Requires Zeta compiler v0.11.0+
zetac src/main.z -o zeta_tokio
./zeta_tokio
```

No separate C runtime needed — all async primitives are in the compiler.
