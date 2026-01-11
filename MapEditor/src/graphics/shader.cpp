#include "shader.hpp"
#include <array>
#include <cstddef>
#include <fstream>
#include <print>
#include <sstream>
#include "zstring_view.hpp"

static auto compile_shader(const GLenum type, const char *source) -> GLuint {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = {};
    constexpr size_t info_log_length = 512;
    std::array<char, info_log_length> info_log = {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log.data());
        std::println("Shader compilation error: {}\n", info_log.data());
    }

    return shader;
}

namespace graphics {
    shader::shader(shader &&other) noexcept : program_{other.program_} { other.program_ = 0; }

    auto shader::operator=(shader &&other) noexcept -> shader & {
        if (this == &other) { return *this; }

        glDeleteProgram(program_);
        program_ = other.program_;
        other.program_ = 0;

        return *this;
    }

    shader::~shader() {
        glDeleteProgram(program_);
    }

    auto shader::program() const -> GLuint { return program_; }

    shader::shader(const zstring_view vertex_path, const zstring_view fragment_path) : program_(glCreateProgram()) {
        std::ifstream vertex_file{vertex_path.str()};
        std::ifstream fragment_file{fragment_path.str()};
        std::stringstream vertex_buffer;
        std::stringstream fragment_buffer;
        vertex_buffer << vertex_file.rdbuf();
        fragment_buffer << fragment_file.rdbuf();
        vertex_file.close();
        fragment_file.close();

        const auto vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_buffer.str().c_str());
        const auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_buffer.str().c_str());
        glAttachShader(program_, vertex_shader);
        glAttachShader(program_, fragment_shader);
        glLinkProgram(program_);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
} // namespace graphics
