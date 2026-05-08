# Zeta Tokio — Async Runtime for Zeta

[<img alt="Zeta Logo" width="24px" src="https://z-lang.org/assets/images/z72.png" />](https://z-lang.org) [![Latest Release](https://img.shields.io/github/v/release/murphsicles/tokio)](https://github.com/murphsicles/tokio/releases) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**A runtime for writing reliable asynchronous applications with Zeta.** Inspired by Tokio's architecture — work-stealing scheduler, reactor-based I/O, and composable async primitives.

Built on Zeta v1.0.2 self-hosted compiler.

## Architecture

```
┌───────────────────────────────────────────┐
│              Application                   │
├───────────────────────────────────────────┤
│  tokio::net  │  tokio::io  │  tokio::fs   │
│  tokio::time │ tokio::sync │ tokio::signal │
├───────────────────────────────────────────┤
│         tokio::runtime (scheduler)         │
├───────────────────────────────────────────┤
│     Zeta Runtime (actors, channels)        │
├───────────────────────────────────────────┤
│           Zeta Compiler (LLVM)             │
└───────────────────────────────────────────┘
```

## Features

- **Multi-threaded work-stealing scheduler** — Tokio-style runtime with configurable worker threads
- **Async I/O** — TCP listeners, streams, async filesystem operations
- **Timers** — sleep, timeout, interval, with high-precision time wheel
- **Channels** — oneshot, mpsc, broadcast
- **Async synchronization** — Mutex, RwLock, Barrier, Semaphore
- **Signal handling** — Ctrl+C, process signals
- **Task management** — spawn, spawn_blocking, JoinHandle, task-local storage

## Quick Start

```zeta
use tokio::runtime::Runtime;

async fn hello() -> str {
    return "Hello from Zeta Tokio!";
}

fn main() -> i64 {
    let rt = Runtime::new();
    let result = rt.block_on(async { hello().await });
    println_str(result);
    return 0;
}
```

## Building

```bash
# Compile with zetac
zetac src/main.z -o zeta_tokio
./zeta_tokio
```

## License

MIT
