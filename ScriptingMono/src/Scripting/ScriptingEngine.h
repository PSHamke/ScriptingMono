#pragma once
#include <filesystem>
#include <string>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}

namespace ScriptingMono {

	

	class ScriptingEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);
	private:
		static void InitMono();
		static void ShutdownMono();
		static void RuntimeDemo();
		static MonoObject* InstatiateMonoObject(MonoClass* klass);

		friend class ScriptClass;
	};

	class ScriptClass {
	public:
		ScriptClass() = default;

		ScriptClass(const std::string& classNameSpace, const std::string className);
			

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& methodName, int paramCount);
		MonoObject* InvokeMethod(MonoMethod* method, void** params = nullptr, MonoObject** exHandle= nullptr);
	private:
		std::string m_ClassNameSpace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;
		MonoObject* m_MonoInstance = nullptr;
	};
}

