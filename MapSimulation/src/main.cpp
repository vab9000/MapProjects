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
int previousMouse[2] = {0, 0};
bool mouseDown = false;
bool mouseMoved = false;
bool open = true;

HWND mapHwnd = nullptr;
HWND guiHwnd = nullptr;
HGLRC openglHdc = nullptr;
GLuint texture = 0;
GLuint readFboId = 0;
std::vector<BYTE> bytes;
auto image = Image();

auto provinces = std::unordered_map<unsigned int, std::unique_ptr<Province>>();
auto tags = std::unordered_map<unsigned int, std::unique_ptr<Tag>>();

Province *selectedProvince = nullptr;
auto mapMode = MapMode::OWNER;
auto date = Date();

inline void setPixel(std::vector<BYTE> &bytes, const int index, const BYTE r, const BYTE g, const BYTE b) {
	bytes[index] = r;
	bytes[index + 1] = g;
	bytes[index + 2] = b;
	bytes[index + 3] = 255;
}

void reloadBitmapProvince(const Province &province) {
	const auto color = province.color;
	const auto pixels = province.getPixels();

	std::vector<int> indices(province.getNumPixels());
	std::iota(indices.begin(), indices.end(), 0);

	std::for_each(std::execution::par, indices.begin(), indices.end(), [pixels, color](const int i) {
		const auto pixel = pixels[i];
		const auto index = (pixel[0] + pixel[1] * image.width) * 4;
		setPixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8), static_cast<BYTE>(color >> 16));
	});

	const auto outline = province.getOutline();
	auto updatedProvinces = std::unordered_set<Province *>();
	for (auto i = 0; i < province.getNumOutline(); ++i) {
		const auto pixel = outline[i].second;
		const auto otherProvince = outline[i].first;
		const auto index = (pixel[0] + pixel[1] * image.width) * 4;
		if (selectedProvince != nullptr && province.baseColor == selectedProvince->baseColor) {
			setPixel(bytes, index, 255, 255, 255);
		} else {
			if (otherProvince != nullptr && otherProvince->color != color) {
				setPixel(bytes, index, 0, 0, 0);
			} else {
				setPixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
				         static_cast<BYTE>(color >> 16));
			}
		}
		if (!updatedProvinces.contains(otherProvince)) {
			updatedProvinces.insert(otherProvince);
			if (otherProvince == nullptr) {
				return;
			}
			const auto otherOutline = otherProvince->getOutline();
			for (unsigned int j = 0; j < otherProvince->getNumOutline(); ++j) {
				const auto otherPixel = otherOutline[j].second;
				const auto otherIndex = (otherPixel[0] + otherPixel[1] * image.width) * 4;
				if (otherOutline[j].first == &const_cast<Province &>(province)) {
					if (otherProvince->color == color) {
						setPixel(bytes, otherIndex, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
						         static_cast<BYTE>(color >> 16));
					} else {
						setPixel(bytes, otherIndex, 0, 0, 0);
					}
				}
			}
		}
	}

	const auto hdc = GetDC(mapHwnd);

	wglMakeCurrent(hdc, openglHdc);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, province.getBounds()[1], image.width,
	                province.getBounds()[3] - province.getBounds()[1], GL_BGRA, GL_UNSIGNED_BYTE,
	                bytes.data() + image.width * province.getBounds()[1] * 4);

	ReleaseDC(mapHwnd, hdc);
}

void reloadBitmap() {
	const auto provinceValues = provinces | std::views::values;

	std::for_each(
	        std::execution::par, provinceValues.begin(), provinceValues.end(),
	        [](const std::unique_ptr<Province> &province) {
		        const auto pixels = province->getPixels();

		        std::vector<int> indices(province->getNumPixels());
		        std::iota(indices.begin(), indices.end(), 0);

		        std::for_each(std::execution::par, indices.begin(), indices.end(), [pixels, &province](const int i) {
			        const auto pixel = pixels[i];
			        const auto color = province->color;
			        const auto index = (pixel[0] + pixel[1] * image.width) * 4;
			        setPixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
			                 static_cast<BYTE>(color >> 16));
		        });

		        const auto outline = province->getOutline();

		        indices = std::vector<int>(province->getNumOutline());
		        std::iota(indices.begin(), indices.end(), 0);
		        std::for_each(std::execution::par, indices.begin(), indices.end(), [outline, &province](const int i) {
			        const auto otherProvince = outline[i].first;
			        const auto pixel = outline[i].second;
			        const auto color = province->color;
			        const auto index = (pixel[0] + pixel[1] * image.width) * 4;
			        if (selectedProvince != nullptr && color == selectedProvince->color) {
				        setPixel(bytes, index, 255, 255, 255);
			        } else {
				        if (otherProvince != nullptr && otherProvince->color != color) {
					        setPixel(bytes, index, 0, 0, 0);
				        } else {
					        setPixel(bytes, index, static_cast<BYTE>(color), static_cast<BYTE>(color >> 8),
					                 static_cast<BYTE>(color >> 16));
				        }
			        }
		        });
	        });

	if (openglHdc != nullptr) {
		const auto hdc = GetDC(mapHwnd);

		wglMakeCurrent(hdc, openglHdc);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_BGRA, GL_UNSIGNED_BYTE, bytes.data());

		ReleaseDC(mapHwnd, hdc);
	}
}

inline void loadImage() {
	auto processPixel = [](const Pixel &pixel, const int *position) {
		const auto i = position[1];
		const auto j = position[0];
		if (const auto color = pixel.x + (pixel.y << 8) + (pixel.z << 16); provinces.contains(color)) {
			const auto province = provinces.at(color).get();
			province->expandBounds(i, j);
		} else {
			auto invertColor = [](const unsigned int colorToChange) {
				return (colorToChange & 0xFF) << 16 | (colorToChange & 0xFF00) | (colorToChange & 0xFF0000) >> 16;
			};
			auto country = std::make_unique<Country>("", invertColor(color));
			auto province = std::make_unique<Province>(std::string("Province ") + std::to_string(provinces.size()),
			                                           color, i, j);
			province->setOwner(*country);
			tags.insert(std::make_pair(color, std::move(country)));
			provinces.insert(std::make_pair(color, std::move(province)));
		}
	};

	auto isBorder = [&](const int x, const int y, const unsigned int color, const Province *&otherProvince) {
		if (x >= 0 && x < image.width && y >= 0 && y < image.height && image.getColor(x, y) != color) {
			otherProvince = provinces.at(image.getColor(x, y)).get();
			return true;
		}
		return false;
	};

	auto processPixelBorders = [isBorder](const Pixel &pixel, const int *position) {
		const auto i = position[1];
		const auto j = position[0];
		const auto color = pixel.x + (pixel.y << 8) + (pixel.z << 16);
		Province *province = provinces.at(color).get();

		if (const Province *otherProvince = nullptr;
		    isBorder(i - 1, j, color, otherProvince) || isBorder(i + 1, j, color, otherProvince) ||
		    isBorder(i, j - 1, color, otherProvince) || isBorder(i, j + 1, color, otherProvince)) {
			province->addOutline(i, j, *otherProvince);
		} else {
			province->addPixel(i, j);
		}
	};

	image = Image("assets/small.png");

	for (int i = 0; i < image.cvImage.rows; ++i) {
		for (int j = 0; j < image.cvImage.cols; ++j) {
			const int coords[2] = {i, j};
			processPixel(image.cvImage.at<Pixel>(i, j), coords);
		}
	}

	for (int i = 0; i < image.cvImage.rows; ++i) {
		for (int j = 0; j < image.cvImage.cols; ++j) {
			const int coords[2] = {i, j};
			processPixelBorders(image.cvImage.at<Pixel>(i, j), coords);
		}
	}

	auto provinceValues = provinces | std::views::values;

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(),
	              [](const std::unique_ptr<Province> &province) { province->finalize(); });

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(),
	              [](const std::unique_ptr<Province> &province) { province->processDistances(); });
}

inline void changeMapMode(const MapMode mode) {
	mapMode = mode;

	const auto provinceValues = provinces | std::views::values;

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(),
	              [](const std::unique_ptr<Province> &province) { province->recolor(mapMode); });

	reloadBitmap();
}

inline void selectProvince(Province *province) {
	const auto oldSelectedProvince = selectedProvince;
	selectedProvince = province;
	if (oldSelectedProvince != nullptr) {
		reloadBitmapProvince(*oldSelectedProvince);
	}
	if (selectedProvince != nullptr) {
		reloadBitmapProvince(*selectedProvince);
	}
}

[[noreturn]] inline DWORD startGameLoop(LPVOID) {
	while (open) {
		for (const auto provinceValues = provinces | std::views::values; const auto &province: provinceValues) {
			province->tick();
		}

		for (const auto tagValues = tags | std::views::values; const auto &tag: tagValues) {
			tag->updateAI();
		}

		date = date + 1;

		if (date.year == -1) {
			open = false;
		}

		InvalidateRect(mapHwnd, nullptr, false);
		InvalidateRect(guiHwnd, nullptr, true);

		// Sleep(10);
	}
	ExitThread(0);
}

LRESULT CALLBACK guiWindowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (uMsg) {
		case WM_CREATE: {
			const auto mapModeSelector =
			        CreateWindowEx(0, WC_COMBOBOX, TEXT(""), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
			                       50, 100, 50, hwnd, reinterpret_cast<HMENU>(1), GetModuleHandle(nullptr), nullptr);
			SendMessage(mapModeSelector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("PROVINCES"));
			SendMessage(mapModeSelector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("OWNER"));
			SendMessage(mapModeSelector, CB_SETCURSEL, 0, 0);
		}
			return 0;
		case WM_COMMAND: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				switch (const auto mapModeSelector = reinterpret_cast<HWND>(lParam);
				        SendMessage(mapModeSelector, CB_GETCURSEL, 0, 0)) {
					case 0:
						changeMapMode(MapMode::PROVINCES);
						break;
					case 1:
						changeMapMode(MapMode::OWNER);
						break;
					default:;
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

			const auto dateText = date.toString();

			RECT textRect = {0, 0, 100, 50};

			FillRect(hdc, &textRect, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));

			DrawText(hdc, dateText.c_str(), static_cast<int>(dateText.size()), &textRect,
			         DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			EndPaint(hwnd, &ps);
		}
			return 0;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK mapWindowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (uMsg) {
		case WM_CREATE:
			CreateThread(nullptr, 0, startGameLoop, nullptr, 0, nullptr);
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
				const auto province = provinces.at(color).get();
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

			wglMakeCurrent(hdc, openglHdc);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
			glBlitFramebuffer(0, 0, image.width, image.height, offset[0], ps.rcPaint.bottom - offset[1],
			                  static_cast<int>(image.width * zoom) + offset[0],
			                  ps.rcPaint.bottom - static_cast<int>(image.height * zoom) - offset[1],
			                  GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			SwapBuffers(hdc);

			wglMakeCurrent(nullptr, nullptr);

			EndPaint(hwnd, &ps);
		}
			return 0;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

inline HWND createWindow(const char *title, const WNDPROC windowProc, const DWORD style) {
	const auto wc = WNDCLASS{.style = CS_HREDRAW | CS_VREDRAW,
	                         .lpfnWndProc = windowProc,
	                         .hInstance = GetModuleHandle(nullptr),
	                         .hCursor = LoadCursor(nullptr, IDC_ARROW),
	                         .hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)),
	                         .lpszClassName = title};

	if (RegisterClass(&wc) == 0) {
		MessageBox(nullptr, "Failed to register window class", "Error", MB_ICONERROR);
		return nullptr;
	}

	const auto hwnd = CreateWindowEx(0, wc.lpszClassName, title, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	                                 CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}

inline void initializeOpenGL() {
	const HDC hdc = GetDC(mapHwnd);
	constexpr DWORD pixelFormatFlags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_GENERIC_ACCELERATED |
	                                   PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	constexpr PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
	                                       1,
	                                       pixelFormatFlags,
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
	                                       0};
	const int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
	openglHdc = wglCreateContext(hdc);

	wglMakeCurrent(hdc, openglHdc);

	const auto getAnyGLFuncAddress = [](const char *name) {
		auto p = reinterpret_cast<void *>(wglGetProcAddress(name));
		if (p == nullptr || (p == reinterpret_cast<void *>(1)) || (p == reinterpret_cast<void *>(2)) ||
		    (p == reinterpret_cast<void *>(3)) || (p == reinterpret_cast<void *>(-1))) {
			const HMODULE module = LoadLibraryA("opengl32.dll");
			p = reinterpret_cast<void *>(GetProcAddress(module, name));
		}
		return p;
	};

	if (!gladLoadGLLoader(getAnyGLFuncAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bytes.data());

	glGenFramebuffers(1, &readFboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	ReleaseDC(mapHwnd, hdc);
}

inline void createDisplay() {
	mapHwnd = createWindow("Map", mapWindowProc, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	guiHwnd = createWindow("GUI", guiWindowProc, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN);

	initializeOpenGL();
}

inline void startMessageLoop() {
	auto msg = MSG{.hwnd = nullptr, .message = WM_NULL, .wParam = 0, .lParam = 0, .time = 0, .pt = POINT{0, 0}};

	while (GetMessage(&msg, nullptr, 0, 0) == 1) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	loadImage();

	bytes = std::vector<BYTE>(image.width * image.height * 4);

	reloadBitmap();

	if (createDisplay(); mapHwnd != nullptr) {
		startMessageLoop();

		DestroyWindow(guiHwnd);

		DestroyWindow(mapHwnd);

		wglDeleteContext(openglHdc);
	}

	return 0;
}
