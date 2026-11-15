#pragma once
#include <atomic>
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
        if (running_.load(std::memory_order_relaxed) || worker_.joinable() || joining_)
            return false;
        queue_.reset();
        running_.store(true, std::memory_order_release);
        try { worker_ = std::thread([this] { run(); }); }
        catch (...) { running_.store(false, std::memory_order_release); queue_.close(); return false; }
        return true;
    }

    void stop() {
        std::thread joining_thread;
        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex_);
            running_.store(false, std::memory_order_release);
            queue_.close();
            if (joining_) return;
            if (!worker_.joinable()) return;
            // A handler may request stop. It can close the queue, but only
            // another thread may join this worker.
            if (worker_.get_id() == std::this_thread::get_id()) return;
            joining_ = true;
            joining_thread = std::move(worker_);
        }
        joining_thread.join();
        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex_);
            joining_ = false;
        }
    }

    bool push(RenderCommand cmd) {
        if (!running_.load(std::memory_order_acquire)) return false;
        return queue_.push(std::move(cmd));
    }
    bool running() const noexcept { return running_.load(std::memory_order_acquire); }

private:
    void run() noexcept {
        RenderCommand cmd;
        while (queue_.wait_pop(cmd)) {
            if (cmd.type == RenderCommandType::Shutdown) break;
            try { if (handler_) handler_(cmd); }
            catch (...) { break; }
        }
        running_.store(false, std::memory_order_release);
    }
    ThreadSafeQueue<RenderCommand> queue_;
    Handler handler_;
    std::atomic<bool> running_{false};
    std::mutex lifecycle_mutex_;
    std::thread worker_;
    bool joining_{false};
};
} // namespace r2d2
