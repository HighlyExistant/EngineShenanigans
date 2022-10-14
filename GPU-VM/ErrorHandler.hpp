#pragma once
#include "cowdef.hpp"
#include <stdexcept>
#include <iostream>
#ifndef COW_ERR_MSG_DYN
#define COW_ERR_MSG_DYN(err) "Messege Recieved: " #err "\n"
#endif
namespace cow 
{
#ifndef COWCHECKVKRESULTFUNC
#define COWCHECKVKRESULTFUNC
	void checkVkResult(VkResult result)
	{
		switch (result)
		{
		case VK_SUCCESS:
			std::cout << COW_ERR_MSG_DYN(VK_SUCCESS);
			break;
		case VK_NOT_READY:
			throw std::runtime_error(COW_ERR_MSG_DYN(VK_NOT_READY));
			break;
		case VK_TIMEOUT:
			throw std::runtime_error(COW_ERR_MSG_DYN(VK_TIMEOUT));
			break;
		case VK_EVENT_SET:
			std::cout << COW_ERR_MSG_DYN(VK_EVENT_SET);
			break;
		case VK_EVENT_RESET:
			std::cout << COW_ERR_MSG_DYN(VK_EVENT_RESET);
			break;
		case VK_INCOMPLETE:
			std::cout << COW_ERR_MSG_DYN(VK_INCOMPLETE);
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			break;
		case VK_ERROR_DEVICE_LOST:
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			break;
		case VK_ERROR_UNKNOWN:
			break;
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			break;
		case VK_ERROR_FRAGMENTATION:
			break;
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			break;
		case VK_PIPELINE_COMPILE_REQUIRED:
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			break;
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			break;
		case VK_ERROR_INVALID_SHADER_NV:
			break;
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			break;
		case VK_ERROR_NOT_PERMITTED_KHR:
			break;
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			break;
		case VK_THREAD_IDLE_KHR:
			break;
		case VK_THREAD_DONE_KHR:
			break;
		case VK_OPERATION_DEFERRED_KHR:
			break;
		case VK_OPERATION_NOT_DEFERRED_KHR:
			break;
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			break;
		case VK_RESULT_MAX_ENUM:
			break;
		default:
			break;
		}
	}
#endif 
}
