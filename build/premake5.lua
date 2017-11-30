function getAbsoluteFromSolution(p)
	local sol = solution() 
	return path.getabsolute(path.join(sol.basedir, p))
end

stargetsuffix = ""

workspace "2drp"
	configurations { "Debug", "Release" }
	platforms { "native", "x32", "x64" }
	symbols "On"

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }	

	filter "configurations:Debug"
		targetdir ( "Build/%{_ACTION}/bin/Debug" )
	 	defines { "DEBUG" }
		stargetsuffix = "_d"
		targetsuffix "_d"
		editandcontinue "Off"
		optimize "Off"

	filter "configurations:Release"
		targetdir ( "Build/%{_ACTION}/bin/Release" )
		defines { "NDEBUG" }
		optimize "On"
		flags { "LinkTimeOptimization" }

	-- C++17 support on GCC.
	filter { "language:C++", "toolset:gcc" }
		buildoptions { "-std=c++17" }

	-- Windows defines.
	filter "system:windows"
		defines { "WIN32", "_WIN32" }
	filter { "system:windows", "platforms:x64" }
		defines { "WIN64", "_WIN64" }

project "2drp"
	kind "ConsoleApp"
	-- kind "WindowedApp"
	language "C++"
	location "projects"
	local stargetdir = targetdir "../bin"

	vectorextensions "SSE2"

	files { "../src/**" }
	removefiles { "../src/server/**" }
	includedirs { "../src" }

	-- Libraries.
	links {
		"bgfx" .. stargetsuffix,
		"Box2D" .. stargetsuffix,
		"bzip2" .. stargetsuffix,
		"zlib" .. stargetsuffix,
		"efnet" .. stargetsuffix,
	}

	-- Library includes.
	includedirs {
		"../dependencies/bgfx/include/",
		"../dependencies/box2d/Box2D/",
		"../dependencies/bzip2/",
		"../dependencies/zlib/",
		"../dependencies/enet/include/",
		"../dependencies/mathfu/include/",
	}

	-- Awesomium
	-- includedirs { os.getenv("AWE_DIR") .. "include" }
	-- libdirs { os.getenv("AWE_DIR") .. "build/lib" }
	-- links { "awesomium" }
	-- filter { "system:windows" }
	-- 	postbuildcommands { "copy \"" .. os.getenv("AWE_DIR") .. "build\\bin\\*\" \"" .. getAbsoluteFromSolution(stargetdir) .. "\\\"" }
	-- filter { "system:not windows" }
	-- 	postbuildcommands { "cp \"" .. os.getenv("AWE_DIR") .. "build/bin/*\" \"" .. getAbsoluteFromSolution(stargetdir) .. "/\"" }

	-- Per-platform libraries.
	-- filter "system:windows"
	-- 	links { "ws2_32" }
	-- filter { "system:linux or system:macosx or system:bsd or system:solaris" }
	-- 	links { "GL", "Xxf86vm", "boost_filesystem", "boost_thread" }

	-- Library directories.
	-- filter { "system:windows", "platforms:native" }
	-- 	libdirs { "../dependencies/openal/lib/win32" }
	-- 	libdirs { "../dependencies/freetype2/lib/win32" }
	-- filter { "system:windows", "platforms:x32" }
	-- 	libdirs { "../dependencies/openal/lib/win32" }
	-- 	libdirs { "../dependencies/freetype2/lib/win32" }
	-- if not _OPTIONS["no-64bit"] then
	-- filter { "system:windows", "platforms:x64" }
	-- 	libdirs { "../dependencies/openal/lib/win64" }
	-- 	libdirs { "../dependencies/freetype2/lib/win64" }
	-- end

project "2drp_server"
	kind "ConsoleApp"
	language "C++"
	location "projects"
	targetdir "../bin_server"

	vectorextensions "SSE2"

	files { "../src/**" }
	removefiles { "../src/client/**" }
	includedirs { "../src" }

	-- Libraries.
	links {
		"Box2D" .. stargetsuffix,
		"bzip2" .. stargetsuffix,
		"zlib" .. stargetsuffix,
		"efnet" .. stargetsuffix,
	}

	-- Library includes.
	includedirs {
		"../dependencies/box2d/Box2D/",
		"../dependencies/bzip2/",
		"../dependencies/zlib/",
		"../dependencies/enet/include/",
		"../dependencies/mathfu/include/",
	}

	-- Per-platform libraries.
	-- filter "system:windows"
	-- 	links { "ws2_32" }
	-- filter { "system:linux or system:bsd or system:solaris" }
	-- 	links { "boost_thread", "boost_filesystem" }

project "bgfx"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/bgfx/include/**" }
	files { "../dependencies/bgfx/src/**.h", "../dependencies/bgfx/src/**.cpp", "../dependencies/bgfx/src/**.mm" }
	removefiles { "../dependencies/bgfx/src/**.bin.h" }
	removefiles { "../dependencies/bgfx/src/amalgamated.**" }
	includedirs { "../dependencies/bgfx/include/" }
	includedirs { "../dependencies/bgfx/3rdparty/" }
	includedirs { "../dependencies/bgfx/3rdparty/dxsdk/include/" }
	includedirs { "../dependencies/bgfx/3rdparty/khronos/" }
	includedirs { "../dependencies/bimg/include/" }
	includedirs { "../dependencies/bx/include/" }
	dependson { "bx", "bimg", "bimg_decode" }
	links {
		"gdi32",
		"psapi",
		"bimg",
		"bimg_decode",
		"bx",
	}
	defines {
		"__STDC_LIMIT_MACROS",
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
	}
	-- defines { "ENTRY_CONFIG_USE_SDL=1" }
	-- links   { "SDL2" }
	filter "toolset:msc*"
		includedirs { "../dependencies/bx/include/compat/msvc" }
	filter "Debug"
		defines { "BGFX_CONFIG_DEBUG=1" }

project "bimg"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/bimg/include/**", "../dependencies/bimg/src/image.**" }
	includedirs { "../dependencies/bimg/include/" }
	includedirs { "../dependencies/bx/include/" }
	dependson { "bx" }
	links { "bx" }
	defines {
		"__STDC_LIMIT_MACROS",
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
	}
	filter "toolset:msc*"
		includedirs { "../dependencies/bx/include/compat/msvc" }

project "bimg_decode"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/bimg/include/**", "../dependencies/bimg/src/image_decode.**" }
	includedirs { "../dependencies/bimg/include/" }
	includedirs { "../dependencies/bx/include/" }
	includedirs { "../dependencies/bimg/3rdparty/" }
	includedirs { "../dependencies/bimg/3rdparty/nvtt/" }
	includedirs { "../dependencies/bimg/3rdparty/iqa/include/" }
	dependson { "bx" }
	links { "bx" }
	defines {
		"__STDC_LIMIT_MACROS",
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
	}
	filter "toolset:msc*"
		includedirs { "../dependencies/bx/include/compat/msvc" }

project "bx"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/bx/include/**.h", "../dependencies/bx/include/**.inl", "../dependencies/bx/src/**.cpp" }
	removefiles { "../dependencies/bx/src/amalgamated.**" }
	includedirs { "../dependencies/bx/include/" }
	includedirs { "../dependencies/bx/3rdparty/" }
	links { "psapi" }
	defines {
		"__STDC_LIMIT_MACROS",
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
		"_HAS_EXCEPTIONS=0",
		"_HAS_ITERATOR_DEBUGGING=0",
		"_SCL_SECURE=0",
		"_SECURE_SCL=0",
		"_SCL_SECURE_NO_WARNINGS",
		"_CRT_SECURE_NO_WARNINGS",
		"_CRT_SECURE_NO_DEPRECATE",
	}
	filter "toolset:msc*"
		includedirs { "../dependencies/bx/include/compat/msvc" }
	filter "Debug"
		defines { "BX_CONFIG_DEBUG=1" }

project "Box2D"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/box2d/Box2D/Box2D/**.h", "../dependencies/box2d/Box2D/Box2D/**.cpp" }
	includedirs { "../dependencies/box2d/Box2D/" }

project "bzip2"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/bzip2/**.h", "../dependencies/bzip2/**.c" }
	includedirs { "../dependencies/bzip2/" }

project "zlib"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/zlib/**.h", "../dependencies/zlib/**.c" }
	includedirs { "../dependencies/zlib/" }

project "enet"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/enet/**.h", "../dependencies/enet/**.c" }
	includedirs { "../dependencies/enet/include/" }

-- project "mathfu"
-- 	kind "None"
-- 	files { "../dependencies/mathfu/include/**" }
