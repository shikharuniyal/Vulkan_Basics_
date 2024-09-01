
#pragma once

#define GLFW_INCLUDE_VULKAN

#include<GLFW/glfw3.h>
#include<stdexcept>
#include<vector>
#include<iostream>

#include "Utilities.h"


class vulkanRenderer
{
public:
	vulkanRenderer();
	int init(GLFWwindow* win);
	void cleanup();

	~vulkanRenderer();
private:
	GLFWwindow* window;//it is the same variable name used in main but this one's scope is only in vulkanRenderer class

	//vulkan Components
	VkInstance instance;

	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	}mainDevice;

	VkQueue graphicsQueue;
	//Vulkan Functions 
	// -vulkan creation functions

	void createInstance();
	void createLogicalDevice();

	// -Get Functions
	void getPhysicalDevice();

	//-vulkan support functions
	//--checker functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);//we pass the pointer to the extension list we get from the "createInstance()"
	bool checkDeviceSuitable(VkPhysicalDevice device);// check the provided device and check if it fits the given criteria

	// -- Getter functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};


