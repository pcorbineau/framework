#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <variant>
#include <windows.h>

struct Windowed {
    std::optional<std::uint32_t> width;
    std::optional<std::uint32_t> height;
};
struct ExclusiveFullscreen {};
struct Borderless {};

using Mode = std::variant<Windowed, ExclusiveFullscreen, Borderless>;

struct CreateWindowConfig {
    std::optional<std::int32_t> x;
    std::optional<std::int32_t> y;
    std::optional<std::uint32_t> width;
    std::optional<std::uint32_t> height;
    std::uint32_t style{};
};

struct WindowModeVisitor {
    auto operator()(const Windowed &windowed) -> CreateWindowConfig {
        return {
            .width = windowed.width,
            .height = windowed.height,
            .style = WS_OVERLAPPEDWINDOW,
        };
    }

    auto operator()([[maybe_unused]] const ExclusiveFullscreen &fullscreen)
        -> CreateWindowConfig {
        throw std::runtime_error("Not implemented");
    }

    auto operator()([[maybe_unused]] const Borderless &borderless)
        -> CreateWindowConfig {
        return {
            .x = 0,
            .y = 0,
            .width = static_cast<std::uint32_t>(GetSystemMetrics(SM_CXSCREEN)),
            .height = static_cast<std::uint32_t>(GetSystemMetrics(SM_CYSCREEN)),
            .style = WS_POPUP,
        };
    }
};
