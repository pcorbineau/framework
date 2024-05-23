#include "window.hpp"

int main() {
    auto window = Window::create(
        Window::Config{.title = "Hello, World!", .width = 800, .height = 600});
    if (!window) {
        return 1;
    }
    window->show();

    // Loop::run();

    return 0;
}
