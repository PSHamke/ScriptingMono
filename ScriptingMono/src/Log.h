#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

class Log
{
public:

	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;

};



// Core log macros
#define RS_CORE_WARN(...)        ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RS_CORE_INFO(...)        ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RS_CORE_TRACE(...)       ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RS_CORE_ERROR(...)       ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RS_CORE_FATAL(...)       ::Log::GetCoreLogger()->critical(__VA_ARGS__)

#ifndef RS_DEBUG
#define RS_CORE_WARN(...)
#define RS_CORE_INFO(...)
#define RS_CORE_TRACE(...)
#define RS_CORE_ERROR(...)
#define RS_CORE_FATAL(...)
#endif

#define  RS_ENABLE_ASSERTS

#ifdef RS_ENABLE_ASSERTS
#define RS_CORE_ASSERT(x, ...) {if(!(x)) {RS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else 
#define RS_CORE_ASSERT(x, ...)
#endif