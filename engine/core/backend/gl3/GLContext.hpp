#pragma once

#include <mutex>
#include <thread>

namespace r2d2::gl3_detail {

struct ContextState {
    std::mutex mutex;
    std::thread::id owner{};
    void* window = nullptr;
    bool active = false;
};

inline ContextState& context_state() {
    static ContextState state;
    return state;
}

inline bool activate_context(std::thread::id owner, void* window) {
    auto& state = context_state();
    std::lock_guard lock(state.mutex);
    if (state.active || owner == std::thread::id{} || !window)
        return false;

    state.owner = owner;
    state.window = window;
    state.active = true;
    return true;
}

inline void deactivate_context(std::thread::id owner, void* window) {
    auto& state = context_state();
    std::lock_guard lock(state.mutex);
    if (state.active && state.owner == owner && state.window == window) {
        state.owner = {};
        state.window = nullptr;
        state.active = false;
    }
}

inline bool context_active() {
    auto& state = context_state();
    std::lock_guard lock(state.mutex);
    return state.active;
}

inline bool is_context_owner_thread() {
    auto& state = context_state();
    std::lock_guard lock(state.mutex);
    return state.active && state.owner == std::this_thread::get_id();
}

inline bool is_current_context_owner(void* current_window) {
    auto& state = context_state();
    std::lock_guard lock(state.mutex);
    return state.active
        && state.owner == std::this_thread::get_id()
        && state.window == current_window
        && current_window != nullptr;
}

} // namespace r2d2::gl3_detail
