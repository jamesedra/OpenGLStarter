#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../utils.h"
#include "../../shaders/shader.h"

int main() {

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello world triangle", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	// vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//vertex buffer object, plan is to store it in VAO
	unsigned int VBO; 
	glGenBuffers(1, &VBO);

	// note that we can bind multiple buffer objecs, as long as it's a different type of buffer (for this one, we are using GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// link vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	Shader ourShader("src/shaders/gradient/gradient.vert", "src/shaders/gradient/gradient.frag");

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// clear frame is important
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();

		// update uniform color (apparently, changing alpha didn't do anything lol
		float timeValue = glfwGetTime();
		float alphaValue = (sin(timeValue) / 2.0f) + 0.5f;
		ourShader.setFloat("aValue", alphaValue);

		// renderer
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	return 0;
}