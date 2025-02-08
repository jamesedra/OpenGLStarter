#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../utils.h"

const char* vertexShaderSource = "#version 330 core\n"
								 "layout (location = 0) in vec2 aPos;\n"
								 "out vec4 vertexColor; \n"
								 "void main()\n"
								 "{\n"
								 " gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
								 "vertexColor = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
								 "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
								   "uniform float bValue; \n"
								   "in vec4 vertexColor;\n"
								   "out vec4 FragColor; \n"
								   "void main() \n"
								   "{ \n"
								   "FragColor = vec4(vertexColor.rg, bValue, 1.0); \n"
								   "}\0";
int main() {

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello world triangle", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float vertices[] = {
		-0.5f, -0.5f, 
		 0.5f, -0.5f,
		 0.0f,  0.5f
	};

	// vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//vertex buffer object, plan is to store it in VAO
	unsigned int VBO; 
	glGenBuffers(1, &VBO);

	// note that we can bind multiple buffer objecs, as long as it's a different type of buffer (for this one, we are using GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// link vertex attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// debugging vertex shader compilation
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return -1;
	}

	// fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return -1;
	}

	// shader program
	unsigned int shaderProgram = glCreateProgram(); // holds the id of the program object
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

		return -1;
	}

	

	// so we don't need these anymore:
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// clear frame is important
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram); // when called, every shader and rendering call after will now use this program object

		// update uniform color
		float timeValue = glfwGetTime();
		float blueValue = (sin(timeValue) / 2.0f) + 0.5f;
		int floatColorLocation = glGetUniformLocation(shaderProgram, "bValue");
		glUniform1f(floatColorLocation, blueValue);

		// renderer
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// buffer swapping and event polling
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	return 0;
}