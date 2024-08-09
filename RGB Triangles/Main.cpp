#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<fstream>
#include <sstream>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 720


using namespace std;

void fbCallback(GLFWwindow* window, GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
}


// Red triangle centered on the origin
float triangleVertices[] = {
	-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

// Cursed formatting
const char* fragmentSource = "#version 330 core\n"
							"in vec3 vertexColor;\n"
							"out vec4 FragColor;\n"
							"void main() {\n"
								"FragColor = vec4(vertexColor.x, vertexColor.y, vertexColor.z, 1.0f);\n"
							"}\0";

const char* vertexSource = "#version 330 core\n"
							"layout(location = 0) in vec3 aPos;\n"
							"layout(location = 1) in vec3 aColor;\n"
							"out vec3 vertexColor;\n"
							"void main() {\n"
								"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
								"vertexColor = aColor;\n"
							"}\0";


int main() {
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (!glfwInit()) {
		cout << "Failed to initialize glfw. Exiting." << endl;
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RGB Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);

	// glfwGetProcAddress requires a context, so initializing a window and making its context current is neccesary
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD. Exiting." << endl;
		return 1;
	}
	
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, &fbCallback);

	int result; // Used for storing the results of various operations
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		char msg[256];
		glGetShaderInfoLog(vertexShader, 256, NULL, msg);
		cout << "Could not compile vertex shader. Error: \n" << msg << endl;
		return 1;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		char msg[256];
		glGetShaderInfoLog(fragmentShader, 256, NULL, msg);
		cout << "Could not compile fragment shader. Error: \n" << msg << endl;
		return 1;
	}

	unsigned int leprogramme = glCreateProgram(); // leprogramme
	glAttachShader(leprogramme, vertexShader);
	glAttachShader(leprogramme, fragmentShader);
	glLinkProgram(leprogramme);
	
	glGetProgramiv(leprogramme, GL_LINK_STATUS, &result);
	if (!result) {
		char msg[256];
		glGetProgramInfoLog(leprogramme, sizeof(msg), NULL, msg);
		cout << "Could not link shader program. Error: \n" << msg << endl;
		return 1;
	}

	// These are no longer needed
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // I LOVE POINTER ARITHMETIC
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float))); // ME TOO
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(leprogramme);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}