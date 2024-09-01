

#define GLFW_INCLUDE_VULKAN

#include<iostream>
#include<GLFW/glfw3.h>

#include<stdexcept>
#include<vector>
#include<iostream>
#include"vulkanRenderer.h"


GLFWwindow* window;
vulkanRenderer vulkanrenderer;//creating vulkanRenderer OBJECT storing in stack fashioned way

void initWin(std::string Wname = "teat_0 window", const int width = 800, const int height = 600) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, Wname.c_str(), nullptr, nullptr);
}


int main() {

	initWin();
	if (vulkanrenderer.init(window) == EXIT_FAILURE) return EXIT_FAILURE;
	//loop until closed
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	vulkanrenderer.cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}