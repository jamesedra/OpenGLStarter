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
#include <map>

constexpr int W_WIDTH = 800;
constexpr int W_HEIGHT = 600;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Face Culling", NULL, NULL);
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); // culls front face
	glFrontFace(GL_CW); // determines that the front face is the clockwise winding order. Hence, this will cull the back instead of the front since the previous line culls the "front face" and the front face is now determined as the back face this is so convoluted haha
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	stbi_set_flip_vertically_on_load(true);

	// camera settings
	Camera camera(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45.0f
	);

	glfwSetWindowUserPointer(window, &camera);

	float vertices[] = {
		// back face
		-0.5f,-0.5f,-0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f,-0.5f, 1.0f, 1.0f, // top-right
		0.5f,-0.5f,-0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f,-0.5f, 1.0f, 1.0f, // top-right
		-0.5f,-0.5f,-0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f,-0.5f, 0.0f, 1.0f, // top-left

		// front face
		-0.5f,-0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f,-0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		- 0.5f,-0.5f, 0.5f, 0.0f, 0.0f, // bottom-left

		// left face
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, 0.5f,-0.5f, 1.0f, 1.0f, // top-left
		-0.5f,-0.5f,-0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f,-0.5f,-0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f,-0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right

		// right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f,-0.5f,-0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f,-0.5f, 1.0f, 1.0f, // top-right
		0.5f,-0.5f,-0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f,-0.5f, 0.5f, 0.0f, 0.0f, // bottom-left

		// bottom face
		-0.5f,-0.5f,-0.5f, 0.0f, 1.0f, // top-right
		0.5f,-0.5f,-0.5f, 1.0f, 1.0f, // top-left
		0.5f,-0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		0.5f,-0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		-0.5f,-0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		-0.5f,-0.5f,-0.5f, 0.0f, 1.0f, // top-right

		// top face
		-0.5f, 0.5f,-0.5f, 0.0f, 1.0f, // top-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f,-0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f,-0.5f, 0.0f, 1.0f, // top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f // bottom-left
	};

	// cube object
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader shader("src/shaders/lighting/vertex.vert", "src/shaders/lighting/diffuse.frag");

	shader.use();
	shader.setInt("texture1", 0);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());

		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f));
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		shader.setVec3("lightPos", glm::vec3(2.0,2.0,2.0));

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
