#pragma once
#include <Windows.h>
#include <expected>
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

  private:
    Window(HWND hwnd) noexcept : m_Hwnd(hwnd) {}

    HWND m_Hwnd;
};
