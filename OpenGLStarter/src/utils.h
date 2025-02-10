#pragma once

// adjusts the viewport when user resizes it
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// process input in the renderer
void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
