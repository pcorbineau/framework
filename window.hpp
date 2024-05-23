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

    static auto create(const Config &config) -> std::expected<Window, Error>;

    void show() const noexcept;
    [[nodiscard]] auto get_title() const noexcept -> std::string;
    void toggle_fullscreen() noexcept;

    auto windowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                    LPARAM lParam) -> LRESULT;

  private:
    Window(const Config &config, const WNDCLASS &wndclass);

    void set_window_mode(const CreateWindowConfig &mode_config) noexcept;

    HWND m_Hwnd;
    WINDOWPLACEMENT m_wpPrev{sizeof(WINDOWPLACEMENT)};
};
