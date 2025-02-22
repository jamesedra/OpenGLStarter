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

	// glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Asteroid Belt (MSAA)", NULL, NULL);
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

	// frame buffer init
	unsigned int msFramebuffer;
	glGenFramebuffers(1, &msFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, msFramebuffer);

	// assign multisample texture as the framebuffer's color attachment
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, W_WIDTH, W_HEIGHT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);

	// create multisample render buffer object
	unsigned int msRbo;
	glGenRenderbuffers(1, &msRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, msRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, W_WIDTH, W_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRbo);

	// validate frame buffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete." << std::endl;

	// unbind frame buffer as we finished the operation. This sets back the frame buffer to the default window.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// for post processing framebuffer
	unsigned int ppFramebuffer;
	glGenFramebuffers(1, &ppFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ppFramebuffer);

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

	unsigned int amount = 20000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime());
	float radius = 150.0f;
	float offset = 25.0f;


	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		float angle = glm::radians((float)i / (float)amount * 360.0f);
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


	Model planet("src/resources/objects/planet/planet.obj");
	Model rock("src/resources/objects/rock/rock.obj");

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	const std::vector<Mesh>& rockMeshes = rock.getMeshes();

	for (unsigned int i = 0; i < rockMeshes.size(); i++)
	{
		unsigned int VAO = rockMeshes[i].getVAO();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		std::size_t v4s = sizeof(glm::vec4);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(1 * v4s));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(2 * v4s));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(3 * v4s));
		glEnableVertexAttribArray(6);

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	Shader shader("src/shaders/lighting/vertex.vert", "src/shaders/model_loading/base_material.frag");
	Shader rockShader("src/shaders/instancing/asteroid_instancing.vert", "src/shaders/model_loading/base_material.frag");

	Shader frame("src/shaders/shader_testing/framebuffer_quad.vert", "src/shaders/post_processing/outlines.frag");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// first pass
		glBindFramebuffer(GL_FRAMEBUFFER, msFramebuffer);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// scene drawing
		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 1000.f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPos = camera.getCameraPos();
		view = glm::lookAt(cameraPos, cameraPos + camera.getCameraFront(), camera.getCameraUp());

		// planet
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		planet.Draw(shader);

		// rocks
		rockShader.use();
		rockShader.setMat4("projection", projection);
		rockShader.setMat4("view", view);
		rock.DrawInstanced(rockShader, amount);

		// second pass
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msFramebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFramebuffer);
		glBlitFramebuffer(0, 0, W_WIDTH, W_HEIGHT, 0, 0, W_WIDTH, W_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		frame.use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}