#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

// 一个简单的线程安全队列模板
template <typename T>
class CommandQueue {
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(value));
        _cond.notify_one();
    }

    // 等待并弹出一个元素
    bool wait_and_pop(T& value) {
        // 这里需要使用 unique_lock<std::mutex> 的原因是：
        // 条件变量（std::condition_variable）的 wait() 系列接口要求必须传入一个 std::unique_lock<std::mutex>&，
        // 因为 wait() 需要在内部：1.原子地解锁互斥量。2.阻塞当前线程直到被通知。3.再次加锁互斥量。
        // unique_lock<std::mutex> 提供更丰富的锁管理：1.手动调用 lock()／unlock()。2.延迟（defer）加锁。3.转移所有权（movable）。这些lock_guard<std::mutex> 不具备。
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this] { return !_queue.empty() || _is_stopping; });
        if (_is_stopping && _queue.empty()) {
            return false;
        }
        value = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _is_stopping = true;
        }
        _cond.notify_all();
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
    bool _is_stopping = false;
};