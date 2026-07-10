#pragma once

#include <queue>
#include <cstdint>

namespace r2d2 {

enum class EventType : uint8_t {
    NONE = 0,
    KEY_DOWN,
    KEY_UP,
    MOUSE_MOVE,
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    WINDOW_CLOSE,
};

struct Event {
    EventType type;
    int a;
    int b;

    Event() : type(EventType::NONE), a(0), b(0) {}
    Event(EventType t, int a_, int b_) : type(t), a(a_), b(b_) {}
};

class EventQueue {
public:
    void push(const Event& e);
    bool poll(Event& out);
    void clear();

private:
    std::queue<Event> queue_;
};

} // namespace r2d2
