#include <algorithm>
#include <chrono>
#include <execution>
#include <glad/glad.h>
#include <opencv2/opencv.hpp>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <windows.h>
#include "Base/Image.hpp"
#include "Base/Province.hpp"
#include "Base/Utils.hpp"
#include "Tags/Country.hpp"
#include "Tags/Tag.hpp"

#include <commctrl.h>

#pragma comment(                                                                                                       \
        linker,                                                                                                        \
        "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int offset[2] = {0, 0};
double zoom = 1.0;
int previous_mouse[2] = {0, 0};
bool mouse_down = false;
bool mouse_moved = false;
bool open = true;
double progress = 0.0;

HWND map_hwnd = nullptr;
HWND gui_hwnd = nullptr;
HGLRC opengl_hdc = nullptr;
GLuint texture = 0;
GLuint read_fbo_id = 0;
std::vector<BYTE> bytes;
image map_image;

auto provinces = std::unordered_map<unsigned int, std::unique_ptr<province> >();
auto tags = std::unordered_map<unsigned int, std::unique_ptr<tag> >();

province *selected_province = nullptr;
auto map_mode = map_modes::owner;
date date;

inline void set_pixel(std::vector<BYTE> &bytes, const int index, const BYTE r, const BYTE g, const BYTE b) {
    bytes[index] = r;
    bytes[index + 1] = g;
    bytes[index + 2] = b;
    bytes[index + 3] = 255;
}

void reload_bitmap_province(const province &reload_province) {
    const auto color = reload_province.color;
    const auto pixels = reload_province.get_pixels();

    std::vector<int> indices(reload_province.get_num_pixels());
    std::iota(indices.begin(), indices.end(), 0);

    std::for_each(std::execution::par, indices.begin(), indices.end(), [pixels, color](const int i) {
        const auto pixel = pixels[i];
        const auto index = (pixel[0] + pixel[1] * map_image.width) * 4;
        set_pixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
                  static_cast<BYTE>(color >> 16));
    });

    const auto outline = reload_province.get_outline();
    auto updated_provinces = std::unordered_set<province *>();
    for (auto i = 0; i < reload_province.get_num_outline(); ++i) {
        const auto pixel = outline[i].second;
        const auto other_province = outline[i].first;
        const auto index = (pixel[0] + pixel[1] * map_image.width) * 4;
        if (selected_province != nullptr && reload_province.base_color == selected_province->base_color) {
            set_pixel(bytes, index, 255, 255, 255);
        } else {
            if (other_province != nullptr && other_province->color != color) {
                set_pixel(bytes, index, 0, 0, 0);
            } else {
                set_pixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
                          static_cast<BYTE>(color >> 16));
            }
        }
        if (!updated_provinces.contains(other_province)) {
            updated_provinces.insert(other_province);
            if (other_province == nullptr) {
                return;
            }
            const auto other_outline = other_province->get_outline();
            for (unsigned int j = 0; j < other_province->get_num_outline(); ++j) {
                const auto other_pixel = other_outline[j].second;
                const auto other_index = (other_pixel[0] + other_pixel[1] * map_image.width) * 4;
                if (other_outline[j].first == &const_cast<province &>(reload_province)) {
                    if (other_province->color == color) {
                        set_pixel(bytes, other_index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
                                  static_cast<BYTE>(color >> 16));
                    } else {
                        set_pixel(bytes, other_index, 0, 0, 0);
                    }
                }
            }
        }
    }

    const auto hdc = GetDC(map_hwnd);

    wglMakeCurrent(hdc, opengl_hdc);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, reload_province.get_bounds()[1], map_image.width,
                    reload_province.get_bounds()[3] - reload_province.get_bounds()[1], GL_BGRA, GL_UNSIGNED_BYTE,
                    bytes.data() + map_image.width * reload_province.get_bounds()[1] * 4);

    ReleaseDC(map_hwnd, hdc);
}

void reload_bitmap() {
    const auto province_values = provinces | std::views::values;

    std::for_each(
        std::execution::par, province_values.begin(), province_values.end(),
        [](const std::unique_ptr<province> &province) {
            const auto pixels = province->get_pixels();

            std::vector<int> indices(province->get_num_pixels());
            std::iota(indices.begin(), indices.end(), 0);

            std::for_each(std::execution::par, indices.begin(), indices.end(), [pixels, &province](const int i) {
                const auto pixel = pixels[i];
                const auto color = province->color;
                const auto index = (pixel[0] + pixel[1] * map_image.width) * 4;
                set_pixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
                          static_cast<BYTE>(color >> 16));
            });

            const auto outline = province->get_outline();

            indices = std::vector<int>(province->get_num_outline());
            std::iota(indices.begin(), indices.end(), 0);
            std::for_each(std::execution::par, indices.begin(), indices.end(), [outline, &province](const int i) {
                const auto other_province = outline[i].first;
                const auto pixel = outline[i].second;
                const auto color = province->color;
                const auto index = (pixel[0] + pixel[1] * map_image.width) * 4;
                if (selected_province != nullptr && color == selected_province->color) {
                    set_pixel(bytes, index, 255, 255, 255);
                } else {
                    if (other_province != nullptr && other_province->color != color) {
                        set_pixel(bytes, index, 0, 0, 0);
                    } else {
                        set_pixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
                                  static_cast<BYTE>(color >> 16));
                    }
                }
            });
        });

    if (opengl_hdc != nullptr) {
        const auto hdc = GetDC(map_hwnd);

        wglMakeCurrent(hdc, opengl_hdc);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map_image.width, map_image.height, GL_BGRA, GL_UNSIGNED_BYTE,
                        bytes.data());

        ReleaseDC(map_hwnd, hdc);
    }
}

[[noreturn]] DWORD WINAPI load_image(const LPVOID lp_parameter) {
    const auto hwnd = static_cast<HWND>(lp_parameter);
    auto invert_color = [](const unsigned int color_to_change) {
        return (color_to_change & 0xFF) << 16 | (color_to_change & 0xFF00) | (color_to_change & 0xFF0000) >> 16;
    };

    auto process_pixel = [invert_color](const pixel_t &pixel, const int *position) {
        const auto i = position[1];
        const auto j = position[0];
        if (const auto color = pixel.x + (pixel.y << 8) + (pixel.z << 16); provinces.contains(color)) {
            const auto province = provinces.at(color).get();
            province->expand_bounds(i, j);
        } else {
            auto new_country = std::make_unique<country>("", invert_color(color));
            auto new_province = std::make_unique<province>(
                std::string("Province ") + std::to_string(provinces.size()),
                color, i, j);
            new_province->set_owner(*new_country);
            tags.insert(std::make_pair(color, std::move(new_country)));
            provinces.insert(std::make_pair(color, std::move(new_province)));
        }
    };

    auto is_border = [&](const int x, const int y, const unsigned int color, const province *&other_province) {
        if (x >= 0 && x < map_image.width && y >= 0 && y < map_image.height && map_image.get_color(x, y) != color) {
            other_province = provinces.at(map_image.get_color(x, y)).get();
            return true;
        }
        if (x == -1 && y >= 0 && y < map_image.height && map_image.get_color(map_image.width - 1, y) != color) {
            other_province = provinces.at(map_image.get_color(map_image.width - 1, y)).get();
            return true;
        }
        if (x == map_image.width && y >= 0 && y < map_image.height && map_image.get_color(0, y) != color) {
            other_province = provinces.at(map_image.get_color(0, y)).get();
            return true;
        }
        return false;
    };

    auto process_pixel_borders = [is_border](const pixel_t &pixel, const int *position) {
        const auto i = position[1];
        const auto j = position[0];
        const auto color = pixel.x + (pixel.y << 8) + (pixel.z << 16);
        province *province_at_pos = provinces.at(color).get();

        if (const province *other_province = nullptr;
            is_border(i - 1, j, color, other_province) || is_border(i + 1, j, color, other_province) ||
            is_border(i, j - 1, color, other_province) || is_border(i, j + 1, color, other_province)) {
            province_at_pos->add_outline(i, j, *other_province);
        } else {
            province_at_pos->add_pixel(i, j);
        }
    };

    map_image = image("assets/provinces1.png");

    for (int i = 0; i < map_image.cv_image.rows; ++i) {
        for (int j = 0; j < map_image.cv_image.cols; ++j) {
            const int coords[2] = {i, j};
            process_pixel(map_image.cv_image.at<pixel_t>(i, j), coords);
        }
        progress = static_cast<double>(i) / map_image.cv_image.rows / 2;
        InvalidateRect(hwnd, nullptr, true);
    }

    for (int i = 0; i < map_image.cv_image.rows; ++i) {
        for (int j = 0; j < map_image.cv_image.cols; ++j) {
            const int coords[2] = {i, j};
            process_pixel_borders(map_image.cv_image.at<pixel_t>(i, j), coords);
        }
        progress = static_cast<double>(i) / map_image.cv_image.rows / 2 + 0.5;
        InvalidateRect(hwnd, nullptr, true);
    }

    auto province_values = provinces | std::views::values;

    std::for_each(std::execution::par, province_values.begin(), province_values.end(),
                  [](const std::unique_ptr<province> &province) { province->finalize(); });

    std::for_each(std::execution::par, province_values.begin(), province_values.end(),
                  [](const std::unique_ptr<province> &province) { province->process_distances(); });

    PostMessage(hwnd, WM_CLOSE, 0, 0);
    ExitThread(0);
}

inline void change_map_mode(const map_modes mode) {
    map_mode = mode;

    const auto province_values = provinces | std::views::values;

    std::for_each(std::execution::par, province_values.begin(), province_values.end(),
                  [](const std::unique_ptr<province> &province) { province->recolor(map_mode); });

    reload_bitmap();
}

inline void select_province(province *province) {
    const auto old_selected_province = selected_province;
    selected_province = province;
    if (old_selected_province != nullptr) {
        reload_bitmap_province(*old_selected_province);
    }
    if (selected_province != nullptr) {
        reload_bitmap_province(*selected_province);
    }
}

[[noreturn]] DWORD start_game_loop(LPVOID) {
    while (open) {
        for (const auto province_values = provinces | std::views::values; const auto &province: province_values) {
            province->tick();
        }

        for (const auto tag_values = tags | std::views::values; const auto &tag: tag_values) {
            tag->tick();
        }

        date = date + 1;

        if (date.year == -1) {
            open = false;
        }

        InvalidateRect(map_hwnd, nullptr, false);
        InvalidateRect(gui_hwnd, nullptr, true);

        // Sleep(10);
    }
    ExitThread(0);
}

LRESULT CALLBACK gui_window_proc(const HWND hwnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param) {
    switch (u_msg) {
        case WM_CREATE: {
            const auto map_mode_selector =
                    CreateWindowEx(0, WC_COMBOBOX, TEXT(""), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
                                   50, 100, 50, hwnd, reinterpret_cast<HMENU>(1), GetModuleHandle(nullptr), nullptr);
            SendMessage(map_mode_selector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("PROVINCES"));
            SendMessage(map_mode_selector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("OWNER"));
            SendMessage(map_mode_selector, CB_SETCURSEL, 0, 0);
        }
            return 0;
        case WM_COMMAND: {
            if (HIWORD(w_param) == CBN_SELCHANGE) {
                switch (const auto map_mode_selector = reinterpret_cast<HWND>(l_param);
                    SendMessage(map_mode_selector, CB_GETCURSEL, 0, 0)) {
                    case 0:
                        change_map_mode(map_modes::provinces);
                        break;
                    case 1:
                        change_map_mode(map_modes::owner);
                        break;
                    default: ;
                }
            }
        }
            return 0;
        case WM_SIZE: {
            InvalidateRect(hwnd, nullptr, false);
        }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            const auto hdc = BeginPaint(hwnd, &ps);

            const auto date_text = date.to_string();

            RECT text_rect = {0, 0, 100, 50};

            FillRect(hdc, &text_rect, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));

            DrawText(hdc, date_text.c_str(), static_cast<int>(date_text.size()), &text_rect,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            return DefWindowProc(hwnd, u_msg, w_param, l_param);
    }
}

LRESULT CALLBACK map_window_proc(const HWND hwnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param) {
    switch (u_msg) {
        case WM_CREATE:
            CreateThread(nullptr, 0, start_game_loop, nullptr, 0, nullptr);
            return 0;
        case WM_DESTROY:
            open = false;
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            InvalidateRect(hwnd, nullptr, false);
        }
            return 0;
        case WM_MOUSEWHEEL: {
            RECT client_rect;
            GetClientRect(hwnd, &client_rect);
            if (const auto delta = GET_WHEEL_DELTA_WPARAM(w_param); delta > 0) {
                if (zoom > 5) {
                    return 0;
                }
                zoom *= 1.1;
                offset[0] -= static_cast<int>((client_rect.right / 2.0 - offset[0]) * 0.1);
                offset[1] -= static_cast<int>((client_rect.bottom / 2.0 - offset[1]) * 0.1);
            } else {
                offset[0] += static_cast<int>((client_rect.right / 2.0 - offset[0]) / 11.0);
                offset[1] += static_cast<int>((client_rect.bottom / 2.0 - offset[1]) / 11.0);
                zoom /= 1.1;
            }
            if (offset[0] > 0) {
                offset[0] -= static_cast<int>(map_image.width * zoom);
            }
            if (offset[1] > 0) {
                offset[1] = 0;
            }
            if (offset[0] < -(map_image.width * zoom)) {
                offset[0] += static_cast<int>(map_image.width * zoom);
            }
            if (offset[1] < -(map_image.height * zoom - client_rect.bottom)) {
                offset[1] = static_cast<int>(-(map_image.height * zoom - client_rect.bottom));
            }
            InvalidateRect(hwnd, nullptr, false);
        }
            return 0;
        case WM_LBUTTONUP: {
            const auto x = static_cast<int>((LOWORD(l_param) - offset[0]) / zoom);
            const auto y = static_cast<int>((HIWORD(l_param) - offset[1]) / zoom);
            if (!mouse_moved) {
                const auto color = map_image.get_color(x % map_image.width, y);
                const auto province = provinces.at(color).get();
                select_province(province);
                InvalidateRect(hwnd, nullptr, false);
            }
            mouse_down = false;
            mouse_moved = false;
        }
            return 0;
        case WM_LBUTTONDOWN: {
            previous_mouse[0] = LOWORD(l_param);
            previous_mouse[1] = HIWORD(l_param);
            mouse_down = true;
            mouse_moved = false;
        }
            return 0;
        case WM_MOUSEMOVE: {
            const auto x = LOWORD(l_param);
            const auto y = HIWORD(l_param);
            if (mouse_down) {
                offset[0] += x - previous_mouse[0];
                offset[1] += y - previous_mouse[1];
                auto client_rect = RECT{};
                GetClientRect(hwnd, &client_rect);
                if (offset[0] > 0) {
                    offset[0] -= static_cast<int>(map_image.width * zoom);
                }
                if (offset[1] > 0) {
                    offset[1] = 0;
                }
                if (offset[0] < -(map_image.width * zoom)) {
                    offset[0] += static_cast<int>(map_image.width * zoom);
                }
                if (offset[1] < -(map_image.height * zoom - client_rect.bottom)) {
                    offset[1] = static_cast<int>(-(map_image.height * zoom - client_rect.bottom));
                }
                InvalidateRect(hwnd, nullptr, false);
                previous_mouse[0] = x;
                previous_mouse[1] = y;
            }
            mouse_moved = true;
        }
            return 0;
        case WM_KEYDOWN:
            if (w_param == VK_ESCAPE) {
                select_province(nullptr);
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            const auto hdc = BeginPaint(hwnd, &ps);

            wglMakeCurrent(hdc, opengl_hdc);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo_id);
            glBlitFramebuffer(0, 0, map_image.width, map_image.height, offset[0], ps.rcPaint.bottom - offset[1],
                              static_cast<int>(map_image.width * zoom) + offset[0],
                              ps.rcPaint.bottom - static_cast<int>(map_image.height * zoom) - offset[1],
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBlitFramebuffer(0, 0, map_image.width, map_image.height,
                              static_cast<int>(map_image.width * zoom) + offset[0],
                              ps.rcPaint.bottom - offset[1],
                              static_cast<int>(map_image.width * zoom) * 2 + offset[0],
                              ps.rcPaint.bottom - static_cast<int>(map_image.height * zoom) - offset[1],
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

            SwapBuffers(hdc);

            wglMakeCurrent(nullptr, nullptr);

            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            return DefWindowProc(hwnd, u_msg, w_param, l_param);
    }
}

LRESULT CALLBACK load_window_proc(const HWND hwnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param) {
    switch (u_msg) {
        case WM_CREATE:
            CreateThread(nullptr, 0, load_image, hwnd, 0, nullptr);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            const auto hdc = BeginPaint(hwnd, &ps);

            constexpr RECT progress_bar = {10, 60, 210, 80}; // Bar dimensions
            FillRect(hdc, &progress_bar, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1)); // Background
            const RECT progress_fill = {10, 60, 10 + static_cast<int>(200 * progress), 80}; // Filled portion
            FillRect(hdc, &progress_fill, reinterpret_cast<HBRUSH>(COLOR_HIGHLIGHT));

            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            return DefWindowProc(hwnd, u_msg, w_param, l_param);
    }
}

inline HWND create_window(const char *title, const WNDPROC window_proc, const DWORD style) {
    const auto wc = WNDCLASS{
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = window_proc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)),
        .lpszClassName = title
    };

    if (RegisterClass(&wc) == 0) {
        MessageBox(nullptr, "Failed to register window class", "Error", MB_ICONERROR);
        return nullptr;
    }

    const auto hwnd = CreateWindowEx(0, wc.lpszClassName, title, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    ShowWindow(hwnd, SW_SHOW);

    return hwnd;
}

inline void initialize_open_gl() {
    const HDC hdc = GetDC(map_hwnd);
    constexpr DWORD pixel_format_flags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_GENERIC_ACCELERATED |
                                         PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    constexpr PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        pixel_format_flags,
        PFD_TYPE_RGBA,
        24,
        8,
        0,
        8,
        0,
        8,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,
        0,
        0
    };
    const int pixel_format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixel_format, &pfd);
    opengl_hdc = wglCreateContext(hdc);

    wglMakeCurrent(hdc, opengl_hdc);

    const auto get_any_gl_func_address = [](const char *name) {
        auto p = reinterpret_cast<void *>(wglGetProcAddress(name));
        if (p == nullptr || (p == reinterpret_cast<void *>(1)) || (p == reinterpret_cast<void *>(2)) ||
            (p == reinterpret_cast<void *>(3)) || (p == reinterpret_cast<void *>(-1))) {
            const HMODULE module = LoadLibraryA("opengl32.dll");
            p = reinterpret_cast<void *>(GetProcAddress(module, name));
        }
        return p;
    };

    if (!gladLoadGLLoader(get_any_gl_func_address)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, map_image.width, map_image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 bytes.data());

    glGenFramebuffers(1, &read_fbo_id);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo_id);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    ReleaseDC(map_hwnd, hdc);
}

inline void create_display() {
    map_hwnd = create_window("Map", map_window_proc, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    gui_hwnd = create_window("GUI", gui_window_proc, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN);

    initialize_open_gl();
}

inline void start_message_loop() {
    auto msg = MSG{.hwnd = nullptr, .message = WM_NULL, .wParam = 0, .lParam = 0, .time = 0, .pt = POINT{0, 0}};

    while (GetMessage(&msg, nullptr, 0, 0) == 1) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    if (const auto loading_window = create_window("Loading", load_window_proc, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        loading_window == nullptr) {
        return -1;
    }

    start_message_loop();

    bytes = std::vector<BYTE>(map_image.width * map_image.height * 4);

    reload_bitmap();

    if (create_display(); map_hwnd != nullptr) {
        start_message_loop();

        DestroyWindow(gui_hwnd);

        DestroyWindow(map_hwnd);

        wglDeleteContext(opengl_hdc);
    }

    return 0;
}
