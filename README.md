<h1 align="center"> ntrusive </h1>

---

An ergonomic, header-only intrusive doubly linked list.

This project was developed to support intrusiveness in my [concurrency runtime](https://github.com/niko256/renn/tree/main).

---

## Motivation

When building a task scheduler or executor, you usually end up with a container of tasks. The standard way to do this is to represent your task (or inner subroutine) using `std::function` (or similar).

```cpp

struct MyFiber {
  std::function<void()> inner_routine_;

  /*
   * ...
   */
  static void yield() {
    /* creating a new state/closure just to reschedule */
    executor_.schedule([this]() { this->resume(); })
  }
};

struct Executor {
  Container<MyFiber> tasks_;

  void schedule(std::function<void()> task) {
    /* this allocation happens on every push */
    tasks_.push(std::move(task));
  }
};
```

There are many unnecessary allocations occurring here.

With this library, the callable entity (your fiber, coroutine, future, etc.) doesn't need to store a closure. Instead, the intrusive list node itself stores all necessary state and is responsible for its own linkage.

The state is allocated exactly once when the object is constructed (a single heap allocation). The linkage metadata is embedded directly within the object (since the object inherits from the list node). When a task yields, you simply pass a pointer to the existing object back to the executor.

This results in zero allocations during runtime operations.

---

## Usage

To make an object intrusive, inherit from `IntrusiveListNode`.

```cpp
#include <ntrusive/intrusive.hpp>

// 1. Inherit your task from the intrusive node
struct TaskBase : IntrusiveListNode {
    virtual void run() = 0;

  protected:
    ~TaskBase() = default;
};

// 2. Implement concrete tasks
struct Fiber : TaskBase {
    int id;

    explicit Fiber(int i) : id(i) {}

    void run() override {
        /* execution logic... */
    }
};
```

---

## Integration

The library is header-only. You can include it via `FetchContent` in your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  ntrusive
    GIT_REPOSITORY https://github.com/niko256/ntrusive.git
    GIT_TAG master
)
FetchContent_MakeAvailable(ntrusive)

target_link_libraries(your_target PRIVATE ntrusive::ntrusive)
```
