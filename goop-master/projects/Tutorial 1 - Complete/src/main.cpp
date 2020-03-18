#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <iostream>

// We'll use a helper method to create our shader program, and load in our simple shader
// @returns The handle to the shader program
GLuint CreateShader() {
	GLuint result = glCreateProgram();

	// Our vertex shader source
	const char* vs_source = R"LIT(#version 450
		layout (location = 0) in vec3 inPosition;
		layout (location = 1) in vec4 inColor;

		layout (location = 0) out vec4 outColor;

		void main() {
			gl_Position = vec4(inPosition, 1);
			outColor = inColor;
		}
	)LIT";

	// Our fragment shader source
	const char* fs_source = R"LIT(#version 450
		layout (location = 0) in vec4 inColor;

		layout (location = 0) out vec4 outColor;

		void main() {
			outColor = inColor;
		}
	)LIT";

	// Create our vertex shader and attach it to our program
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, nullptr);
	glAttachShader(result, vs);

	// Create our fragment shader and attach it to our program
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, nullptr);
	glAttachShader(result, fs);

	// Link our program together
	glLinkProgram(result);

	// Detach and clean up the shader parts
	glDetachShader(result, vs);
	glDeleteShader(vs);
	glDetachShader(result, fs);
	glDeleteShader(fs);

	return result;
}

// We'll use a helper method for loading up our VAO
// @param data The data to load into the buffer
// @param numVerts The number of vertices to load
// @returns The handle of the VAO
GLuint CreateVAO(float* data, size_t numVerts) {
	GLuint result = 0;
	GLuint vbo = 0;

	// Determine the stride (the size of each individual vertex)
	size_t stride = sizeof(float) * (4 + 3);

	// Create and bind our VAO
	glCreateVertexArrays(1, &result);
	glBindVertexArray(result);

	// Create our VBO and load the data into it
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glNamedBufferData(vbo, numVerts * stride, data, GL_STATIC_DRAW);

	// Enable vertex attribute 0 (our position)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, nullptr);

	// Enable vertex attribute 1 (our color)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, stride, reinterpret_cast<void*>(sizeof(float) * 3));

	// Unbind the VAO to prevent further modification
	glBindVertexArray(0);
	
	// We can delete the VBO now, the VAO will keep a reference to it
	glDeleteBuffers(1, &vbo);

	return result;
}

int main() {
	// Attempt to initialize GLFW
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW!" << std::endl;
		return 1;
	}

	// Create our GLFW window and make it the current rendering context
	GLFWwindow* window = glfwCreateWindow(600, 600, "Hello world!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Attempt to initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLFW!" << std::endl;
		return 2;
	}

	// Test OpenGL
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version:  " << glGetString(GL_VERSION) << std::endl;

	// Create our shader
	GLuint shader = CreateShader();

	// Create our triangle and load it into a VAO
	float data[] = {
		-0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f
	};	
	GLuint vao = CreateVAO(data, 3);

	// Loop as long as our window is open
	while (!glfwWindowShouldClose(window)) {
		// Poll for input, window moves, etc...
		glfwPollEvents();

		// We tell OpenGL to use our shader
		glUseProgram(shader);
		// Our data will come from the VAO
		glBindVertexArray(vao);
		// Draw 3 vertices as a single triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Swap our window's front and back buffers
		glfwSwapBuffers(window);
	}

	// Unbind any resources we are using
	glUseProgram(0);
	glBindVertexArray(0);

	// Delete our OpenGL resources
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader);

	// Shut down GLFW
	glfwTerminate();

	return 0;
}
