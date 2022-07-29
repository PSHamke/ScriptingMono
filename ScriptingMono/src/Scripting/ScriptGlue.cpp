#include "ScriptGlue.h"
#include "Log.h"
namespace ScriptingMono {

#define SM_ADD_INTERNAL_CALL(Name) mono_add_internal_call("ScriptingMono.InternalCalls::"#Name, (void*)InternalCalls::Name)
	
	namespace InternalCalls {
#pragma region DemoFunctions
		static void CppFunc() {
			RS_CORE_INFO("This function belongs to C++!");
		}

		static void NativeLog(MonoString* string, int value) {
			char* cStr = mono_string_to_utf8(string);
			std::string str(cStr);
			RS_CORE_INFO("{0} - {1}", str, value);
			mono_free(cStr);
		}
		struct Vector3 {
			float m_X, m_Y, m_Z;
			Vector3(float x, float y, float z) : m_X(x), m_Y(y), m_Z(z) {}
		};
		static void NativeLog_Vector(Vector3* vec) {

			RS_CORE_INFO("X = {0}, Y = {1}, Z = {2}", vec->m_X, vec->m_Y, vec->m_Z);
		}
		// We are actually using given parameter pointer as a return 
		// So its actually changes *vec* parameter, we might not wanted to do it every time 

		static Vector3* Vector3_Scale(Vector3* vec) {
			vec->m_X *= 2;
			vec->m_Y *= 2;
			vec->m_Z *= 2;
			return vec;
		}
		static void Vector3_Transformation(Vector3* vec1, Vector3* vec2, Vector3* result) {
			Vector3 resultVec{ vec1->m_X + vec2->m_X,vec1->m_Y + vec2->m_Y,vec1->m_Z + vec2->m_Z };
			*result = resultVec; // Copying overhead ?
		}
#pragma endregion DemoFunctions

	}
	void ScriptGlue::RegisterFunctions()
	{
		SM_ADD_INTERNAL_CALL(CppFunc);
		SM_ADD_INTERNAL_CALL(NativeLog);
		SM_ADD_INTERNAL_CALL(NativeLog_Vector);
		SM_ADD_INTERNAL_CALL(Vector3_Scale);
		SM_ADD_INTERNAL_CALL(Vector3_Transformation);
	}

}

