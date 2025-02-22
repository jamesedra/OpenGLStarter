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

int main()
{
	// initializing window settings
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Asteroid Belt (non-instanced)", NULL, NULL);
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

	unsigned int amount = 2000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime());
	float radius = 50.0;
	float offset = 2.5f;
	

	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		float angle = (float) i / (float) amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;

		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset; // randomize displacement again

		float y = displacement * 0.4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;

		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		model = glm::scale(model, glm::vec3(rand() % 20 / 100.f + 0.05));

		float rotAngle = rand() % 360;
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		modelMatrices[i] = model;
	}

	glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.f);

	
	glfwSetWindowUserPointer(window, &camera);

	Shader shader("src/shaders/lighting/vertex.vert", "src/shaders/model_loading/base_material.frag");

	Model planet("src/resources/objects/planet/planet.obj");
	Model rock("src/resources/objects/rock/rock.obj");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());
		
		// planet
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		planet.Draw(shader);

		for (unsigned int i = 0; i < amount; i++)
		{
			shader.setMat4("model", modelMatrices[i]);
			rock.Draw(shader);
		}

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}