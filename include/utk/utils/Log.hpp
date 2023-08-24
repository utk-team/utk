/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#pragma once

#ifdef UTK_LOG

#include <cstdarg>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace utk
{
    class Logger
    {
    private:
        Logger()
        {
            CreateConsoleLogger();
        }

        void CreateConsoleLogger()
        {
            auto default_sink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();

            spdlog::sinks_init_list sink_list = { default_sink };
            logger = std::make_shared<spdlog::logger>("UTK_LOGGER", sink_list.begin(), sink_list.end());
            logger->set_pattern("[UTK] %l: %v");

            EnableLog();
        }

        void CreateFileLogger(const std::string& file)
        {
            auto default_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(file);

            spdlog::sinks_init_list sink_list = { default_sink };
            logger = std::make_shared<spdlog::logger>("UTK_LOGGER", sink_list.begin(), sink_list.end());
            logger->set_pattern("[UTK] %l: %v");

            EnableLog();
        }
    public:
        Logger(Logger const&)          = delete;
        void operator=(Logger const&)  = delete;
    
    public:
        std::shared_ptr<spdlog::logger> logger; // Public ... Why ? no idea, but why not ?
                                                // Not static just to be sure it is properly constructed 
                                                // but without cpp file :)

        void DisableLog()
        {
            logger->set_level(spdlog::level::off);
        }

        void EnableLog()
        {
            logger->set_level(spdlog::level::trace);
        }

        void SetFileLogging(const std::string& file)
        {
            CreateFileLogger(file);
        }

        void SetConsoleLogging()
        {
            CreateConsoleLogger();
        }
        
        static Logger& instance()
        {
            static Logger log;
            return log;
        }
    };
}
    #ifdef UTK_LOG_NO_DEBUG
        #define UTK_DEBUG(message, ...) ;
    #else
        #define UTK_DEBUG(message, ...) utk::Logger::instance().logger->debug(message, __VA_ARGS__);
    #endif
    #define UTK_LOG_DISABLE()       utk::Logger::instance().DisableLog();
    #define UTK_LOG_ENABLE()        utk::Logger::instance().EnableLog();
    #define UTK_LOG_FILE(file)      utk::Logger::instance().SetFileLogging(file);
    #define UTK_LOG_CONSOLE()       utk::Logger::instance().SetConsoleLogging();
    #define UTK_INFO(message, ...)  utk::Logger::instance().logger->info (message, __VA_ARGS__);
    #define UTK_WARN(message, ...)  utk::Logger::instance().logger->warn (message, __VA_ARGS__);
    #define UTK_ERROR(message, ...) utk::Logger::instance().logger->error(message, __VA_ARGS__);
#else
    #define UTK_LOG_DISABLE()  ;
    #define UTK_LOG_ENABLE()   ;
    #define UTK_LOG_FILE(file) ;
    #define UTK_LOG_CONSOLE()  ;
    #define UTK_DEBUG(message, ...) ;
    #define UTK_INFO(message, ...)  ;
    #define UTK_WARN(message, ...)  ;
    #define UTK_ERROR(message, ...) ;
#endif