# mini-pthread – Custom Threading Library in C

![C](https://img.shields.io/badge/C-Language-blue)
![Concurrency](https://img.shields.io/badge/Threads-User_Level-purple)
![Synchronization](https://img.shields.io/badge/Mutexes_&_Semaphores-green)
![Operating Systems](https://img.shields.io/badge/OS-Concepts-orange)

**mini-pthread** is a lightweight implementation of a threading library that replicates the core behaviour of POSIX `pthread`s at the **user level**, built from scratch in C. It supports cooperative multitasking, thread lifecycle control, manual context switching, and basic synchronization primitives.

---

## Features

- `thread_create` – Create new user-level threads
- `thread_exit` – Terminate the calling thread
- `thread_join` – Wait for a thread to finish
- `thread_sleep(ms)` – Sleep a thread for a given duration
- `thread_yield()` – Yield execution to other threads
- `context_switch()` – Explicit context switching between threads

### Synchronisation Support

- **Mutexes**:
  - `mutex_init(m)`
  - `mutex_acquire(m)`
  - `mutex_release(m)`
- **Semaphores**:
  - `sem_init(sem, value)`
  - `sem_wait(sem)`
  - `sem_post(sem)`

---

## How It Works

- Implements cooperative (non-preemptive) threading using `setjmp` / `longjmp` or `ucontext` for context switching
- Threads are managed in a queue with a simple scheduler
- Each thread has its own stack and context
- Mutexes and semaphores block/yield threads without kernel involvement

