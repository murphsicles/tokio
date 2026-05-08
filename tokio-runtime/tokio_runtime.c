/* tokio_runtime.c — Zeta Tokio low-level runtime
 *
 * Provides: epoll reactor, waker pipe, timerfd, non-blocking I/O
 * Compiled to tokio_runtime.o and linked into Zeta Tokio programs.
 *
 * All functions use the zeta_tokio_ prefix for symbol stability.
 */

#define _GNU_SOURCE
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>

/* ── Constants ── */

#define ZT_READABLE  1
#define ZT_WRITABLE  2
#define ZT_ERROR     4

#define MAX_EVENTS   1024

/* ── Reactor ── */

/* Create epoll reactor. Returns fd, or -1 on error. */
int64_t zeta_tokio_reactor_create(void) {
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    return (int64_t)epfd;
}

/* Register fd for events (ZT_READABLE | ZT_WRITABLE). Returns 0 or -1. */
int64_t zeta_tokio_reactor_add(int64_t epfd, int64_t fd, int64_t events) {
    struct epoll_event ev;
    ev.events = 0;
    ev.data.fd = (int)fd;
    if (events & ZT_READABLE) ev.events |= EPOLLIN;
    if (events & ZT_WRITABLE) ev.events |= EPOLLOUT;
    ev.events |= EPOLLERR | EPOLLHUP;
    return (int64_t)epoll_ctl((int)epfd, EPOLL_CTL_ADD, (int)fd, &ev);
}

/* Modify registered events for a fd. Returns 0 or -1. */
int64_t zeta_tokio_reactor_modify(int64_t epfd, int64_t fd, int64_t events) {
    struct epoll_event ev;
    ev.events = 0;
    ev.data.fd = (int)fd;
    if (events & ZT_READABLE) ev.events |= EPOLLIN;
    if (events & ZT_WRITABLE) ev.events |= EPOLLOUT;
    ev.events |= EPOLLERR | EPOLLHUP;
    return (int64_t)epoll_ctl((int)epfd, EPOLL_CTL_MOD, (int)fd, &ev);
}

/* Remove fd from reactor. Returns 0 or -1. */
int64_t zeta_tokio_reactor_remove(int64_t epfd, int64_t fd) {
    return (int64_t)epoll_ctl((int)epfd, EPOLL_CTL_DEL, (int)fd, NULL);
}

/* Thread-local event buffer */
static __thread struct epoll_event g_events[MAX_EVENTS];
static __thread int g_ready_count = 0;

/* Poll for events. timeout_ms: -1=infinite, 0=nonblocking, >0=ms.
 * Returns number of ready events (0 on timeout, <0 on error).
 * After call, read events with zeta_tokio_reactor_event_fd/flags. */
int64_t zeta_tokio_reactor_poll(int64_t epfd, int64_t timeout_ms) {
    g_ready_count = epoll_wait((int)epfd, g_events, MAX_EVENTS, (int)timeout_ms);
    return (int64_t)g_ready_count;
}

/* Get the fd for ready event at index n (0-based). */
int64_t zeta_tokio_reactor_event_fd(int64_t n) {
    if (n >= 0 && n < g_ready_count)
        return (int64_t)g_events[n].data.fd;
    return -1;
}

/* Get event flags for ready event at index n. */
int64_t zeta_tokio_reactor_event_flags(int64_t n) {
    if (n < 0 || n >= g_ready_count) return -1;
    uint32_t ev = g_events[n].events;
    int64_t flags = 0;
    if (ev & EPOLLIN)   flags |= ZT_READABLE;
    if (ev & EPOLLOUT)  flags |= ZT_WRITABLE;
    if (ev & (EPOLLERR | EPOLLHUP)) flags |= ZT_ERROR;
    return flags;
}

/* Destroy reactor. */
void zeta_tokio_reactor_destroy(int64_t epfd) {
    close((int)epfd);
}

/* ── Waker (pipe-based) ── */

/* Create a waker pipe pair. Returns read fd, or -1. */
int64_t zeta_tokio_waker_create(void) {
    int fds[2];
    if (pipe2(fds, O_CLOEXEC | O_NONBLOCK) < 0)
        return -1;
    return (int64_t)fds[0]; /* return read end; write end stored in fd+1 */
}

/* Get the write end of a waker pipe. */
int64_t zeta_tokio_waker_write_fd(int64_t read_fd) {
    return read_fd + 1; /* pipe2 gives adjacent fds */
}

/* Wake a waker by writing 1 byte to its pipe. */
int64_t zeta_tokio_waker_wake(int64_t read_fd) {
    int write_fd = (int)read_fd + 1;
    char byte = 1;
    ssize_t rc = write(write_fd, &byte, 1);
    return (rc > 0) ? 0 : -1;
}

/* Consume waker event (drain the pipe). */
int64_t zeta_tokio_waker_consume(int64_t read_fd) {
    char buf[8];
    ssize_t rc = read((int)read_fd, buf, 8);
    return (rc > 0) ? 0 : -1;
}

/* Destroy waker pipe. */
void zeta_tokio_waker_destroy(int64_t read_fd) {
    close((int)read_fd);
    close((int)read_fd + 1);
}

/* ── Non-blocking I/O helpers ── */

/* Set a fd to non-blocking mode. Returns 0 or -1. */
int64_t zeta_tokio_set_nonblocking(int64_t fd) {
    int flags = fcntl((int)fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return (int64_t)fcntl((int)fd, F_SETFL, flags | O_NONBLOCK);
}

/* ── Timerfd ── */

/* Create a monotonic timerfd. Returns fd or -1. */
int64_t zeta_tokio_timerfd_create(void) {
    return (int64_t)timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
}

/* Arm timerfd to fire after `ns` nanoseconds. Returns 0 or -1. */
int64_t zeta_tokio_timerfd_set(int64_t fd, int64_t ns) {
    struct itimerspec spec;
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 0;
    spec.it_value.tv_sec = ns / 1000000000;
    spec.it_value.tv_nsec = ns % 1000000000;
    return (int64_t)timerfd_settime((int)fd, 0, &spec, NULL);
}

/* Arm timerfd at absolute monotonic time (nanoseconds). Returns 0 or -1. */
int64_t zeta_tokio_timerfd_set_absolute(int64_t fd, int64_t abs_ns) {
    struct itimerspec spec;
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 0;
    spec.it_value.tv_sec = abs_ns / 1000000000;
    spec.it_value.tv_nsec = abs_ns % 1000000000;
    return (int64_t)timerfd_settime((int)fd, TFD_TIMER_ABSTIME, &spec, NULL);
}

/* Read (clear/consume) a timerfd. Returns number of expirations. */
int64_t zeta_tokio_timerfd_read(int64_t fd) {
    uint64_t val;
    ssize_t rc = read((int)fd, &val, sizeof(val));
    if (rc > 0) return (int64_t)val;
    return -1;
}

/* Get monotonic time in nanoseconds. */
int64_t zeta_tokio_monotonic_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
        return (int64_t)ts.tv_sec * 1000000000 + (int64_t)ts.tv_nsec;
    return -1;
}
