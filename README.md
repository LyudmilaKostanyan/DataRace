# Race Conditions

## Overview

This project demonstrates how race conditions can occur in multithreaded programs and how to detect and fix them. It includes:

* A faulty version with a race condition using a shared counter.
* A fixed version using `std::atomic<int>`.
* A fixed version using `std::mutex`.
* A performance comparison between atomic and mutex-based synchronization.
* ThreadSanitizer integration for detecting race conditions.

---

## What is a Data Race?

A **data race** happens when:

* Two or more threads access the same memory location concurrently.
* At least one of the accesses is a write.
* There is no proper synchronization (e.g. no mutex or atomic protection).

This causes **undefined behavior** — the program might produce incorrect results, crash, or behave differently each time it runs. Data races are subtle and often hard to reproduce, which makes them dangerous in concurrent systems.

Proper synchronization using tools like `std::mutex`, `std::atomic`, or other concurrency primitives prevents data races.

---

## Problem Description

When multiple threads access and modify a shared variable without proper synchronization, **race conditions** may occur, leading to unpredictable or incorrect program behavior.

In this project, two threads increment a shared counter 1,000,000 times each (total expected: 2,000,000). The faulty version shows how incorrect final values may arise due to race conditions. We use ThreadSanitizer to detect these issues and compare fixes using both atomic operations and mutex locking.

---

## Example Output

```
Test        Final Counter      Time (s)       Comment
------------------------------------------------------------
race        1745853            0.02481         Race Detected
mutex       2000000            0.11234         OK
atomic      2000000            0.03512         OK
```

---

## Explanation of Output

* **Race**: Final counter is incorrect due to unsynchronized access by multiple threads.
* **Mutex**: Ensures safe access to the counter using `std::mutex`, but has more overhead.
* **Atomic**: Fixes the race with less overhead than a mutex by using `std::atomic<int>`.

---

## How to Compile and Run

### 1. Clone the Repository

```bash
git clone https://github.com/LyudmilaKostanyan/DataRace.git
cd DataRace
```

### 2. Build the Project

Use CMake to build the project:

```bash
cmake -S . -B build
cmake --build build
```

Ensure you have CMake and a C++ compiler that supports C++17.

### 3. Run the Program

```bash
cd build
./main
```

You will see a table comparing all three implementations.

---

### Running with ThreadSanitizer

You can use ThreadSanitizer to detect the race condition in the `race` implementation.

#### For Linux or macOS:

```bash
g++ -fsanitize=thread -g -std=c++17 main.cpp
./main
```

Look for messages indicating a **data race** on `counter_unsync`.
