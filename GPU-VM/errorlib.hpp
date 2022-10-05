#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstring>
#include "cowdef.hpp"
#ifndef COW_ERR_MSG_DYN
#define COW_ERR_MSG_DYN(err) "Messege Recieved: " #err "\n"
#endif
namespace cow
{
    enum class COW_ERR_TYPE : uint8_t
    {
        SUCCESS,
        WARNING,
        FAILURE,
		MESSEGE,
		FOUND
    };
    class Logger
    {
    private:
        FILE *file;
    public:
        Logger(/* args */);
        ~Logger();
		void checkVkResult(VkResult result)
		{
			switch (result)
			{
			case VK_SUCCESS:
				Log(COW_ERR_MSG_DYN(VK_SUCCESS), COW_ERR_TYPE::MESSEGE);
				break;
			case VK_NOT_READY:
				Log(COW_ERR_MSG_DYN(VK_NOT_READY), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_NOT_READY));
				break;
			case VK_TIMEOUT:
				Log(COW_ERR_MSG_DYN(VK_TIMEOUT), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_TIMEOUT));
				break;
			case VK_EVENT_SET:
				Log(COW_ERR_MSG_DYN(VK_EVENT_SET), COW_ERR_TYPE::FAILURE);
				std::cout << COW_ERR_MSG_DYN(VK_EVENT_SET);
				break;
			case VK_EVENT_RESET:
				Log(COW_ERR_MSG_DYN(VK_EVENT_RESET), COW_ERR_TYPE::FAILURE);
				std::cout << COW_ERR_MSG_DYN(VK_EVENT_RESET);
				break;
			case VK_INCOMPLETE:
				Log(COW_ERR_MSG_DYN(VK_INCOMPLETE), COW_ERR_TYPE::FAILURE);
				std::cout << COW_ERR_MSG_DYN(VK_INCOMPLETE);
				break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				Log(COW_ERR_MSG_DYN(VK_ERROR_OUT_OF_HOST_MEMORY), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_OUT_OF_HOST_MEMORY));
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				Log(COW_ERR_MSG_DYN(VK_ERROR_OUT_OF_DEVICE_MEMORY), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_OUT_OF_DEVICE_MEMORY));
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				Log(COW_ERR_MSG_DYN(VK_ERROR_INITIALIZATION_FAILED), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_INITIALIZATION_FAILED));
				break;
			case VK_ERROR_DEVICE_LOST:
				Log(COW_ERR_MSG_DYN(VK_ERROR_DEVICE_LOST), COW_ERR_TYPE::FAILURE);
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				Log(COW_ERR_MSG_DYN(VK_ERROR_MEMORY_MAP_FAILED), COW_ERR_TYPE::FAILURE);
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				Log(COW_ERR_MSG_DYN(VK_ERROR_LAYER_NOT_PRESENT), COW_ERR_TYPE::FAILURE);
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				Log(COW_ERR_MSG_DYN(VK_ERROR_EXTENSION_NOT_PRESENT), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_EXTENSION_NOT_PRESENT));
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				Log(COW_ERR_MSG_DYN(VK_ERROR_FEATURE_NOT_PRESENT), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_FEATURE_NOT_PRESENT));
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				Log(COW_ERR_MSG_DYN(VK_ERROR_FORMAT_NOT_SUPPORTED), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_FORMAT_NOT_SUPPORTED));
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				Log(COW_ERR_MSG_DYN(VK_ERROR_FRAGMENTED_POOL), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_FRAGMENTED_POOL));
				break;
			case VK_ERROR_UNKNOWN:
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				Log(COW_ERR_MSG_DYN(VK_ERROR_OUT_OF_POOL_MEMORY), COW_ERR_TYPE::FAILURE);
				throw std::runtime_error(COW_ERR_MSG_DYN(VK_ERROR_OUT_OF_POOL_MEMORY));
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
        void Log(const char* msg, COW_ERR_TYPE error_type);
		inline void LogMultiple(std::string str, COW_ERR_TYPE error_type) 
		{
			Log(str.c_str(), error_type);
		}
    };
} // namespace cow
