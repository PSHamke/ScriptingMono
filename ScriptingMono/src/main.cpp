#include "Core.h"
#include "Log.h"
#include "Scripting/ScriptingEngine.h"
void main() {
	Log::Init();
	ScriptingMono::ScriptingEngine::Init();
	ScriptingMono::ScriptingEngine::Shutdown();
}