#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "camera.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mouse stuff
bool firstMouse = false;
float lastX = 400;
float lastY = 300;
float pitch = 0.0f;
float yaw = -90.0f;
float zoom = 45.0f;


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) return;

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


	const float cameraSpeed = 2.5f * deltaTime; // Adjust as needed.

	// Update camera position based on key input:
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->setCameraPos(camera->getCameraPos() + cameraSpeed * camera->getCameraFront());
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->setCameraPos(camera->getCameraPos() - cameraSpeed * camera->getCameraFront());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		// Calculate the right vector: normalized cross product of front and up.
		glm::vec3 right = glm::normalize(glm::cross(camera->getCameraFront(), camera->getCameraUp()));
		camera->setCameraPos(camera->getCameraPos() - right * cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		glm::vec3 right = glm::normalize(glm::cross(camera->getCameraFront(), camera->getCameraUp()));
		camera->setCameraPos(camera->getCameraPos() + right * cameraSpeed);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) return;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	camera->setCameraFront(glm::normalize(direction));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) return;

	zoom -= (float)yoffset;
	if (zoom < 1.0f) zoom = 1.0f;
	if (zoom > 45.0f) zoom = 45.0f;

	camera->setFOV(zoom);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}