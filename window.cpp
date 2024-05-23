#include "window.hpp"
#include <expected>

namespace {
auto register_class() -> std::expected<WNDCLASS, Window::Error> {
    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "FRAMEWORK_WINDOW_CLASS";

    if (!RegisterClass(&wc)) {
        return std::unexpected(Window::Error::RegisterClassFailed);
    }

    return wc;
}

} // namespace

std::expected<Window, Window::Error> Window::create(const Config &config) {
    static auto wc = register_class();
    return wc.and_then(
        [&config](auto wc) -> std::expected<Window, Window::Error> {
            auto hwnd = CreateWindow(
                wc.lpszClassName, config.title.data(), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, config.width, config.height,
                nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

            if (!hwnd) {
                return std::unexpected(Error::CreateWindowFailed);
            }

            return Window{hwnd};
        });
}

void Window::show() const noexcept {
    ShowWindow(m_Hwnd, SW_SHOW);
    UpdateWindow(m_Hwnd);
}
