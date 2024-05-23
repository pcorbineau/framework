#include "loop.hpp"
#include "window.hpp"

int main() {
    Window window_array[3] = {
        Window::create(
            Window::Config{.title = "Window 1", .width = 400, .height = 300})
            .value(),
        Window::create(
            Window::Config{.title = "Window 2", .width = 400, .height = 300})
            .value(),
        Window::create(
            Window::Config{.title = "Window 3", .width = 400, .height = 300})
            .value()};
    for (auto &w : window_array) {
        w.show();
    }

    Loop::run();

    return 0;
}
