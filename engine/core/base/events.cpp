#include "events.hpp"

namespace r2d2 {

void EventQueue::push(const Event& e) {
    queue_.push(e);
}

bool EventQueue::poll(Event& out) {
    if (queue_.empty())
        return false;

    out = queue_.front();
    queue_.pop();
    return true;
}

void EventQueue::clear() {
    while (!queue_.empty())
        queue_.pop();
}

} // namespace r2d2
