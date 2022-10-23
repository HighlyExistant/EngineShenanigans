#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#define GLM_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <iostream>

namespace cow
{
	struct Vertex2D
	{
		static void Rect(float width, float height, Vertex2D buf[4]);

		glm::vec2 position;

		static VkVertexInputBindingDescription bindingDesc();
		static std::vector<VkVertexInputAttributeDescription> attributeDesc();
	};
	struct Vertex2DRGB
	{
		static void Rect(float width, float height, Vertex2DRGB buf[4]);
		glm::vec2 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription bindingDesc();
		static std::vector<VkVertexInputAttributeDescription> attributeDesc();
	} ;
	struct Vertex2DTextured
	{
		static void Rect(float width, float height, Vertex2DTextured buf[4]);
		glm::vec2 position;
		glm::vec2 texture;

		static VkVertexInputBindingDescription bindingDesc();
		static std::vector<VkVertexInputAttributeDescription> attributeDesc();
	};
	struct Vertex2DRGBA
	{
		static void Rect(float width, float height, Vertex2DRGBA buf[4]);
		glm::vec2 position;
		glm::vec4 color;

		static VkVertexInputBindingDescription bindingDesc();
		static std::vector<VkVertexInputAttributeDescription> attributeDesc();
	};
	struct Vertex2DTexturedRGBA
	{
		static void Rect(float width, float height, Vertex2DTexturedRGBA buf[4]);
		glm::vec2 position;
		glm::vec2 texture;
		glm::vec4 color;

		static VkVertexInputBindingDescription bindingDesc();
		static std::vector<VkVertexInputAttributeDescription> attributeDesc();
	};

	/*
	* Example found:
	* https://www.toptal.com/game/video-game-physics-part-ii-collision-detection-for-solid-objects
	*/

	typedef struct AxisAlignedBoundingBox
	{
		glm::vec2 min;
		glm::vec2 max;

		static bool testOverlap(AxisAlignedBoundingBox* a, AxisAlignedBoundingBox* b)
		{
			float d1x = b->min.x - a->max.x;
			float d1y = b->min.y - a->max.y;
			float d2x = a->min.x - b->max.x;
			float d2y = a->min.y - b->max.y;

			if (d1x > 0.0f || d1y > 0.0f)
				return false;

			if (d2x > 0.0f || d2y > 0.0f)
				return false;

			return true;
		}
		static bool testOverlap(AxisAlignedBoundingBox a, AxisAlignedBoundingBox b)
		{
			float d1x = b.min.x - a.max.x;
			float d1y = b.min.y - a.max.y;
			float d2x = a.min.x - b.max.x;
			float d2y = a.min.y - b.max.y;

			if (d1x > 0.0f || d1y > 0.0f)
				return false;

			if (d2x > 0.0f || d2y > 0.0f)
				return false;

			return true;
		}
	} AABB;

	struct Transform2DComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;
		glm::mat2 mat2()
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.f}, {.0f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};
}
