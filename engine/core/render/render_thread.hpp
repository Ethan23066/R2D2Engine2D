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
        if (running_.load(std::memory_order_relaxed) || worker_.joinable()) return false;
        queue_.reset();
        running_.store(true, std::memory_order_release);
        try { worker_ = std::thread([this] { run(); }); }
        catch (...) { running_.store(false, std::memory_order_release); queue_.close(); return false; }
        return true;
    }
    void stop() {
        std::lock_guard<std::mutex> lock(lifecycle_mutex_);
        running_.store(false, std::memory_order_release);
        queue_.close();
        if (worker_.joinable()) {
            // stop() must be called by an owning thread, never by its own handler.
            if (worker_.get_id() == std::this_thread::get_id()) return;
            worker_.join();
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
            catch (...) { break; } // Never allow an exception to escape a thread entry point.
        }
        running_.store(false, std::memory_order_release);
    }
    ThreadSafeQueue<RenderCommand> queue_;
    Handler handler_;
    std::atomic<bool> running_{false};
    std::mutex lifecycle_mutex_;
    std::thread worker_;
};
} // namespace r2d2
