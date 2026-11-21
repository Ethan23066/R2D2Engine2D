#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>

#include "engine/core/render/render_command.hpp"
#include "engine/core/threads/job_queue.hpp"

namespace r2d2 {

class RenderThread {
public:
    using Handler = std::function<void(const RenderCommand&)>;

    explicit RenderThread(Handler handler = {}) : handler_(std::move(handler)) {}
    ~RenderThread() { stop(); }

    RenderThread(const RenderThread&) = delete;
    RenderThread& operator=(const RenderThread&) = delete;

    bool start() {
        std::lock_guard<std::mutex> lock(lifecycle_mutex_);
        if (running_.load(std::memory_order_relaxed) || worker_.joinable() || joining_) {
            return false;
        }
        queue_.reset();
        running_.store(true, std::memory_order_release);
        try {
            worker_ = std::thread([this] { run(); });
        } catch (...) {
            running_.store(false, std::memory_order_release);
            queue_.close();
            return false;
        }
        return true;
    }

    void stop() {
        std::thread joining_thread;
        {
            std::unique_lock<std::mutex> lock(lifecycle_mutex_);
            running_.store(false, std::memory_order_release);
            queue_.close();

            if (worker_id_ == std::this_thread::get_id()) {
                return;
            }
            if (joining_) {
                lifecycle_cv_.wait(lock, [this] { return !joining_; });
                return;
            }
            if (!worker_.joinable()) {
                return;
            }
            joining_ = true;
            joining_thread = std::move(worker_);
        }

        joining_thread.join();

        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex_);
            joining_ = false;
            worker_id_ = {};
        }
        lifecycle_cv_.notify_all();
    }

    bool push(RenderCommand cmd) {
        if (!running_.load(std::memory_order_acquire)) {
            return false;
        }
        return queue_.push(std::move(cmd));
    }

    bool running() const noexcept {
        return running_.load(std::memory_order_acquire);
    }

private:
    void run() noexcept {
        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex_);
            worker_id_ = std::this_thread::get_id();
        }

        while (auto cmd = queue_.wait_pop()) {
            if (cmd->type == RenderCommandType::Shutdown) {
                break;
            }
            try {
                if (handler_) {
                    handler_(*cmd);
                }
            } catch (...) {
                break;
            }
        }

        running_.store(false, std::memory_order_release);
    }

    ThreadSafeQueue<RenderCommand> queue_;
    Handler handler_;
    std::atomic<bool> running_{false};
    std::mutex lifecycle_mutex_;
    std::condition_variable lifecycle_cv_;
    std::thread worker_;
    std::thread::id worker_id_{};
    bool joining_{false};
};

} // namespace r2d2