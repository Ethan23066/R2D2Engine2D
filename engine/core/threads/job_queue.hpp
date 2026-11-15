#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>
namespace r2d2 {
// close() wakes waiters; queued items remain readable. reset() requires no active consumers.
template<class T> class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    bool push(T value) {
        { std::lock_guard<std::mutex> lock(mutex_); if (closed_) return false; queue_.push(std::move(value)); }
        cond_.notify_one(); return true;
    }
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        value = std::move(queue_.front()); queue_.pop(); return true;
    }
    bool wait_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return closed_ || !queue_.empty(); });
        if (queue_.empty()) return false;
        value = std::move(queue_.front()); queue_.pop(); return true;
    }
    void close() noexcept {
        { std::lock_guard<std::mutex> lock(mutex_); closed_ = true; }
        cond_.notify_all();
    }
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) queue_.pop();
        closed_ = false;
    }
    bool empty() const { std::lock_guard<std::mutex> lock(mutex_); return queue_.empty(); }
private:
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<T> queue_;
    bool closed_{false};
};
} // namespace r2d2
