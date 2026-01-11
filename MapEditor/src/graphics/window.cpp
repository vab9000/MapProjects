#include "window.hpp"
#include <array>
#include <cstddef>
#include <imgui.h>
#include <limits>
#include <memory>
#include <print>
#include <ranges>
#include <span>
#include <utility>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "image.hpp"
#include "zstring_view.hpp"

constexpr std::array opengl_version = {4, 6};

namespace graphics {
    auto window::error_callback(int error, const char *description) -> void {
        std::println("GLFW error: ({}) {}\n", error, description);
    }

    auto window::key_callback(GLFWwindow *window, const int key, const int scancode, const int action,
        const int mods) -> void {
        if (window != window_) { return; }
        if (!callbacks_.key_callback) { return; }
        callbacks_.key_callback(key, action, scancode, mods);
    }

    auto window::mouse_callback(GLFWwindow *window, const int button, const int action, const int mods) -> void {
        if (window != window_) { return; }
        if (!callbacks_.mouse_callback) { return; }
        callbacks_.mouse_callback(button, action, mods);
    }

    auto window::cursor_callback(GLFWwindow *window, double x_pos, double y_pos) -> void {
        if (window != window_) { return; }
        if (!callbacks_.cursor_callback) { return; }
        callbacks_.cursor_callback(x_pos, y_pos);
    }

    auto window::scroll_callback(GLFWwindow *window, const double x_offset, const double y_offset) -> void {
        if (window != window_) { return; }
        if (!callbacks_.scroll_callback) { return; }
        callbacks_.scroll_callback(x_offset, y_offset);
    }

    auto window::drop_callback(GLFWwindow *window, const int count, const char **paths) -> void {
        if (window != window_) { return; }
        if (!callbacks_.drop_callback) { return; }
        callbacks_.drop_callback(count, paths);
    }

    auto window::resize_callback(GLFWwindow *window, const int width, const int height) -> void {
        if (window != window_) { return; }
        if (!callbacks_.resize_callback) { return; }
        callbacks_.resize_callback(width, height);
    }

    auto window::create_cursor(const image &img, int x_offset, int y_offset) -> size_t {
        cursors_.emplace_back();
        auto &pixels = cursors_.back().second;
        for (size_t idx = 0; idx < cursor_dims * cursor_dims; ++idx) {
            const auto pixel = img.at(image::dimensions{
                static_cast<int>(idx % cursor_dims), static_cast<int>(idx / cursor_dims)
            });
            constexpr unsigned char max = std::numeric_limits<unsigned char>::max();
            if (pixel[0] == max && pixel[1] == max && pixel[2] == max) {
                pixels.at(idx * 4 + 0) = 0;
                pixels.at(idx * 4 + 1) = 0;
                pixels.at(idx * 4 + 2) = 0;
                pixels.at(idx * 4 + 3) = 0;
                continue;
            }
            pixels.at(idx * 4 + 0) = pixel[0];
            pixels.at(idx * 4 + 1) = pixel[1];
            pixels.at(idx * 4 + 2) = pixel[2];
            pixels.at(idx * 4 + 3) = pixel[3];
        }
        GLFWimage glfw_img = {};
        glfw_img.width = cursor_dims;
        glfw_img.height = cursor_dims;
        glfw_img.pixels = pixels.data();

        GLFWcursor *cursor = glfwCreateCursor(&glfw_img, x_offset, y_offset);
        cursors_.back().first = cursor;
        return cursors_.size() - 1;
    }

    auto window::set_cursor(const size_t idx) -> void {
        if (idx >= cursors_.size()) { return; }
        glfwSetCursor(window_, cursors_.at(idx).first);
    }

    auto window::init(const zstring_view name, const dimensions dims, callbacks &&calls) -> bool {
        size_ = dims;
        callbacks_ = std::move(calls);

        if (glfwInit() == GLFW_FALSE) { return false; }

        glfwSetErrorCallback(error_callback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version.at(0));
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version.at(1));
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

        window_ = glfwCreateWindow(dims.at(0), dims.at(1), name.str(), nullptr, nullptr);

        if (window_ == nullptr) {
            glfwTerminate();
            return false;
        }

        glfwSetKeyCallback(window_, key_callback);
        glfwSetMouseButtonCallback(window_, mouse_callback);
        glfwSetCursorPosCallback(window_, cursor_callback);
        glfwSetScrollCallback(window_, scroll_callback);
        glfwSetDropCallback(window_, drop_callback);
        glfwSetFramebufferSizeCallback(window_, resize_callback);

        glfwMakeContextCurrent(window_);

        if (const int version = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)); version == 0) {
            std::println("Unable to load OpenGL functions.");
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        ImGui::StyleColorsDark();
        auto &imgui_io = ImGui::GetIO();
        imgui_io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        imgui_io.IniFilename = nullptr;

        return true;
    }

    auto window::run() -> void {
        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1);

        glfwGetFramebufferSize(window_, &size_.at(0), &size_.at(1));
        glViewport(0, 0, size_.at(0), size_.at(1));
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        while (glfwWindowShouldClose(window_) == GLFW_FALSE) {
            glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwGetFramebufferSize(window_, &size_.at(0), &size_.at(1));
            glViewport(0, 0, size_.at(0), size_.at(1));

            for (auto &sprite : sprites_) { sprite->draw(size_); }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            callbacks_.draw_callback();
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window_);
            glfwWaitEvents();
        }
    }

    auto window::end() -> void {
        sprites_.clear();

        for (const auto &key : cursors_ | std::views::keys) { glfwDestroyCursor(key); }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    auto window::add_sprite(std::shared_ptr<sprite> &&spr) -> void { sprites_.emplace_back(std::move(spr)); }

    auto window::remove_sprite(const sprite &spr) -> void {
        std::erase_if(sprites_, [&spr](const std::shared_ptr<sprite> &sprite) -> bool { return sprite.get() == &spr; });
    }

    auto window::sprites() -> std::span<std::shared_ptr<sprite>> { return sprites_; }

    auto window::size() -> dimensions { return size_; }

    auto window::cursor_pos() -> std::array<double, 2> {
        double x_pos = {};
        double y_pos = {};
        glfwGetCursorPos(window_, &x_pos, &y_pos);
        return {x_pos, y_pos};
    }
} // namespace graphics
