#include "errorlib.hpp"

namespace cow
{
    Logger::Logger() 
    {
        file = fopen("log.txt", "wb+");
    }
    Logger::~Logger() 
    {
        fclose(file);
    }
    void Logger::Log(const char* msg, COW_ERR_TYPE error_type) 
        {
            if (error_type == COW_ERR_TYPE::SUCCESS) 
            {
                fwrite("[success] ", 1, 10, file);
            }
            if (error_type == COW_ERR_TYPE::WARNING)
            {
                fwrite("[warning] ", 1, 10, file);
            }
            if (error_type == COW_ERR_TYPE::FAILURE) 
            {
                fwrite("[failure] ", 1, 10, file);
            }
            if (error_type == COW_ERR_TYPE::FOUND)
            {
                fwrite("[found]   ", 1, 10, file);
            }
            fwrite(msg, 1, strlen(msg), file);
            fwrite("\n", 1, 1, file);
        }
} // namespace cow
