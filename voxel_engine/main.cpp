#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

#include "ShaderPipeline.h"
#include "Shader.h"




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); for apple

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    ShaderPipeline pipeline;
    {
	    Shader vert("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	    Shader frag("shaders/PixelShader.glsl", GL_FRAGMENT_SHADER);

	    if (!pipeline.registerShader(GL_VERTEX_SHADER, vert)) std::cout << "Vert shader not registered" << std::endl;
        if (!pipeline.registerShader(GL_FRAGMENT_SHADER, frag)) std::cout << "Fragment shader not registered" << std::endl;
        if (!pipeline.link()) return 1;
    }


    
    float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0,
		0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0,
		0.0f,  0.5f, 0.0f, 0.0, 0.0, 1.0
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    pipeline.registerAttribute(0, 3, GL_FLOAT, 6);
    pipeline.registerAttribute(1, 3, GL_FLOAT, 6, 3);


	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind VBO
	glBindVertexArray(0); //unbind VAO

    glBindVertexArray(VAO); //bind VAO back


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float time = 0.0;// glfwGetTime();

    

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		pipeline.use();
        pipeline.setFloat("time", time);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        time += 0.001;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(pipeline.ID);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

}
