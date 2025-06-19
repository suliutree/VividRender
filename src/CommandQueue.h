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