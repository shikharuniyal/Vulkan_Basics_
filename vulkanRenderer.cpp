#include "vulkanRenderer.h"



vulkanRenderer::vulkanRenderer() {}

int vulkanRenderer::init(GLFWwindow* win) {
	window = win;
	try {
		createInstance();
		getPhysicalDevice();
		createLogicalDevice();
	}
	catch (const std::runtime_error& e) {
		std::cout << "ERROR is\n" << e.what();
		return EXIT_FAILURE;
	}

	return 0;

}

void vulkanRenderer::cleanup()
{
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}

vulkanRenderer::~vulkanRenderer()
{

}
void vulkanRenderer::createInstance() {

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "basics of Vulkan";
	//appInfo.applicationVersion = VK_MAKE_API_VERSION(1.1.0.0);	depreciated correct it
	appInfo.pEngineName = "Naah no Engine";
	//appInfo.engineVersion = VK_MAKE_VERSION(1.0.0);   depreciated
	appInfo.apiVersion = VK_API_VERSION_1_3;  // to check use CMD >vulkanInfo --summary



	/*typedef struct VkInstanceCreateInfo {
	VkStructureType             sType;
	const void*                 pNext;
	VkInstanceCreateFlags       flags;
	const VkApplicationInfo*    pApplicationInfo;
	uint32_t                    enabledLayerCount;
	const char* const*          ppEnabledLayerNames;
	uint32_t                    enabledExtensionCount;
	const char* const*          ppEnabledExtensionNames;
	*/
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	// createInfo.pNext = <not using> //used to point to different struct 
	// of similar type to add up more info to this creaInfo struct
	//createInfo.flags = <not using>  uses uint32_t bit field modified  number to 
	// direct what happens to createInfo depending on the either of the flags

//==========QUERYING EXTENSIONS===================

	//create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	//set up extensions Instance we'll use
	uint32_t glfwExtensionCnt = 0;		//GlFW may require multiple extensions	
	const char** glfwExtensions;		//extensions passed as array of cstrings so need pointers, so need pointer(the array) to pointer to cstring

	//getting GLFW Extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCnt);//parameters - address of counter 
	// returns extensions count --> param loc && array of extentions as pointer to the pointer --> "glfwExtensions"

	//Add Extensions to list of extensions
	for (int i = 0; i < glfwExtensionCnt; i++) {
		instanceExtensions.push_back(glfwExtensions[i]);
	}


	//checking instance extensions are suppoerted
	if (!checkInstanceExtensionSupport(&instanceExtensions)) {
		throw std::runtime_error("VkInstance does not support required extensions");
	}
//========================END QUERING EXTENSIONS=======================


	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	//TODO: set up instance layer that vulkan instance will use
	//TODO: creating validation layers
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;



	// creating instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS) {													//checking if the creation of instance is success	
		throw std::runtime_error("failed to create a vulkan instance");
	}



}

void vulkanRenderer::createLogicalDevice(){


	//NOTE:: QUEUE is BEING CREATED HERE
	

	//getting the queue family indices for the chosen physical device
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	//queue the logical device needs to create and info to do so (only 1 req right now)
	// this queue family is implicit to the 'this' particular referenced locical device that means it is specifically used to pass to the logical device creation therefore not used anywhere else hence need not to have its own moudlar create function

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;				//index of the family to create the queue from
	queueCreateInfo.queueCount = 1;			//no. of queues to create
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority; //vulkan needs to know how to handel multiple queues according to their precedence using priority


	//info to create Logical device(often just called 'device')
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;						//no. of queue create info
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;			// list of create infos so device can create required Queues
	deviceCreateInfo.enabledExtensionCount = 0;						//THESE extensions are different from the Instance extensions that are used by glfw Vulkan but here are the extensions that are required by the logical device to help configuring physical device
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;				//at this point we dont need any 'LIST' of logical device extensions
	//asa of vulkan 1.1 all the validation layers are handeled by Instance

	//creating the feature for device struct  prior to assigning it to the initializing Enables features for creating the logical device

	VkPhysicalDeviceFeatures deviceFeatures = {};					//TODO:we ar'nt using any features at this point og time

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;			//these are the thing like feature that are geometry shaders,tessellation shaders & etc...
																	//this value taked in the the struct type of 'VkPhysicalDeviceFeatures ' hence we need to create a struct prior to initializing pEnabledFeatures
	//by setting the physical device features true in deviceFeatures struct they will be accessible by the logical device

	//creating logical device for given physical device using information provided in deviceCreateInfo
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr/*<custom allocator*/,&mainDevice.logicalDevice);
	//VkResults stores the value for the testing purpose somehwhat similat to the boolean case but returns various err output depending upon the return stratement of the "VkCreateDevice()"

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a LOgical Device!");
	}

	//Queues are created at the same time as the device
	//WE want handel to QUEUES to add FEATURES in FUTURE
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	//from the given logical device ,of given queue family,of given queue index(0 ince only one in the Queue),place reference in VkQueue
	
}

void vulkanRenderer::getPhysicalDevice(){
	//enumerate the no. of physical device that vk instance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);

	//if no devices are available or if none supporet vulkan
	if (deviceCount == 0) {
		throw std::runtime_error("can find GPU that supports vulkan instance");
	}

	//getting list of physical devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	//
	for (const auto& device : deviceList) {
		if (checkDeviceSuitable(device)) {
			mainDevice.physicalDevice = device;
			break;
		}
	}
}





bool vulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtension) {

	//firset we get the no. of extensions to create a array to hold extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);//first param asks for the layer from the extensions need to be checked from



	//creating the list of the VkExtensionProperties [TYPE] using the count we got to store the extension properties of vulkan
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());//it will now populate the 'extensions' list


	//checking if extensions are available in list
	for (const auto& checkExtension : *checkExtension) //this iteration allows to store the reference of the pointer value passed 
	{
		bool hasExtensions = false;
		for (const auto& extension : extensions) {
			if (strcmp(checkExtension, extension.extensionName)) {
				hasExtensions = true;
				break;
			}
		}
		if (!hasExtensions)
			return false;

	}
	return true;
}

bool vulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	* require at later stage
	 
	
	//information ablot the device itself(ID,name,type,vendor,etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(mainDevice.physicalDevice,&deviceProperties);
	
	//Information about what the device can do (geo shader,tess shader,wide lines,etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(mainDevice.physicalDevice, &deviceFeatures);
	
	*/

	/*QUEUE FAMLIES:-->
	there are different types of queue famlies->
	1. graphic queue family
	2. transfer queue family
	3. Presentation queue family    [have a fmily of its own]
	
	
	USE:-
	    Queues are used to execute commands that we submit a series of commands called a 'COMMAND BUFFER' to a queue
		then from that queue one by one those commands are executed
		They are classified in such family according to what their functionality is and such similar commands they holds within them to execute
	*/

	//check what queues our physical device can handel
	//create a struct to contain info about what queues our physical device can handel
	QueueFamilyIndices indices = getQueueFamilies(device);

	return indices.isValid();
}

QueueFamilyIndices vulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	//getting all the queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	//go through each queue family and check if it has at least 1 of the required types of queue
	int i = 0;
	for (const auto& queueFamily : queueFamilyList) {
		//first check if queue family has at least one queue in 
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)	//flags are usually bit fields 'VK_QUEUE_GRAPHICS_BIT' has got the value of 1
			//a queue family supports multiple type of queue (i.e. compute and transfer bit) therefore this bit  is here to check if that particular function is provided by the queue family
		{
			indices.graphicsFamily = i;		//if queue family is valid, then get index
		}
		//check if queue family indices are valid stop searching and dont waste time
		if (indices.isValid()) break;
		i++;
	}

	return indices;
}
