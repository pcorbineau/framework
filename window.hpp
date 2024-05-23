#pragma once
#include <Windows.h>
#include <expected>
#include <string>
#include <string_view>


class Window {
  public:
    struct Config {
        std::string_view title;
        int width;
        int height;
    };

    enum class Error {
        RegisterClassFailed,
        CreateWindowFailed,
    };

    static std::expected<Window, Error> create(const Config &config);

    void show() const noexcept;
    auto get_title() const noexcept -> std::string;

    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    Window(const Config &config, const WNDCLASS &wndclass);

    HWND m_Hwnd;
};
