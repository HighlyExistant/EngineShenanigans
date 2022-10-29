#pragma once
#include "Buffers.hpp"
namespace cow 
{ 
	class Texture
	{
	public:
		Texture(Device &device, const char* filepath);
		~Texture();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void createTextureImageView();
		void createTextureSampler();
		
		void fillImageInfo(VkDescriptorImageInfo* pInfo, VkImageLayout layout);
		
		VkImage m_textureImage;
		VkImageView m_imageView;
		VkSampler m_textureSampler;
	private:

		VkDeviceMemory m_textureImageMemory;

		Device& m_ref_device;
	};

	
}
