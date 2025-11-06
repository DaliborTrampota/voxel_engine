

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;  // for nvidia
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;    // for amd
}
#endif

std::vector<GLuint64> m_textureHandles;
std::vector<GLuint> m_textures;

void createTexture(unsigned char* data, int width, int height, int channels, bool pixelated);

// Vertex Shader source
const char* vertexShaderSource = R"(
#version 450 core
#extension GL_ARB_bindless_texture : require
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
out vec2 uv;

void main()
{
    uv = aUV;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

// Fragment Shader source
const char* fragmentShaderSource = R"(
#version 450 core
#extension GL_ARB_bindless_texture : require


//layout(binding = 0, std430) readonly buffer ssbo {
//    sampler2D textures[];
//};

uniform sampler2D u_texture;

out vec4 FragColor;
in vec2 uv;

void main()
{
    FragColor = texture(u_texture, uv);
}
)";

// Callback for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Process input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// OpenGL debug callback
void GLAPIENTRY debugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    // Ignore non-significant error/warning codes
    if (id == 131'169 || id == 131'185 || id == 131'218 || id == 131'204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API: std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other"; break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR: std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER: std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: std::cout << "Type: Other"; break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW: std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
    throw std::runtime_error("GL Error");
}

int main(int argc, char* argv[]) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    std::cout << "OpenGL Debug Output enabled" << std::endl;

    // Check for bindless texture support
    if (!GLAD_GL_ARB_bindless_texture) {
        std::cerr << "ERROR: GL_ARB_bindless_texture is not supported!" << std::endl;
        return -1;
    }
    std::cout << "GL_ARB_bindless_texture is supported" << std::endl;

    // Verify function pointers are loaded
    if (!glGetTextureHandleARB || !glMakeTextureHandleResidentARB || !glUniformHandleui64ARB) {
        std::cerr << "ERROR: Bindless texture functions not loaded!" << std::endl;
        std::cerr << "  glGetTextureHandleARB: " << (void*)glGetTextureHandleARB << std::endl;
        std::cerr << "  glMakeTextureHandleResidentARB: " << (void*)glMakeTextureHandleResidentARB
                  << std::endl;
        std::cerr << "  glUniformHandleui64ARB: " << (void*)glUniformHandleui64ARB << std::endl;
        return -1;
    }
    std::cout << "All bindless texture functions loaded successfully" << std::endl;

    // Build and compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Build and compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders into shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data and buffers
    float vertices[] = {
        -0.5f,
        -0.5f,
        0.0f,  // Left
        0.5f,
        -0.5f,
        0.0f,  // Right
        0.0f,
        0.5f,
        0.0f  // Top
    };

    float uvs[] = {
        0.0f,
        0.0f,  // Bottom-left
        1.0f,
        0.0f,  // Bottom-right
        0.5f,
        1.0f  // Top-center
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO first
    glBindVertexArray(VAO);

    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(uvs), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(uvs), uvs);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(vertices)));
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // int numInstances = 10;
    // int textureSize = 32 * 32 * 3;
    // unsigned char* textureData = new unsigned char[textureSize];

    // for (int i = 0; i < numInstances; ++i) {
    //     const unsigned char limit = unsigned char(rand() % 231 + 25);
    //     // Randomly generate an unsigned char per RGB channel
    //     for (int j = 0; j < textureSize; ++j) {
    //         textureData[j] = unsigned char(rand() % limit);
    //     }

    //     GLuint texture;
    //     glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    //     glTextureStorage2D(texture, 1, GL_RGB8, 32, 32);
    //     glTextureSubImage2D(
    //         texture,
    //         // level, xoffset, yoffset, width, height
    //         0,
    //         0,
    //         0,
    //         32,
    //         32,
    //         GL_RGB,
    //         GL_UNSIGNED_BYTE,
    //         (const void*)&textureData[0]
    //     );
    //     glGenerateTextureMipmap(texture);

    //     // Retrieve the texture handle after we finish creating the texture
    //     const GLuint64 handle = glGetTextureHandleARB(texture);
    //     if (handle == 0) {
    //         std::cerr << "Error! Handle returned null" << std::endl;
    //         exit(-1);
    //     }

    //     m_textures.push_back(texture);
    //     m_textureHandles.push_back(handle);
    // }

    // Use the program before setting uniforms
    glUseProgram(shaderProgram);
    GLenum progErr = glGetError();
    if (progErr != GL_NO_ERROR) {
        std::cerr << "Error using program: 0x" << std::hex << progErr << std::dec << std::endl;
    }

    // Create a white texture
    unsigned char* data = new unsigned char[100 * 100 * 3];
    const unsigned char limit = static_cast<unsigned char>(rand() % 231 + 25);
    // Randomly generate an unsigned char per RGB channel
    for (int j = 0; j < 100 * 100 * 3; ++j) {
        data[j] = static_cast<unsigned char>(rand() % limit);
    }
    createTexture(data, 100, 100, 3, false);
    //delete[] data;

    // Get the uniform location for our bindless texture sampler
    GLint location = glGetUniformLocation(shaderProgram, "u_texture");
    if (location == -1) {
        std::cerr << "Error: could not find uniform 'u_texture'" << std::endl;
    } else {
        std::cout << "Found uniform 'u_texture' at location: " << location << std::endl;

        // Query the uniform type to verify it's a sampler
        GLint uniformCount = 0;
        glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &uniformCount);
        std::cout << "Active uniforms in program: " << uniformCount << std::endl;

        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        for (GLint i = 0; i < uniformCount; i++) {
            glGetActiveUniform(shaderProgram, i, sizeof(name), &length, &size, &type, name);
            std::cout << "Uniform " << i << ": " << name << " (type=0x" << std::hex << type
                      << std::dec << ", size=" << size << ")" << std::endl;
        }
    }

    // Set the bindless texture handle to the uniform
    // The texture is already made resident in createTexture()
    if (!m_textureHandles.empty()) {
        GLuint64 handle = m_textureHandles[0];
        std::cout << "Attempting to set texture handle: 0x" << std::hex << handle << std::dec
                  << std::endl;

        // Verify texture is resident
        GLboolean isResident = glIsTextureHandleResidentARB(handle);
        std::cout << "Texture handle resident status: " << (isResident ? "YES" : "NO") << std::endl;

        // Check current program
        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        std::cout << "Current program: " << currentProgram << " (expected: " << shaderProgram << ")"
                  << std::endl;

        if (!isResident) {
            std::cerr << "ERROR: Texture handle is not resident!" << std::endl;
        } else if (location != -1 && currentProgram == (GLint)shaderProgram) {
            // Try using glProgramUniformHandleui64ARB instead
            std::cout << "Calling glProgramUniformHandleui64ARB..." << std::endl;
            glProgramUniformHandleui64ARB(shaderProgram, location, handle);
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "GL Error with glProgramUniformHandleui64ARB: 0x" << std::hex << err
                          << std::dec << std::endl;

                // Try the non-program version
                std::cout << "Trying glUniformHandleui64ARB instead..." << std::endl;
                glUniformHandleui64ARB(location, handle);
                err = glGetError();
                if (err != GL_NO_ERROR) {
                    std::cerr << "GL Error with glUniformHandleui64ARB: 0x" << std::hex << err
                              << std::dec << std::endl;
                } else {
                    std::cout << "Successfully set texture handle with glUniformHandleui64ARB!"
                              << std::endl;
                }
            } else {
                std::cout << "Successfully set texture handle with glProgramUniformHandleui64ARB!"
                          << std::endl;
            }
        }
    }

    //GLuint textureBuffer;
    //glCreateBuffers(1, &textureBuffer);
    //glNamedBufferStorage(
    //    textureBuffer,
    //    sizeof(GLuint64) * m_textureHandles.size(),
    //    (const void*)m_textureHandles.data(),
    //    GL_DYNAMIC_STORAGE_BIT
    //);

    //glEnable(GL_DEPTH_TEST);

    //// Mark all as resident
    //for (GLuint64 handle : m_textureHandles) {
    //    glMakeTextureHandleResidentARB(handle);
    //}

    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureBuffer);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}


void createTexture(unsigned char* data, int width, int height, int channels, bool pixelated) {
    unsigned int tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);

    // Use DSA functions consistently
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameterfv(tex, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(0, 0, 0, 0)));

    GLenum format = channels == 1   ? GL_RED
                    : channels == 2 ? GL_RG
                    : channels == 3 ? GL_RGB
                                    : GL_RGBA;

    GLenum internalFormat = channels == 1   ? GL_R8
                            : channels == 2 ? GL_RG8
                            : channels == 3 ? GL_RGB8
                                            : GL_RGBA8;

    printf(
        "Creating texture: texID=%u, format=%s, data=%p\n",
        tex,
        channels == 1   ? "RED"
        : channels == 2 ? "RG"
        : channels == 3 ? "RGB"
                        : "RGBA",
        data
    );

    glTextureStorage2D(tex, 1, internalFormat, width, height);
    glTextureSubImage2D(tex, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("GL Error after glTexImage2D: 0x%x\n", err);
    }

    GLuint64 handle = glGetTextureHandleARB(tex);
    if (handle == 0) {
        printf("Error! Handle returned null\n");
        exit(-1);
    }
    printf("Got texture handle: 0x%llx for texID=%u\n", handle, tex);

    // Make texture resident immediately and keep it resident
    glMakeTextureHandleResidentARB(handle);
    GLenum resErr = glGetError();
    if (resErr != GL_NO_ERROR) {
        printf("GL Error making texture resident: 0x%x\n", resErr);
    } else {
        printf("Texture made resident successfully\n");
    }

    m_textureHandles.push_back(handle);
    m_textures.push_back(tex);
}