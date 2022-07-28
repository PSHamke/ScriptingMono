#include "Core.h"
#include "Log.h"
#include "Scripting/ScriptingEngine.h"
void main() {
	Log::Init();
	ScriptingEngine::Init();
	ScriptingEngine::Shutdown();
}