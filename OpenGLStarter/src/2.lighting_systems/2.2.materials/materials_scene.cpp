#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../utils.h"
#include "../../shaders/shader.h"
#include "../../stb/stb_image.h"
#include "../../camera.h"

constexpr int W_WIDTH = 800;
constexpr int W_HEIGHT = 600;

constexpr glm::vec3 lightPos(1.5f, 0.0f, 1.0f);

int materials_scene_main()
{
	// initializing window settings
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Materials", NULL, NULL);
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

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f,  1.0f
	};


	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f,-15.0f),
		glm::vec3(-1.5f,-2.2f,-2.5f),
	};

	// cube object
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// light object
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	Camera camera(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45.0f
	);

	glfwSetWindowUserPointer(window, &camera);

	Shader lightSourceShader("src/shaders/lighting/vertex.vert", "src/shaders/lighting/light_source.frag");
	Shader shader("src/shaders/lighting/vertex.vert", "src/shaders/material_samples/material.frag");


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());

		for (unsigned int i = 0; i < (int)cubePositions->length(); i++)
		{
			shader.use();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			// model = glm::rotate(model, glm::radians((float)std::fmod(glfwGetTime() * 50, 360.0)), glm::vec3(0.0f, 1.0f, 0.0f));
			// model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));

			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			shader.setMat4("model", model);
			shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			shader.setVec3("viewPos", cameraPos);

			shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
			shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
			shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
			shader.setFloat("material.shininess", 32.0f);

			glm::vec3 lightColor;
			lightColor.x = sin(glfwGetTime() * 2.0f);
			lightColor.y = sin(glfwGetTime() * 0.7f);
			lightColor.z = sin(glfwGetTime() * 1.3f);
			glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

			shader.setVec3("light.position", lightPos);
			shader.setVec3("light.ambient", ambientColor);
			shader.setVec3("light.diffuse", diffuseColor);
			shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
			
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lightSourceShader.use();
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		// model = glm::rotate(model, glm::radians((float)std::fmod(glfwGetTime() * 50, 360.0)), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSourceShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}