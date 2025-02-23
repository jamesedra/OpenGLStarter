#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Model.h"
#include "../utils.h"
#include "../shaders/shader.h"
#include "../stb/stb_image.h"
#include "../camera.h"

constexpr int W_WIDTH = 800;
constexpr int W_HEIGHT = 600;

int inst_arrays_main()
{
	// initializing window settings
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Instanced Arrays", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	stbi_set_flip_vertically_on_load(true);

	// camera settings
	Camera camera(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45.0f
	);

	// cube object
	unsigned int cubeVAO = createCubeVAO();
	
	// set up instancing offset values
	glm::vec2 translations[1000];
	int index = 0;
	float offset = 0.5f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 0.5f + offset;
			translation.y = (float)y / 0.5f + offset;
			translations[index++] = translation;
		}
	}

	// instanced array
	glBindVertexArray(cubeVAO); // note that when we create a cubeVAO, the function unbinds the current vertex array so we have to bind it again

	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttribDivisor(3, 1);

	glfwSetWindowUserPointer(window, &camera);

	Shader shader("src/shaders/instancing/instance_arrays.vert", "src/shaders/shader_testing/blue.frag");
	

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		glBindVertexArray(cubeVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1000);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}