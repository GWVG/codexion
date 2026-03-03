*This project has been created as part of the 42 curriculum by gert-willemvangompel.*

# Codexion

## Description

Codexion is a multithreaded C project inspired by the Dining Philosophers problem.
Multiple coders compete for shared dongles (resources), and each coder must compile
often enough to avoid burnout.

The goal is to implement a correct, deterministic, and thread-safe simulation that:
- avoids unsafe concurrent access,
- handles blocking conditions predictably,
- reports lifecycle events through serialized logs,
- supports FIFO and EDF arbitration strategies.

At runtime, each coder repeatedly:
1. takes two dongles,
2. compiles,
3. debugs,
4. refactors.

A monitor thread detects termination conditions:
- a coder burned out,
- or all coders reached the required number of compiles.

## Instructions

### Build

From the repository root:
- `make` builds `codexion`
- `make asan` builds `codexion_asan` (AddressSanitizer)
- `make run` runs default smoke scenario (`SMOKE_ARGS`)
- `make asan-run` runs ASan build (macOS-safe defaults)
- `make valgrind-run` runs Valgrind (Linux/evaluator systems)
- `make clean`, `make fclean`, `make re`

### Execute

Program format:

`./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler`

Constraints:
- `number_of_coders` > 0
- `time_to_burnout` > 0
- `time_to_compile` > 0
- `time_to_debug` > 0
- `time_to_refactor` > 0
- `number_of_compiles_required` >= 0
- `dongle_cooldown` >= 0
- `scheduler` is `fifo` or `edf`

Example:

`./codexion 5 1200 200 200 200 2 10 fifo`

## Blocking cases handled

This implementation addresses the following concurrency and blocking cases:

1. **Deadlock prevention**
	- Dongles are acquired in a consistent global order (by dongle address ordering),
	  which breaks circular wait and removes one of Coffman's deadlock conditions.
	- Wait queues per dongle ensure orderly acquisition attempts.

2. **Starvation mitigation**
	- FIFO mode uses arrival sequence ordering.
	- EDF mode prioritizes earliest burnout deadline and uses arrival order as tie-breaker.
	- This reduces long-term unfairness versus naive lock racing.

3. **Cooldown handling**
	- On release, dongles enter a cooldown interval.
	- Coders only acquire when dongle is available, cooldown expired, and waiter is at queue head.

4. **Precise burnout detection**
	- Monitor regularly checks each coder's last compile start timestamp.
	- Burnout is detected in millisecond resolution against `time_to_burnout`.

5. **Stop propagation for blocked threads**
	- When stop is set, all dongle waiters are awakened so threads blocked on waits can exit cleanly.

6. **Log serialization**
	- All logs are protected by a print mutex to avoid interleaved/corrupted lines.
	- Burnout line is allowed as the final decisive event.

## Thread synchronization mechanisms

The implementation coordinates shared state with standard pthread primitives:

1. **`pthread_mutex_t`**
	- `stop_mutex`: protects global stop flag reads/writes.
	- `print_mutex`: serializes stdout logging.
	- `coder.state_mutex`: protects per-coder mutable state
	  (`last_compile_start_ms`, `compile_count`, `compiling`).
	- `dongle.mtx`: protects dongle availability, cooldown timestamp, arrival sequence,
	  and wait queue operations.

2. **`pthread_cond_t`**
	- One condition variable per dongle for waiter coordination.
	- Threads waiting for dongle availability/cooldown can sleep and be awakened
	  on release or global stop wake-up.

3. **Custom coordination logic**
	- Custom min-heap wait queues per dongle for FIFO/EDF arbitration.
	- Monitor-driven stop signal plus explicit wake-all on stop to unblock waiting coders.

Race-condition prevention examples:
- Coder state is never read/written without `state_mutex`.
- Stop flag is never read/written without `stop_mutex`.
- Queue head checks and dongle take/release are always done under `dongle.mtx`.

Thread-safe communication between coders and monitor:
- Coders update their state under mutexes.
- Monitor reads those states under matching mutexes.
- Monitor triggers global stop; waiting coders are then awakened and terminate gracefully.

## Resources

Classic references used for this topic:
- POSIX threads and condition variables:
  - `pthread_create(3)`, `pthread_mutex_*`, `pthread_cond_*` man pages
  - https://man7.org/linux/man-pages/man3/pthread_cond_init.3.html
- Priority queues / heaps:
  - https://www.geeksforgeeks.org/c/heap-in-c/
  - https://www.geeksforgeeks.org/dsa/building-heap-from-array/
- Concurrency background and starvation/fairness concepts:
  - Operating systems and synchronization course material

### AI usage disclosure

AI was used as a development assistant for:
- refining architecture and implementation sequencing,
- reviewing thread/stop-flow logic,
- drafting and revising Makefile targets,
- preparing README structure and wording.

All code decisions, integration, testing, and final validation were manually reviewed and executed in the local project environment.

