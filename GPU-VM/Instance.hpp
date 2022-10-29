#pragma once
#include "cowdef.hpp"
#include <tuple>
#include <stdexcept>
#include <iostream>
namespace cow
{
	class Instance
	{
	public:

		// no copy or move constructors
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;
		Instance(Instance&&) = delete;
		Instance& operator=(Instance&&) = delete;

		Instance();
		~Instance();

		/// <summary>
		/// Wrapper around the vkEnumeratePhysicalDevices function from the vulkan.h header
		/// </summary>
		/// <returns>
		/// uint32_t: physical devices found
		/// VkPhysicalDevice*: pointer to VkPhysicalDevice must be freed using free() 
		/// </returns>
		std::tuple<uint32_t, COW_FREE_RETURN VkPhysicalDevice*> enumeratePhysicalDevices() const;

		/// <summary>
		/// Wrapper around the vkEnumeratePhysicalDeviceGroups function from the vulkan.h header
		/// </summary>
		/// <returns>
		/// uint32_t: physical device groups found
		/// VkPhysicalDevice*: pointer to VkPhysicalDeviceGroupProperties must be freed using free() 
		/// </returns>
		std::tuple<uint32_t, COW_FREE_RETURN VkPhysicalDeviceGroupProperties*> enumeratePhysicalDeviceGroups() const;

		/// <summary>
		/// Wrapper around the vkEnumerateExtensionProperties function from the vulkan.h header
		/// </summary>
		/// <returns>
		/// uint32_t: extensions found
		/// VkPhysicalDevice*: pointer to VkExtensionProperties must be freed using free() 
		/// </returns>
		static std::tuple<uint32_t, COW_FREE_RETURN VkExtensionProperties*> enumerateExtensionProperties();

		// Getters
		inline VkInstance get() const noexcept { return instance; }
		VkInstance instance;

	private:
	};
}
