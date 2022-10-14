#pragma once
#include "Device.hpp"
namespace cow 
{
	class Buffer
	{
	public:
		Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		~Buffer();
		inline VkResult map(
			VkDeviceSize size = VK_WHOLE_SIZE, 
			VkDeviceSize offset = 0
		) 
		{ 
			VkResult result;
			result = vkMapMemory(m_ref_device.getDevice(), memory, offset, size,  0, &mapped);
			return result;
		}	
		void unmap();
		void* write(void *data, VkDeviceSize size, VkDeviceSize offset);
		void copy(VkBuffer src, VkDeviceSize size);
		inline VkDeviceSize size() { return m_size; }
		inline VkBuffer get() { return buffer; }
		void* mapped;
	private:
		void createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		VkDeviceSize m_size;
		VkDeviceMemory memory;
		VkBuffer buffer;

		Device& m_ref_device;
	};
	
}
