#include "window.hpp"
#include "window_mode.hpp"
#include <expected>
#include <print>
#include <variant>

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
                          CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr,
                          GetModuleHandle(nullptr), this)) {
    SetWindowLongPtr(m_Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(m_Hwnd, SW_SHOW);
    auto mode_config = std::visit(WindowModeVisitor{}, config.mode);
    set_window_mode(mode_config);
}

void Window::set_window_mode(const CreateWindowConfig &mode_config) noexcept {
    SetWindowLong(m_Hwnd, GWL_STYLE, mode_config.style);
    constexpr auto valid_pos_value = [](auto x, auto y) {
        return x.has_value() && y.has_value();
    };
    auto flags =
        SWP_FRAMECHANGED |
        (valid_pos_value(mode_config.x, mode_config.y) ? 0 : SWP_NOMOVE) |
        (valid_pos_value(mode_config.width, mode_config.height) ? 0
                                                                : SWP_NOSIZE);
    SetWindowPos(m_Hwnd, HWND_TOP, mode_config.x.value_or(0),
                 mode_config.y.value_or(0), mode_config.width.value_or(0),
                 mode_config.height.value_or(0), flags);
    ShowWindow(m_Hwnd, SW_SHOW);
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

void Window::toggle_fullscreen() noexcept {
    DWORD dwStyle = GetWindowLong(m_Hwnd, GWL_STYLE);
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = {sizeof(mi)};
        if (GetWindowPlacement(m_Hwnd, &m_wpPrev) &&
            GetMonitorInfo(MonitorFromWindow(m_Hwnd, MONITOR_DEFAULTTOPRIMARY),
                           &mi)) {
            SetWindowLong(m_Hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(m_Hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(m_Hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(m_Hwnd, &m_wpPrev);
        SetWindowPos(m_Hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

LRESULT Window::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE: {
        std::print("Window '{}' closed\n", get_title());
        DestroyWindow(m_Hwnd);
        break;
    }
    case WM_DESTROY: { // Quit on last window destroyed
        auto count = framework_window_cout();
        std::print("Window destroyed, remaining: {}\n", count - 1);
        if (count == 1) {
            std::print("Quitting\n");
            PostQuitMessage(0);
        }
        break;
    }
    case WM_KEYUP: {
        constexpr auto F_KEY = 0x46;
        // quit on ESC key
        if (wParam == VK_ESCAPE) {
            DestroyWindow(m_Hwnd);
        }
        // toggle fullscreen on F
        else if (wParam == F_KEY) {
            toggle_fullscreen();
        }
    }
    default:
        return DefWindowProc(m_Hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
