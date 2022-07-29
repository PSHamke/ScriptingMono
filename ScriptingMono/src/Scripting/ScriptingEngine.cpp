#include "ScriptingEngine.h"
#include "Log.h"
#include "Core.h"
#include <fstream>

#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"


namespace ScriptingMono {

	namespace Utils {

		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize) {
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);
			if (!stream) {
				return nullptr;
			}
			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				return nullptr;
			}
			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();
			*outSize = (uint32_t) size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath) {
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);
			if (status != MONO_IMAGE_OK) {
				const char* errorMessage = mono_image_strerror(status);
				// Log
				RS_CORE_FATAL("ASM READ ERROR =  {0}", errorMessage);
				return nullptr;
			}

			std::string path = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
			mono_image_close(image);

			delete[] fileData;

			return assembly;

		}

		void PrintAssemblyTypes(MonoAssembly* assembly) {
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				RS_CORE_TRACE("=> {0}.{1}", nameSpace, name);
			}
		}
	}
	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* AssemblyImage = nullptr;

		ScriptClass MainClass;
	};
	
	static ScriptEngineData* s_Data = nullptr;
	
	

	void ScriptingEngine::Init()
	{
		s_Data = new ScriptEngineData();
		InitMono();
		LoadAssembly("Resources/Scripts/Test-ScriptingCore.dll");
		s_Data->AssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		RuntimeDemo();
	}

	void ScriptingEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}

	void ScriptingEngine::InitMono()
	{
		mono_set_assemblies_path("/mono/lib");
		MonoDomain* rootDomain = mono_jit_init("TestJITRuntime");
		RS_CORE_ASSERT(rootDomain);
		// Store the root domain pointer
		s_Data->RootDomain = rootDomain;
	}

	void ScriptingEngine::ShutdownMono()
	{
		// Unloading issues
		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;
		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;

	}

	void ScriptingEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_Data->AppDomain = mono_domain_create_appdomain("TestScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);
		// Load DLL Assembly
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
	}

	
	void ScriptingEngine::RuntimeDemo() {

		ScriptGlue::RegisterFunctions();

		if (s_Data->CoreAssembly)
			Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
		else
			RS_CORE_TRACE("Cannot read asm");

		// Object Creation .. Calling constructor
		s_Data->MainClass = ScriptClass("ScriptingMono", "Main");
		MonoObject* instance = s_Data->MainClass.Instantiate();

		// Calling Method
		MonoMethod* printMessageFunc = s_Data->MainClass.GetMethod("PrintMessage",0);
		s_Data->MainClass.InvokeMethod(printMessageFunc);


		// Calling Method with param
		// 1 parameter 
		
		MonoMethod* printIntFunc = s_Data->MainClass.GetMethod("PrintInt", 1);

		int value = 7;
		void* param = &value;
		s_Data->MainClass.InvokeMethod(printIntFunc,&param);

		// Multiple Parameters
		
		MonoMethod* printIntsFunc = s_Data->MainClass.GetMethod("PrintInts", 2);
		int value1 = 2;
		int value2 = 5;
		void* params[2] = {
			&value1, &value2
		};

		
		s_Data->MainClass.InvokeMethod(printIntsFunc,params);

		// String as a parameter
		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World From C++!");
		MonoMethod* printCustomMessageFunc = s_Data->MainClass.GetMethod("PrintCustomMessage", 1);
		void* stringParam = monoString;
		s_Data->MainClass.InvokeMethod(printCustomMessageFunc, &stringParam);
	}
	
	MonoObject* ScriptingEngine::InstatiateMonoObject(MonoClass* klass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, klass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNameSpace, const std::string className)
		: m_ClassNameSpace(classNameSpace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->AssemblyImage, classNameSpace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		m_MonoInstance = ScriptingEngine::InstatiateMonoObject(m_MonoClass);
		return m_MonoInstance;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int paramCount) {
		return mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), paramCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoMethod* method, void** params, MonoObject** exHandle)
	{
		return mono_runtime_invoke(method,m_MonoInstance, params, exHandle);
	}


}