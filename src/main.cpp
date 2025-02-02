#include <unordered_map>
#include <shared_mutex>
#include <ranges>
#include <algorithm>
#include <execution>
#include <windows.h>
#include <CommCtrl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Base/Province.hpp"
#include "Base/Image.hpp"
#include "Base/AI.hpp"
#include "Tags/Country.hpp"

Image image;
HBITMAP bmp;
std::shared_mutex provinceMutex;
std::unordered_map<unsigned int, Province *> provinces;
std::unordered_map<unsigned int, Country *> countries;
Province *selectedProvince = nullptr;
int offset[2] = {0, 0};
double zoom = 1.0;
int previousMouse[2] = {0, 0};
bool mouseDown = false;
bool mouseMoved = false;

void selectProvince(Province *province);

void reloadBitmapProvince(const Province *province);

void reloadBitmap();

void loadImage();

LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND createDisplay();

void startMessageLoop(HWND hwnd);

void selectProvince(Province *province) {
    const auto oldSelectedProvince = selectedProvince;
    selectedProvince = province;
    if (oldSelectedProvince != nullptr) {
        reloadBitmapProvince(oldSelectedProvince);
    }
    if (selectedProvince != nullptr) {
        reloadBitmapProvince(selectedProvince);
    }
}

void reloadBitmapProvince(const Province *province) {
    if (bmp == nullptr || province == nullptr) {
        return;
    }

    BITMAP bmpInfo;
    GetObject(bmp, sizeof(BITMAP), &bmpInfo);

    const auto hdc = CreateCompatibleDC(nullptr);

    const auto hbmOld = SelectObject(hdc, bmp);

    const auto bmpSize = bmpInfo.bmWidth * bmpInfo.bmHeight * 4;
    auto *bytes = new BYTE[bmpSize];

    GetBitmapBits(bmp, bmpSize, bytes);

    const auto color = province->color;
    const auto pixels = province->getPixels();
    for (int i = 0; i < province->numPixels; i++) {
        const auto pixel = pixels[i];
        const auto index = (pixel[0] + pixel[1] * image.width) * 4;
        bytes[index] = static_cast<BYTE>(color);
        bytes[index + 1] = static_cast<BYTE>(color >> 8);
        bytes[index + 2] = static_cast<BYTE>(color >> 16);
        bytes[index + 3] = 255;
    }
    const auto outline = province->getOutline();
    for (int i = 0; i < province->numOutline; i++) {
        const auto pixel = outline[i].second;
        const auto otherProvince = outline[i].first;
        const auto index = (pixel[0] + pixel[1] * image.width) * 4;
        if (selectedProvince != nullptr && province->color == selectedProvince->color) {
            bytes[index] = 255;
            bytes[index + 1] = 255;
            bytes[index + 2] = 255;
            bytes[index + 3] = 255;
        } else {
            if (otherProvince != nullptr && otherProvince->color != province->color) {
                bytes[index] = 0;
                bytes[index + 1] = 0;
                bytes[index + 2] = 0;
                bytes[index + 3] = 255;
            } else {
                bytes[index] = static_cast<BYTE>(color % 256);
                bytes[index + 1] = static_cast<BYTE>((color >> 8) % 256);
                bytes[index + 2] = static_cast<BYTE>((color >> 16) % 256);
                bytes[index + 3] = 255;
            }
        }
    }

	const auto centerIndex = (province->center[0] + province->center[1] * image.width) * 4;
	bytes[centerIndex] = 125;
	bytes[centerIndex + 1] = 125;
	bytes[centerIndex + 2] = 125;
	bytes[centerIndex + 3] = 255;

    bmp = CreateBitmap(image.width, image.height, 1, 32, bytes);

    delete[] bytes;

    SelectObject(hdc, hbmOld);

    DeleteDC(hdc);
}

void reloadBitmap() {
    DeleteObject(bmp);

    const auto bytes = new BYTE[image.width * image.height * 4];

    const auto provinceValues = provinces | std::views::values;
    std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(), [bytes](const Province *province) {
        const auto pixels = province->getPixels();
        for (int i = 0; i < province->numPixels; i++) {
            const auto pixel = pixels[i];
            const auto color = province->color;
            const auto index = (pixel[0] + pixel[1] * image.width) * 4;
            bytes[index] = static_cast<BYTE>(color % 256);
            bytes[index + 1] = static_cast<BYTE>((color >> 8) % 256);
            bytes[index + 2] = static_cast<BYTE>((color >> 16) % 256);
            bytes[index + 3] = 255;
        }
        const auto outline = province->getOutline();
        for (int i = 0; i < province->numOutline; i++) {
            const auto otherProvince = outline[i].first;
            const auto pixel = outline[i].second;
            const auto color = province->color;
            const auto index = (pixel[0] + pixel[1] * image.width) * 4;
            if (selectedProvince != nullptr && province->color == selectedProvince->color) {
                bytes[index] = 255;
                bytes[index + 1] = 255;
                bytes[index + 2] = 255;
                bytes[index + 3] = 255;
            } else {
                if (otherProvince != nullptr && otherProvince->color != province->color) {
                    bytes[index] = 0;
                    bytes[index + 1] = 0;
                    bytes[index + 2] = 0;
                    bytes[index + 3] = 255;
                } else {
                    bytes[index] = static_cast<BYTE>(color % 256);
                    bytes[index + 1] = static_cast<BYTE>((color >> 8) % 256);
                    bytes[index + 2] = static_cast<BYTE>((color >> 16) % 256);
                    bytes[index + 3] = 255;
                }
            }
        }
    	
    	const auto centerIndex = (province->center[0] + province->center[1] * image.width) * 4;
		bytes[centerIndex] = 125;
		bytes[centerIndex + 1] = 125;
		bytes[centerIndex + 2] = 125;
		bytes[centerIndex + 3] = 255;
    });

    bmp = CreateBitmap(image.width, image.height, 1, 32, bytes);

    delete[] bytes;
}

void loadImage() {
    auto checkBorder = [](const unsigned int color, Province** ptr, const int i, const int j, const Image &image, const std::unordered_map<unsigned int, Province*>& provinces) {
        if (image.getColor(i, j) != color) {
            *ptr = provinces.at(image.getColor(i, j));
            return true;
        }

        return false;
    };

    image = Image{};

    image.data = stbi_load("assets/provinces.png", &image.width, &image.height, &image.channels, 3);

    std::vector<int> iRange(image.width);
    std::iota(iRange.begin(), iRange.end(), 0);

    std::for_each(std::execution::par, iRange.begin(), iRange.end(), [](auto &&i) {
        for (auto j = 0; j < image.height; ++j) {
            std::unique_lock lock(provinceMutex);
            if (const auto color = image.getColor(i, j); provinces.contains(color)) {
                Province *province = provinces.at(color);
                province->expandBounds(i, j);
            } else {
                auto *province = new Province(
                    std::string("Province ") + std::to_string(provinces.size()),
                    color,
                    i,
                    j
                );
                provinces[color] = province;
            }
        }
    });

    std::for_each(std::execution::par, iRange.begin(), iRange.end(), [checkBorder](auto &&i) {
        for (auto j = 0; j < image.height; ++j) {
            const auto color = image.getColor(i, j);
            Province *province = provinces.at(color);
            std::unique_lock lock(provinceMutex);
            if (Province* otherProvince = nullptr;
                (i > 0 && checkBorder(color, &otherProvince, i - 1, j, image, provinces))
                || (i < image.width - 1 && checkBorder(color, &otherProvince, i + 1, j, image, provinces))
                || (j > 0 && checkBorder(color, &otherProvince, i, j - 1, image, provinces))
                || (j < image.height - 1 && checkBorder(color, &otherProvince, i, j + 1, image, provinces))) {
                province->addOutline(i, j, otherProvince);
            } else {
                province->addPixel(i, j);
            }
        }
    });

    for (const auto &province: provinces | std::views::values) {
        province->lock();
    }
}

LRESULT CALLBACK windowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            InvalidateRect(hwnd, nullptr, false);
            return 0;
        case WM_MOUSEWHEEL: {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            if (const auto delta = GET_WHEEL_DELTA_WPARAM(wParam); delta > 0) {
                if (zoom > 5) {
                    return 0;
                }
                zoom *= 1.1;
                offset[0] -= static_cast<int>((clientRect.right / 2.0 - offset[0]) * 0.1);
                offset[1] -= static_cast<int>((clientRect.bottom / 2.0 - offset[1]) * 0.1);
            } else {
                offset[0] += static_cast<int>((clientRect.right / 2.0 - offset[0]) / 11.0);
                offset[1] += static_cast<int>((clientRect.bottom / 2.0 - offset[1]) / 11.0);
                zoom /= 1.1;
            }
            if (offset[0] > 0) {
                offset[0] = 0;
            }
            if (offset[1] > 0) {
                offset[1] = 0;
            }
            if (offset[0] < -(image.width * zoom - clientRect.right)) {
                offset[0] = static_cast<int>(-(image.width * zoom - clientRect.right));
            }
            if (offset[1] < -(image.height * zoom - clientRect.bottom)) {
                offset[1] = static_cast<int>(-(image.height * zoom - clientRect.bottom));
            }
            InvalidateRect(hwnd, nullptr, false);
        }
            return 0;
        case WM_LBUTTONUP: {
            const auto x = static_cast<int>((LOWORD(lParam) - offset[0]) / zoom);
            const auto y = static_cast<int>((HIWORD(lParam) - offset[1]) / zoom);
            if (!mouseMoved) {
                const auto color = image.getColor(x, y);
                const auto province = provinces.at(color);
                selectProvince(province);
                InvalidateRect(hwnd, nullptr, false);
            }
            mouseDown = false;
            mouseMoved = false;
        }
            return 0;
        case WM_LBUTTONDOWN: {
            previousMouse[0] = LOWORD(lParam);
            previousMouse[1] = HIWORD(lParam);
            mouseDown = true;
            mouseMoved = false;
        }
            return 0;
        case WM_MOUSEMOVE: {
            const auto x = LOWORD(lParam);
            const auto y = HIWORD(lParam);
            if (mouseDown) {
                offset[0] += x - previousMouse[0];
                offset[1] += y - previousMouse[1];
                auto clientRect = RECT{};
                GetClientRect(hwnd, &clientRect);
                if (offset[0] > 0) {
                    offset[0] = 0;
                }
                if (offset[1] > 0) {
                    offset[1] = 0;
                }
                if (offset[0] < -(image.width * zoom - clientRect.right)) {
                    offset[0] = static_cast<int>(-(image.width * zoom - clientRect.right));
                }
                if (offset[1] < -(image.height * zoom - clientRect.bottom)) {
                    offset[1] = static_cast<int>(-(image.height * zoom - clientRect.bottom));
                }
                InvalidateRect(hwnd, nullptr, false);
                previousMouse[0] = x;
                previousMouse[1] = y;
            }
            mouseMoved = true;
        }
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                selectProvince(nullptr);
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            const auto hdc = BeginPaint(hwnd, &ps);

            const auto buffer = CreateCompatibleDC(hdc);
            const auto bitmap = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left,
                                                       ps.rcPaint.bottom - ps.rcPaint.top);
            SelectObject(buffer, bitmap);

            FillRect(buffer, &ps.rcPaint, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

            if (bmp != nullptr) {
                const auto hdcMem = CreateCompatibleDC(buffer);
                SelectObject(hdcMem, bmp);
                StretchBlt(buffer, offset[0], offset[1], static_cast<int>(image.width * zoom),
                           static_cast<int>(image.height * zoom), hdcMem, 0, 0, image.width, image.height, SRCCOPY);
                DeleteDC(hdcMem);
            }

            BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, buffer, 0, 0,
                   SRCCOPY);

            DeleteObject(bitmap);
            DeleteDC(buffer);

            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND createDisplay() {
    const auto wc = WNDCLASS{
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = windowProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)),
        .lpszClassName = "Display"
    };

    if (RegisterClass(&wc) == 0) {
        MessageBox(nullptr, "Failed to register window class", "Error", MB_ICONERROR);
        return nullptr;
    }

    const auto hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Display",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    return hwnd;
}

void startMessageLoop(const HWND hwnd) {
    if (hwnd == nullptr) {
        return;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    auto msg = MSG{
        .hwnd = hwnd,
        .message = WM_NULL,
        .wParam = 0,
        .lParam = 0,
        .time = 0,
        .pt = POINT{0, 0}
    };

    while (GetMessage(&msg, hwnd, 0, 0) == 1) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    loadImage();

    const auto hwnd = createDisplay();

    reloadBitmap();

    startMessageLoop(hwnd);

    DestroyWindow(hwnd);

    for (const auto &province: provinces | std::views::values) {
        delete province;
    }

	for (const auto &country: countries | std::views::values) {
		delete country;
	}

    stbi_image_free(image.data);

    return 0;
}
