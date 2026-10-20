#include "events.hpp"

namespace r2d2 {

void EventQueue::push(const Event& e) {
    std::scoped_lock lock(mutex_);
    queue_.push(e);
}

bool EventQueue::poll(Event& out) {
    std::scoped_lock lock(mutex_);
    if (queue_.empty())
        return false;

    out = queue_.front();
    queue_.pop();
    return true;
}

void EventQueue::clear() {
    std::scoped_lock lock(mutex_);
    while (!queue_.empty())
        queue_.pop();
}

} // namespace r2d2
