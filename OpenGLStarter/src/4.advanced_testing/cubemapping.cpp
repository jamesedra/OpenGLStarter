#include <iostream>
#include <map>
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
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Cube Maps", NULL, NULL);
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	stbi_set_flip_vertically_on_load(true);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create texture for frame buffer's color attachment
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W_WIDTH, W_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind

	// attach texture as the color attachment to the bound framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// create a render buffer object for frame buffer
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W_WIDTH, W_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach the render buffer object as the depth and stencil attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// validate frame buffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete." << std::endl;

	// unbind frame buffer as we finished the operation. This sets back the frame buffer to the default window.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = {
		// positions(2) : uvs(2)
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0, -1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f, 1.0f
	};

	unsigned int quadVAO;
	glGenVertexArrays(1, &quadVAO);

	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glBindVertexArray(quadVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// camera settings
	Camera camera(
		glm::vec3(1.0f, 1.0f, 3.0f),
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
		-0.5f,-0.5f, 0.5f, 0.0f, 0.0f, // bottom-left

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
	Shader frame("src/shaders/shader_testing/framebuffer_quad.vert", "src/shaders/shader_testing/framebuffer_quad.frag");
	Shader skybox("src/shaders/cubemapping/cubemap.vert", "src/shaders/cubemapping/cubemap.frag");

	// cubemap object
	stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	unsigned int skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);

	unsigned int skyVBO;
	glGenBuffers(1, &skyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(skyboxVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	std::vector<std::string> faces = {
		"src/resources/skybox/right.jpg",
		"src/resources/skybox/left.jpg",
		"src/resources/skybox/top.jpg",
		"src/resources/skybox/bottom.jpg",
		"src/resources/skybox/front.jpg",
		"src/resources/skybox/back.jpg"
	};

	unsigned int cubemapTexture = loadCubemap(faces);
	stbi_set_flip_vertically_on_load(true);

	shader.use();
	shader.setInt("texture1", 0);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// First pass
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// draw scene (included in first pass)
		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());

		// cube
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		shader.setVec3("lightPos", glm::vec3(2.0, 2.0, 2.0));

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw cubemap
		glFrontFace(GL_CW);
		//  glDepthMask(GL_FALSE); // disable depth writes
		glDepthFunc(GL_LEQUAL);
		skybox.use();
		skybox.setMat4("projection", projection);
		skybox.setMat4("view", glm::mat4(glm::mat3(view)));
		skybox.setInt("skybox", 0);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);

		// second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // set frame buffer to default again
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		frame.use();
		frame.setInt("screenTexture", 0);
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
