#pragma once
#include "window_mode.hpp"
#include <Windows.h>
#include <expected>
#include <string>
#include <string_view>

class Window {
  public:
    struct Config {
        std::string_view title;
        Mode mode;
    };

    enum class Error {
        RegisterClassFailed,
        CreateWindowFailed,
    };

    static std::expected<Window, Error> create(const Config &config);

    void show() const noexcept;
    std::string get_title() const noexcept;
    void toggle_fullscreen() noexcept;

    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    Window(const Config &config, const WNDCLASS &wndclass);

    void set_window_mode(const CreateWindowConfig &mode_config) noexcept;

    HWND m_Hwnd;
    WINDOWPLACEMENT m_wpPrev{sizeof(WINDOWPLACEMENT)};
};
