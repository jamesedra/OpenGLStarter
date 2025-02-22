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

int instancing_main()
{
	// initializing window settings
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Instancing", NULL, NULL);
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

	glfwSetWindowUserPointer(window, &camera);
	Shader shader("src/shaders/instancing/instancing.vert", "src/shaders/model_loading/base_material.frag");

	Model object("src/resources/objects/backpack/backpack.obj");

	glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.f);

	// set up instancing offset values
	glm::vec2 translations[100];
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

	shader.use();
	for (unsigned int i = 0; i < 100; i++)
	{
		shader.setVec2(("offsets[" + std::to_string(i) + "]"), translations[i]);
	}

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		object.DrawInstanced(shader, 100);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}