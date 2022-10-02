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
			return vkMapMemory(m_ref_device.getDevice(), memory, offset, size,  0, &mapped);
		}
		void unmap();
		void* write(void *data, VkDeviceSize size, VkDeviceSize offset);
		void copy(VkBuffer src, VkDeviceSize size);
		inline VkDeviceSize size() { return m_size; }
		inline VkBuffer get() { return buffer; }
	private:
		void createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		VkDeviceSize m_size;
		VkDeviceMemory memory;
		VkBuffer buffer;
		void* mapped;

		Device& m_ref_device;
	};
	void Buffer::createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = m_size;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(m_ref_device.getDevice(), &createInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_ref_device.getDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_ref_device.getPhysicalDevice(), &memProperties);
		uint32_t i = 0;
		for (; i < memProperties.memoryTypeCount; i++) {
			if ((memRequirements.memoryTypeBits & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				break; // i has memory type index
			}
		}

		allocInfo.memoryTypeIndex = i;

		if (vkAllocateMemory(m_ref_device.getDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}
		vkBindBufferMemory(m_ref_device.getDevice(), buffer, memory, 0);
	}

	Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
		: m_size{ size },
		m_ref_device{ device }
	{
		createBuffer(usage, properties);
	}
	void Buffer::copy(VkBuffer src, VkDeviceSize size)
	{
		// Begin
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_ref_device.getCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_ref_device.getDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		// Copy
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, src, this->buffer, 1, &copyRegion);
		// End
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_ref_device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_ref_device.getGraphicsQueue());

		vkFreeCommandBuffers(m_ref_device.getDevice(), m_ref_device.getCommandPool(), 1, &commandBuffer);
	}

	Buffer::~Buffer()
	{
		vkFreeMemory(m_ref_device.getDevice(), memory, nullptr);
		vkDestroyBuffer(m_ref_device.getDevice(), buffer, nullptr);
	}
	void Buffer::unmap()
	{
		if (mapped) {
			vkUnmapMemory(m_ref_device.getDevice(), memory);
			mapped = nullptr;
		}
	}
	void* Buffer::write(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		if (size == VK_WHOLE_SIZE)
			return memcpy(mapped, data, m_size);
		return memcpy((&mapped)[offset], data, size);
	}
}
