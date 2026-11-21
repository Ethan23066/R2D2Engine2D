#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

namespace r2d2 {

// File thread-safe avec capacité optionnelle et retour via std::optional.
// reset() nécessite qu'aucun consommateur ne soit activement bloqué dans wait_pop().
template <typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(std::size_t max_capacity = 0) noexcept
        : max_capacity_(max_capacity) {}

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    bool push(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) {
                return false;
            }
            if (max_capacity_ > 0 && queue_.size() >= max_capacity_) {
                return false;
            }
            queue_.push(std::move(value));
        }
        cond_.notify_one();
        return true;
    }

    [[nodiscard]] std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        std::optional<T> result(std::move(queue_.front()));
        queue_.pop();
        return result;
    }

    [[nodiscard]] std::optional<T> wait_pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return closed_ || !queue_.empty(); });
        if (queue_.empty()) {
            return std::nullopt;
        }
        std::optional<T> result(std::move(queue_.front()));
        queue_.pop();
        return result;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        cond_.notify_all();
    }

    void reset() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::queue<T> empty_queue;
            queue_.swap(empty_queue);
            closed_ = false;
        }
        cond_.notify_all();
    }

    [[nodiscard]] bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    [[nodiscard]] std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    [[nodiscard]] bool is_closed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<T> queue_;
    std::size_t max_capacity_{0};
    bool closed_{false};
};

} // namespace r2d2
