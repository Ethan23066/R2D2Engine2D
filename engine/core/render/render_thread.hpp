#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <utility>

#include "engine/core/render/render_command.hpp"
#include "engine/core/threads/job_queue.hpp"

namespace r2d2 {

class RenderThread {
public:
    using Handler = std::function<void(const RenderCommand&)>;

    explicit RenderThread(Handler handler = {})
        : handler_(std::move(handler)) {}

    ~RenderThread() {
        stop();
    }

    void start() {
        if (running_.load(std::memory_order_acquire))
            return;

        running_.store(true, std::memory_order_release);
        worker_ = std::thread([this] { run(); });
    }

    void stop() {
        if (!running_.load(std::memory_order_acquire))
            return;

        running_.store(false, std::memory_order_release);
        queue_.push(RenderCommand::shutdown());

        if (worker_.joinable())
            worker_.join();
    }

    void push(RenderCommand cmd) {
        if (!running_.load(std::memory_order_acquire))
            return;

        queue_.push(std::move(cmd));
    }

    bool running() const noexcept {
        return running_.load(std::memory_order_acquire);
    }

private:
    void run() {
        while (running_.load(std::memory_order_acquire)) {
            RenderCommand cmd;
            if (!queue_.wait_pop(cmd))
                continue;

            if (cmd.type == RenderCommandType::Shutdown) {
                if (handler_)
                    handler_(cmd);
                running_.store(false, std::memory_order_release);
                continue;
            }

            if (handler_)
                handler_(cmd);
        }
    }

private:
    ThreadSafeQueue<RenderCommand> queue_;
    Handler handler_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

} // namespace r2d2
