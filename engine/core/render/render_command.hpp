#pragma once
namespace r2d2 {
enum class RenderCommandType { None, Shutdown };
struct RenderCommand {
    RenderCommandType type{RenderCommandType::None};
    static RenderCommand shutdown() noexcept { return {RenderCommandType::Shutdown}; }
};
} // namespace r2d2
