#include "cow_types.hpp"
namespace cow 
{
	VkVertexInputBindingDescription Vertex2D::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc;
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2D);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2D::attributeDesc()
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		// The following values will be loaded into the vertex shader
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2D, position) });
		//r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex2DSRGB, color) });
		return r_attrDescs;
	}
	// -- --
	VkVertexInputBindingDescription Vertex2DRGB::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc;
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2DRGB);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2DRGB::attributeDesc()
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		// The following values will be loaded into the vertex shader
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DRGB, position) });
		r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex2DRGB, color) });
		std::cout << "position: " << offsetof(Vertex2DRGB, position) << '\n';
		std::cout << "color: " << offsetof(Vertex2DRGB, color) << '\n';
		return r_attrDescs;
	}
	// -- --
	VkVertexInputBindingDescription Vertex2DTextured::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc{};
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2DTextured);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2DTextured::attributeDesc()
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DTextured, position) });
		r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DTextured, texture) });
		return r_attrDescs;
	}


	VkVertexInputBindingDescription Vertex2DRGBA::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc{};
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2DRGBA);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2DRGBA::attributeDesc() 
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DRGBA, position) });
		r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex2DRGBA, color) });
		return r_attrDescs;
	}

	VkVertexInputBindingDescription Vertex2DTexturedRGBA::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc{};
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2DTexturedRGBA);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2DTexturedRGBA::attributeDesc()
	{
		std::cout << sizeof(Vertex2DTexturedRGBA);
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DTexturedRGBA, position) });
		r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DTexturedRGBA, texture) });
		r_attrDescs.push_back({ 2,0,VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex2DTexturedRGBA, color) });
		return r_attrDescs;
	}

	void Vertex2D::Rect(float width, float height, Vertex2D buf[4])
	{
		/*
		* 0.0 0.0			* width 0.0
		
		* 0.0 height		* width height
		*/
		buf[0].position = { 0.0, 0.0 };
		buf[1].position = { width, 0.0 };
		buf[2].position = { 0.0, height };
		buf[3].position = { width, height };
	}
	void Vertex2DRGB::Rect(float width, float height, Vertex2DRGB buf[4])
	{
		buf[0].position = { 0.0, 0.0 };
		buf[1].position = { width, 0.0 };
		buf[2].position = { 0.0, height };
		buf[3].position = { width, height };
	}
	void Vertex2DTextured::Rect(float width, float height, Vertex2DTextured buf[4])
	{
		buf[0].position = { 0.0, 0.0 };
		buf[1].position = { width, 0.0 };
		buf[2].position = { 0.0, height };
		buf[3].position = { width, height };
	}
	void Vertex2DRGBA::Rect(float width, float height, Vertex2DRGBA buf[4])
	{
		buf[0] = { { width, -1.0	}, {0.0 , 0.0, 0.0, 1.0} };
		buf[1] = { { -1.0, height	}, {0.0 , 0.0, 0.0, 1.0} };
		buf[2] = { { width, height	}, {0.0 , 0.0, 0.0, 1.0} };
		buf[3] = { { -1.0, -1.0		}, {0.0 , 0.0, 0.0, 1.0} };
	}
	void Vertex2DTexturedRGBA::Rect(float width, float height, Vertex2DTexturedRGBA buf[4])
	{

		buf[0] = { { width, -1.0 }, {1.0 , 0.0},{0.0 , 0.0, 0.0, 0.0} };
		buf[1] = { { -1.0, height }, {0.0 , 1.0}, {0.0 , 0.0, 0.0, 0.0} };
		buf[2] = { { width, height}, {1.0 , 1.0}, {0.0 , 0.0, 0.0, 0.0} };
		buf[3] = { { -1.0, -1.0 },  {0.0 , 0.0},{0.0 , 0.0, 0.0, 0.0} };
	}
}
