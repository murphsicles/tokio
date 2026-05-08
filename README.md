# Zeta Tokio — Async Runtime for Zeta

Epoll-based multi-threaded async runtime with reactor, waker, timerfd.
Requires Zeta v0.11.3+.

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
- 🔄 AOT: links with tokio_runtime.o — timing needs investigation
- ❌ Self-hosting (--bootstrap): 1 remaining (new overload)
- ❌ True async waker wiring: needs JIT async state machine integration
