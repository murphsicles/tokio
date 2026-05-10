# Zeta Tokio — Async Runtime for Zeta

Epoll-based multi-threaded async runtime with reactor, waker, timerfd.
Requires Zeta **v1.0.11+** (v0.14.4 bootstrap — monomorphized call resolution, waker JIT, submodule loading).

## Modes

**JIT (primary)** — `zetac --jit src/main.z`
All runtime symbols available via the zetac binary. Works out of the box.

**AOT** — Compile with gcc + tokio_runtime.o
```bash
zetac src/main.z -o prog   # produces prog.o
gcc -no-pie prog.o tokio_runtime.o -lc -o prog
./prog
```

## Architecture
```
Zeta source → zetac --jit → LLVM JIT → reactor → epoll/waker/timerfd
```

## Status
- ✅ Reactor: epoll_create, epoll_ctl, epoll_wait — JIT verified, ~50ms ±0.1%
- ✅ Waker: pipe2, write/read notification — JIT verified
- ✅ Timer: timerfd_create/set/read — JIT verified  
- ✅ Scheduler: reactor + waker integration — JIT verified
- ✅ Blocking pool: dedicated thread pool — runtime ready
- ✅ Monomorphized fn params: 0-param generic fns generate correct LLVM signatures
- ✅ Future wrapping: async fn returns wrapped in future_ready() for safe .await
- ✅ Async yield: .await calls async_yield() (sched_yield) instead of busy-spinning
- ✅ Channel examples: oneshot (send/recv) + mpsc (try_send/try_recv) tested working
- 🟢 AOT: links with tokio_runtime.o ✅ — timing vs JIT needs investigation
- ✅ Self-hosting (--bootstrap): **zero errors**
- ❌ Full waker wiring: async state machine needs runtime waker registration (reactor integration not yet automatic)
