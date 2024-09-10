#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

#include "structures/ShaderPipeline.h"
#include "structures/Shader.h"

#include "Game.h"
#include "VertexData.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


void printMat4(glm::mat4 mat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << mat[j][i] << " ";
		}
		std::cout << std::endl;
	}

}

Game game;

int main() {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); for apple

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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


    glEnable(GL_DEPTH_TEST);


    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float vertices[36 * 3];
    float colors[36 * 3];
    int idx = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
            colors[idx + 0] = 0.0f;
            colors[idx + 1] = 1.0f;
            colors[idx + 2] = 0.5f;
            for (int c = 0; c < 3; c++) {
			    vertices[idx] = vert::vertices[vert::faces[i][j]][c] + 0.5f;
                idx++;
            }
		}
	}
    
  //  float vertices[] = {
		//-0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0,
		//0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0,
		//0.0f,  0.5f, 0.0f, 0.0, 0.0, 1.0
  //  };
  //  unsigned int indices[] = {  // note that we start from 0!
  //      0, 1, 3,  // first Triangle
  //      1, 2, 3   // second Triangle
  //  };
    unsigned int VBO, VAO, VBO_col;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_col);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    pipeline.registerAttribute(0, 3, GL_FLOAT, 0);


    //pipeline.registerAttribute(0, 3, GL_FLOAT, 6);
    //pipeline.registerAttribute(1, 3, GL_FLOAT, 6, 3);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    pipeline.registerAttribute(1, 3, GL_FLOAT, 0);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind VBO
	glBindVertexArray(0); //unbind VAO

    glBindVertexArray(VAO); //bind VAO back


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float time = 0.0;// glfwGetTime();
    
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    
    
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    pipeline.use();
    glm::mat4 projection = game.cam.getProjection();
    pipeline.setMat4("projection", projection);
	printMat4(projection);

    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        game.processInput(window, deltaTime);
        game.update(deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pipeline.setFloat("time", time);
        glm::mat4 view = game.cam.getView();
        pipeline.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 1.0f));
		pipeline.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            pipeline.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        time += 0.001f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(pipeline.ID);

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    game.cam.rotate(xoffset, yoffset);
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	game.cam.resize(width, height);

}