#include <algorithm>
#include <chrono>
#include <execution>
#include <glad/glad.h>
#include <opencv2/opencv.hpp>
#include <ranges>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <windows.h>
#include "Base/Image.hpp"
#include "Base/Province.hpp"
#include "Base/Utils.hpp"
#include "Tags/Country.hpp"
#include "Tags/Tag.hpp"

#pragma comment(                                                                                                       \
        linker,                                                                                                        \
        "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

Image image;
HWND hwnd;
HGLRC openglHdc = nullptr;
BYTE *bytes;
std::shared_mutex provinceMutex;
std::unordered_map<unsigned int, Province *> provinces;
std::unordered_map<unsigned int, Tag *> tags;
Province *selectedProvince = nullptr;
int offset[2] = {0, 0};
double zoom = 1.0;
int previousMouse[2] = {0, 0};
bool mouseDown = false;
bool mouseMoved = false;
auto mapMode = MapMode::OWNER;
auto date = Date();
GLuint texture;
GLuint readFboId = 0;

void changeMapMode(MapMode mode);

void selectProvince(Province *province);

void reloadBitmapProvince(const Province &province);

void reloadBitmap();

void loadImage();

LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND createDisplay();

void startMessageLoop(HWND hwnd);

void changeMapMode(const MapMode mode) {
	mapMode = mode;

	const auto provinceValues = provinces | std::views::values;

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(),
	              [](Province *province) { province->recolor(mapMode); });

	reloadBitmap();
}

void selectProvince(Province *province) {
	const auto oldSelectedProvince = selectedProvince;
	selectedProvince = province;
	if (oldSelectedProvince != nullptr) {
		reloadBitmapProvince(*oldSelectedProvince);
	}
	if (selectedProvince != nullptr) {
		reloadBitmapProvince(*selectedProvince);
	}
}

void reloadBitmapProvince(const Province &province) {
	const auto color = province.color;
	const auto pixels = province.getPixels();

	std::vector<int> indices(province.numPixels);
	std::iota(indices.begin(), indices.end(), 0);

	std::for_each(std::execution::par, indices.begin(), indices.end(), [pixels, color](const int i) {
		const auto pixel = pixels[i];
		const auto index = (pixel[0] + pixel[1] * image.width) * 4;
		bytes[index] = static_cast<BYTE>(color);
		bytes[index + 1] = static_cast<BYTE>(color >> 8);
		bytes[index + 2] = static_cast<BYTE>(color >> 16);
		bytes[index + 3] = 255;
	});

	indices = std::vector<int>(province.numOutline);
	std::iota(indices.begin(), indices.end(), 0);

	const auto outline = province.getOutline();
	auto updatedProvinces = std::unordered_set<Province *>();
	std::for_each(std::execution::par, indices.begin(), indices.end(),
	              [outline, &updatedProvinces, &province, color](const int i) {
		              const auto pixel = outline[i].second;
		              const auto otherProvince = outline[i].first;
		              const auto index = (pixel[0] + pixel[1] * image.width) * 4;
		              if (selectedProvince != nullptr && province.baseColor == selectedProvince->baseColor) {
			              bytes[index] = 255;
			              bytes[index + 1] = 255;
			              bytes[index + 2] = 255;
			              bytes[index + 3] = 255;
		              } else {
			              if (otherProvince != nullptr && otherProvince->color != color) {
				              bytes[index] = 0;
				              bytes[index + 1] = 0;
				              bytes[index + 2] = 0;
				              bytes[index + 3] = 255;
			              } else {
				              bytes[index] = static_cast<BYTE>(color);
				              bytes[index + 1] = static_cast<BYTE>(color >> 8);
				              bytes[index + 2] = static_cast<BYTE>(color >> 16);
				              bytes[index + 3] = 255;
			              }
		              }
		              std::unique_lock lock(provinceMutex);
		              if (!updatedProvinces.contains(otherProvince)) {
			              updatedProvinces.insert(otherProvince);
			              if (otherProvince == nullptr) {
				              return;
			              }
			              const auto otherOutline = otherProvince->getOutline();
			              for (unsigned int j = 0; j < otherProvince->numOutline; ++j) {
				              const auto otherPixel = otherOutline[j].second;
				              const auto otherIndex = (otherPixel[0] + otherPixel[1] * image.width) * 4;
				              if (otherOutline[j].first == &const_cast<Province &>(province)) {
					              if (otherProvince->color == color) {
						              bytes[otherIndex] = static_cast<BYTE>(color);
						              bytes[otherIndex + 1] = static_cast<BYTE>(color >> 8);
						              bytes[otherIndex + 2] = static_cast<BYTE>(color >> 16);
						              bytes[otherIndex + 3] = 255;
					              } else {
						              bytes[otherIndex] = 0;
						              bytes[otherIndex + 1] = 0;
						              bytes[otherIndex + 2] = 0;
						              bytes[otherIndex + 3] = 255;
					              }
				              }
			              }
		              }
	              });

	const auto hdc = GetDC(hwnd);

	wglMakeCurrent(hdc, openglHdc);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, province.bounds[1], image.width, province.bounds[3] - province.bounds[1],
	                GL_BGRA, GL_UNSIGNED_BYTE, bytes + image.width * province.bounds[1] * 4);

	ReleaseDC(hwnd, hdc);
}

void reloadBitmap() {
	const auto provinceValues = provinces | std::views::values;

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(), [](const Province *province) {
		const auto pixels = province->getPixels();

		std::vector<int> indices(province->numPixels);
		std::iota(indices.begin(), indices.end(), 0);

		std::for_each(std::execution::par, indices.begin(), indices.end(), [pixels, province](const int i) {
			const auto pixel = pixels[i];
			const auto color = province->color;
			const auto index = (pixel[0] + pixel[1] * image.width) * 4;
			bytes[index] = static_cast<BYTE>(color);
			bytes[index + 1] = static_cast<BYTE>(color >> 8);
			bytes[index + 2] = static_cast<BYTE>(color >> 16);
			bytes[index + 3] = 255;
		});

		const auto outline = province->getOutline();

		indices = std::vector<int>(province->numOutline);
		std::iota(indices.begin(), indices.end(), 0);
		std::for_each(std::execution::par, indices.begin(), indices.end(), [outline, province](const int i) {
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
					bytes[index] = static_cast<BYTE>(color);
					bytes[index + 1] = static_cast<BYTE>(color >> 8);
					bytes[index + 2] = static_cast<BYTE>(color >> 16);
					bytes[index + 3] = 255;
				}
			}
		});
	});

	if (openglHdc != nullptr) {
		const auto hdc = GetDC(hwnd);

		wglMakeCurrent(hdc, openglHdc);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_BGRA, GL_UNSIGNED_BYTE, bytes);

		ReleaseDC(hwnd, hdc);
	}
}

void loadImage() {
	constexpr auto processPixel = [](const Pixel &pixel, const int *position) {
		const auto i = position[1];
		const auto j = position[0];
		std::unique_lock lock(provinceMutex);
		if (const auto color = pixel.x + (pixel.y << 8) + (pixel.z << 16); provinces.contains(color)) {
			const auto province = provinces.at(color);
			province->expandBounds(i, j);
		} else {
			const auto province =
			        new Province(std::string("Province ") + std::to_string(provinces.size()), color, i, j);
			const auto country = new Country("", color);
			tags[color] = country;
			province->setOwner(*country);
			provinces[color] = province;
		}
	};

	constexpr auto isBorder = [&](const int x, const int y, const unsigned int color, const Province *&otherProvince) {
		if (x >= 0 && x < image.width && y >= 0 && y < image.height && image.getColor(x, y) != color) {
			otherProvince = provinces.at(image.getColor(x, y));
			return true;
		}
		return false;
	};

	constexpr auto processPixelBorders = [isBorder](const Pixel &pixel, const int *position) {
		const auto i = position[1];
		const auto j = position[0];
		const auto color = pixel.x + (pixel.y << 8) + (pixel.z << 16);
		Province *province = provinces.at(color);

		std::unique_lock lock(provinceMutex);
		if (const Province *otherProvince = nullptr;
		    isBorder(i - 1, j, color, otherProvince) || isBorder(i + 1, j, color, otherProvince) ||
		    isBorder(i, j - 1, color, otherProvince) || isBorder(i, j + 1, color, otherProvince)) {
			province->addOutline(i, j, *otherProvince);
		} else {
			province->addPixel(i, j);
		}
	};

	image = Image("assets/provinces1.png");

	image.cvImage.forEach<Pixel>(processPixel);

	image.cvImage.forEach<Pixel>(processPixelBorders);

	auto provinceValues = provinces | std::views::values;

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(),
	              [](Province *province) { province->lock(); });

	std::for_each(std::execution::par, provinceValues.begin(), provinceValues.end(),
	              [](Province *province) { province->processDistances(); });

	// Test code for pathfinding optimization
	// const auto army = tags.at(0x6B643E)->newArmy();
	// const auto unit = army->newUnit(*provinces.at(0x6B643E));
	// testUnit = unit;
	// const auto start = std::chrono::high_resolution_clock::now();
	// unit->setDestination(*provinces.at(0xDF2050));
	// const auto end = std::chrono::high_resolution_clock::now();
	// std::chrono::duration<double> elapsed = end - start;
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

			wglMakeCurrent(hdc, openglHdc);

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
			glBlitFramebuffer(0, 0, image.width, image.height, offset[0], ps.rcPaint.bottom - offset[1],
			                  static_cast<int>(image.width * zoom) + offset[0],
			                  ps.rcPaint.bottom - static_cast<int>(image.height * zoom) - offset[1],
			                  GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			SwapBuffers(hdc);

			wglMakeCurrent(nullptr, nullptr);

			// Draw the test unit path
			// if (testUnit != nullptr) {
			// 	const auto path = testUnit->path;
			// 	const Province *previous = nullptr;
			// 	SelectObject(buffer, GetStockObject(DC_PEN));
			// 	for (const auto & i : *path) {
			// 		const auto pixel = i->center;
			// 		if (previous != nullptr) {
			// 			MoveToEx(buffer, previous->center[0] + offset[0], previous->center[1] + offset[1], nullptr);
			// 			LineTo(buffer, pixel[0] + offset[0], pixel[1] + offset[1]);
			// 		}
			// 		previous = i;
			// 	}
			// }

			EndPaint(hwnd, &ps);
		}
			return 0;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

HWND createDisplay() {
	const auto wc = WNDCLASS{.style = CS_HREDRAW | CS_VREDRAW,
	                         .lpfnWndProc = windowProc,
	                         .hInstance = GetModuleHandle(nullptr),
	                         .hCursor = LoadCursor(nullptr, IDC_ARROW),
	                         .hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)),
	                         .lpszClassName = "Display"};

	if (RegisterClass(&wc) == 0) {
		MessageBox(nullptr, "Failed to register window class", "Error", MB_ICONERROR);
		return nullptr;
	}

	hwnd = CreateWindowEx(0, wc.lpszClassName, "Display", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
	                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(nullptr),
	                      nullptr);

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	const HDC hdc = GetDC(hwnd);
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

	constexpr auto getAnyGLFuncAddress = [](const char *name) {
		auto p = reinterpret_cast<void *>(wglGetProcAddress(name));
		if (p == nullptr || (p == reinterpret_cast<void *>(0x1)) || (p == reinterpret_cast<void *>(0x2)) ||
		    (p == reinterpret_cast<void *>(0x3)) || (p == reinterpret_cast<void *>(-1))) {
			const HMODULE module = LoadLibraryA("opengl32.dll");
			p = reinterpret_cast<void *>(GetProcAddress(module, name));
		}
		return p;
	};

	if (!gladLoadGLLoader(getAnyGLFuncAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bytes);

	glGenFramebuffers(1, &readFboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	ReleaseDC(hwnd, hdc);

	return hwnd;
}

void startMessageLoop() {
	if (hwnd == nullptr) {
		return;
	}

	auto msg = MSG{.hwnd = hwnd, .message = WM_NULL, .wParam = 0, .lParam = 0, .time = 0, .pt = POINT{0, 0}};

	while (GetMessage(&msg, hwnd, 0, 0) == 1) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	loadImage();

	bytes = new BYTE[image.width * image.height * 4];

	reloadBitmap();

	if (createDisplay(); hwnd != nullptr) {
		startMessageLoop();

		DestroyWindow(hwnd);

		wglDeleteContext(openglHdc);
	}

	delete[] bytes;

	for (const auto &province: provinces | std::views::values) {
		delete province;
	}

	for (const auto &tag: tags | std::views::values) {
		delete tag;
	}

	return 0;
}
