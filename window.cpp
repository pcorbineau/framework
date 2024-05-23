#include "window.hpp"
#include <expected>
#include <print>

namespace {

static constexpr auto WINDOW_CLASS_NAME = "FRAMEWORK_WINDOW_CLASS";

LRESULT CALLBACK WindowProcStatic(HWND hwnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam) {
    auto window =
        reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (window) {
        return window->windowProc(hwnd, uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

auto register_class() -> std::expected<WNDCLASS, Window::Error> {
    std::print("Registering window class\n");
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProcStatic;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClass(&wc)) {
        return std::unexpected(Window::Error::RegisterClassFailed);
    }

    return wc;
}

auto framework_window_cout() {
    // search for all FRAMEWORK_WINDOW_CLASS in the system
    HWND hwnd = nullptr;
    auto count = 0;
    while ((hwnd = FindWindowEx(nullptr, hwnd, WINDOW_CLASS_NAME, nullptr))) {
        count++;
    }

    return count;
}

} // namespace

std::expected<Window, Window::Error> Window::create(const Config &config) {
    static auto window_class = register_class();
    return window_class.transform(
        [&config](const auto &wndclass) { return Window(config, wndclass); });
}

Window::Window(const Config &config, const WNDCLASS &wndclass)
    : m_Hwnd(CreateWindow(wndclass.lpszClassName, config.title.data(),
                          WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          config.width, config.height, nullptr, nullptr,
                          GetModuleHandle(nullptr), this)) {
    SetWindowLongPtr(m_Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void Window::show() const noexcept {
    ShowWindow(m_Hwnd, SW_SHOW);
    UpdateWindow(m_Hwnd);
}

auto Window::get_title() const noexcept -> std::string {
    std::string title(256, '\0');
    GetWindowText(m_Hwnd, title.data(), static_cast<int>(title.size()));
    return title;
}

LRESULT Window::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE: {
        std::print("Window '{}' closed\n", get_title());
        DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY: { // Quit on last window destroyed
        auto count = framework_window_cout();
        std::print("Window destroyed, remaining: {}\n", count);
        if (count == 1) {
            PostQuitMessage(0);
        }
        break;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
