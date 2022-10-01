#pragma once
#include "Device.hpp"
#include "GraphicsCommands.hpp"
#define GLM_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace cow 
{
	class EmptyObject
	{
	public:
		using id_t = uint32_t;
		
		static EmptyObject create() 
		{
			static id_t currentId = 0;
			return EmptyObject(currentId++);
		}
		~EmptyObject();

		EmptyObject(const EmptyObject&) = delete;
		EmptyObject& operator=(const EmptyObject&) = delete;
		EmptyObject(EmptyObject&&) = default;
		EmptyObject& operator=(EmptyObject&&) = default;
	private:
		EmptyObject(id_t id) : id{ id } {}
		id_t id;
	};

	EmptyObject::~EmptyObject()
	{
	}
}
