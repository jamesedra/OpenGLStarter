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

int msaa_refactored_main()
{
	// initializing window settings
	glfwInit();

	// glfwWindowHint(GLFW_SAMPLES, 4); // only use when we aren't using framebuffers for msaa
	GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Asteroid Belt (MSAA, refactored)", NULL, NULL);
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

	// frame buffers. third parameter creates a multisampling counterpart
	Framebuffer msFBO(W_WIDTH, W_HEIGHT, 4);
	Framebuffer ppFBO(W_WIDTH, W_HEIGHT);

	unsigned int quadVAO = createFrameVAO();

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
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO.FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFBO.FBO);
		glBlitFramebuffer(0, 0, W_WIDTH, W_HEIGHT, 0, 0, W_WIDTH, W_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		frame.use();
		glBindVertexArray(quadVAO);
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