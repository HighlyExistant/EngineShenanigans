#pragma once
#include "Device.hpp"

namespace cow 
{
	struct Image2DCreation
	{
		uint32_t width; 
		uint32_t height; 
		VkFormat format; 
		VkImageTiling tiling;
		VkImageLayout layout;
	};
	class Image2D
	{
	public:
		Image2D(Image2DCreation *pCreateInfo)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = pCreateInfo->width;
			imageInfo.extent.height = pCreateInfo->height;
			imageInfo.extent.depth = 1.0; // depth must be 1 for this is a 2 dimensional image
			imageInfo.mipLevels = 1;
			imageInfo.format = pCreateInfo->format;
			imageInfo.tiling = pCreateInfo->tiling;
			imageInfo.initialLayout = pCreateInfo->layout;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		~Image2D();

	private:
		VkImage* m_pscImage;
		VkImageView* m_pscImageViews;
		uint32_t m_scImageViewCount;
	};

}
