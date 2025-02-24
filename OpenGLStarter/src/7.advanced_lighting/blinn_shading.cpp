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
#include "../Framebuffer.h"

constexpr int W_WIDTH = 800;
constexpr int W_HEIGHT = 600;

int main()
{
	// initializing window settings
	glfwInit();

	// glfwWindowHint(GLFW_SAMPLES, 4); // only use when we aren't using framebuffers for msaa
	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Blinn Shading", NULL, NULL);
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
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	stbi_set_flip_vertically_on_load(true);

	// camera settings
	Camera camera(
		glm::vec3(0.0f, 50.0f, 200.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45.0f
	);
	glfwSetWindowUserPointer(window, &camera);

	// frame buffers
	Framebuffer msFBO(W_WIDTH, W_HEIGHT, 4);
	Framebuffer ppFBO(W_WIDTH, W_HEIGHT);
	unsigned int ppVAO = createFrameVAO();

	// floor
	unsigned int floorVAO = createQuadVAO();

	Shader shader("src/shaders/lighting/vertex.vert", "src/shaders/advanced_lighting/blinn_shading.frag");
	Shader frame("src/shaders/post_processing/framebuffer_quad.vert", "src/shaders/post_processing/outlines.frag");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// first pass
		msFBO.bind();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// scene drawing
		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 1000.f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());

		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);


		// second pass
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO.FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFBO.FBO);
		glBlitFramebuffer(0, 0, W_WIDTH, W_HEIGHT, 0, 0, W_WIDTH, W_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		frame.use();
		glBindVertexArray(ppVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ppFBO.getTexture());
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}