workspace "ScriptingMono"
    architecture "x64"
    startproject "ScriptingMono"

    configurations
    {
        "Debug",
        "Release"
    }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    IncludeDir = {}
    IncludeDir["mono"] = "%{wks.location}/ScriptingMono/vendor/mono/include"

    LibraryDir = {}
    LibraryDir["mono"] ="%{wks.location}/ScriptingMono/vendor/mono/lib/%{cfg.buildcfg}"

    Library = {}
    Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

    -- Windows Dependencies
    Library["WinSock"] = "Ws2_32.lib"
    Library["Winmm"] = "Winmm.lib"
    Library["WinVersion"] = "Version.lib"
    Library["Bcrypt"] = "Bcrypt.lib"
    project "ScriptingMono"
        location "ScriptingMono"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "off"
        
        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
        
        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "%{prj.name}/src",
		    "%{prj.name}/vendor/spdlog/include",
            "%{IncludeDir.mono}"
        }
        
        links
        {
            "%{Library.mono}"
        }

        filter "system:windows"
            systemversion "latest"
            defines
            {
            }
            links
            {
                "%{Library.WinSock}",
                "%{Library.Winmm}",
                "%{Library.WinVersion}",
                "%{Library.Bcrypt}"
            }

        filter "configurations:Debug"
		defines "RS_DEBUG"
		runtime "Debug"
		symbols "On"

	    filter "configurations:Release"
		defines "RS_RELEASE"
		runtime "Release"
		optimize "On"


    project "Test-ScriptingCore"
        location "Test-ScriptingCore"
        kind "SharedLib"
        language "C#"
        dotnetframework "4.7.2"
        targetdir ("%{wks.location}/Resources/Scripts")
        objdir ("%{wks.location}/Resources/Scripts-int")

        files 
        {
            "%{prj.name}/Source/**.cs",
            "%{prj.name}/Properties/**.cs"
        }

        filter "configurations:Debug"
            optimize "Off"
            symbols "Default"
        
        filter "configurations:Release"
            optimize "On"
            symbols "Default"
        
        filter "configurations:Dist"
            optimize "Full"
            symbols "Off"