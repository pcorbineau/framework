#include "loop.hpp"
#include "window.hpp"
#include "window_mode.hpp"
#include <array>

auto main() -> int {
    auto window_array = std::array{
        Window::create(Window::Config{.title = "Window 1", .mode = Windowed{}})
            .value(),
        Window::create(
            Window::Config{.title = "Window 2",
                           .mode = Windowed{.width = 1600, .height = 900}})
            .value(),
        Window::create(
            Window::Config{.title = "Window 3", .mode = Borderless{}})
            .value(),
    };

    for (auto &w : window_array) {
        w.show();
    }

    Loop::run();

    return 0;
}
