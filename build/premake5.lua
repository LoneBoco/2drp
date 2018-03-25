function getAbsoluteFromSolution(p)
	local sol = solution() 
	return path.getabsolute(path.join(sol.basedir, p))
end

workspace "2drp"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	symbols "On"
	targetdir ( "Build/%{_ACTION}/bin/%{cfg.buildcfg}" )
	libdirs { "Build/%{_ACTION}/bin/%{cfg.buildcfg}" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }	

	-- C++17 support
	cppdialect "C++17"

	filter "configurations:Debug"
	 	defines { "DEBUG" }
		editandcontinue "Off"
		optimize "Off"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "LinkTimeOptimization" }

	-- Set our platform architectures.
	filter "platforms:x32"
		architecture "x32"
	filter "platforms:x64"
		architecture "x64"

	-- C++17 support.
	filter { "language:C++", "toolset:clang*" }
		links { "c++experimental", "stdc++fs" }

	-- Windows defines.
	filter "system:windows"
		defines { "WIN32", "_WIN32" }
	filter { "system:windows", "platforms:x64" }
		defines { "WIN64", "_WIN64" }

project "2drp"
	-- kind "ConsoleApp"
	kind "WindowedApp"
	language "C++"
	location "projects"
	targetdir "../bin"
	debugdir "../bin"

	vectorextensions "SSE2"

	files { "../src/**" }
	removefiles { "../src/server/**" }
	removefiles { "../src/engine/filesystem/watch/FileWatchOS_*" }
	includedirs { "../src" }

	-- Libraries.
	links {
		"bgfx",
		"Box2D",
		"bzip2",
		"zlib",
		"enet",
		"pugixml",
		"SDL2", -- External.
		"SDL2main", -- External.
	}

	-- Library includes.
	includedirs {
		"../dependencies/bgfx/include/",
		"../dependencies/box2d/Box2D/",
		"../dependencies/bzip2/",
		"../dependencies/zlib/",
		"../dependencies/enet/include/",
		"../dependencies/mathfu/include/",
		"../dependencies/mathfu/dependencies/vectorial/include/",
		"../dependencies/sdl/include/",
		"../dependencies/BabyDI/include/",
		"../dependencies/pugixml/src/",
	}

	dependson { "bgfx", "box2d", "bzip2", "zlib", "enet" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

	-- Post build commands.
	filter {}
		postbuildcommands { "{COPY} %{wks.location}/../doc/settings.ini %{cfg.targetdir}" }
		postbuildcommands { "{COPY} %{wks.location}/../media/packages/login/ %{cfg.targetdir}/packages/login/"}
	filter { "system:windows", "platforms:x32" }
		postbuildcommands { "{COPY} %{wks.location}/../dependencies/sdl/lib/x86/SDL2.dll %{cfg.targetdir}" }
	filter { "system:windows", "platforms:x64" }
		postbuildcommands { "{COPY} %{wks.location}/../dependencies/sdl/lib/x64/SDL2.dll %{cfg.targetdir}" }

	-- SDL
	filter { "system:windows", "platforms:x32" }
		libdirs "../dependencies/sdl/lib/x86/"
	filter { "system:windows", "platforms:x64" }
		libdirs "../dependencies/sdl/lib/x64/"

	-- Handle file watch properly.
	filter { "system:windows" }
		files { "../src/engine/filesystem/watch/FileWatchOS_Windows*" }
	filter { "system:linux" }
		files { "../src/engine/filesystem/watch/FileWatchOS_Linux*" }

	-- Awesomium
	-- includedirs { os.getenv("AWE_DIR") .. "include" }
	-- libdirs { os.getenv("AWE_DIR") .. "build/lib" }
	-- links { "awesomium" }
	-- filter { "system:windows" }
	-- 	postbuildcommands { "copy \"" .. os.getenv("AWE_DIR") .. "build\\bin\\*\" \"" .. getAbsoluteFromSolution(stargetdir) .. "\\\"" }
	-- filter { "system:not windows" }
	-- 	postbuildcommands { "cp \"" .. os.getenv("AWE_DIR") .. "build/bin/*\" \"" .. getAbsoluteFromSolution(stargetdir) .. "/\"" }

	-- Per-platform libraries.
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		links { "pthread" }

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
	removefiles { "../src/engine/filesystem/watch/FileWatchOS_*" }
	includedirs { "../src" }

	-- Libraries.
	links {
		"Box2D",
		"bzip2",
		"zlib",
		"enet",
		"pugixml",
	}

	-- Library includes.
	includedirs {
		"../dependencies/box2d/Box2D/",
		"../dependencies/bzip2/",
		"../dependencies/zlib/",
		"../dependencies/enet/include/",
		"../dependencies/mathfu/include/",
		"../dependencies/mathfu/dependencies/vectorial/include/",
		"../dependencies/pugixml/src/",
	}

	dependson { "box2d", "bzip2", "zlib", "enet" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

	-- Handle file watch properly.
	filter { "system:windows" }
		files { "../src/engine/filesystem/watch/FileWatchOS_Windows*" }
	filter { "system:linux" }
		files { "../src/engine/filesystem/watch/FileWatchOS_Linux*" }

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

project "box2d"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/box2d/Box2D/Box2D/**.h", "../dependencies/box2d/Box2D/Box2D/**.cpp" }
	includedirs { "../dependencies/box2d/Box2D/" }

project "bzip2"
	kind "StaticLib"
	language "C"
	location "projects"
	files { "../dependencies/bzip2/**.h", "../dependencies/bzip2/**.c" }
	includedirs { "../dependencies/bzip2/" }
	removefiles {
		"../dependencies/bzip2/bzip2.c",
		"../dependencies/bzip2/bzip2recover.c",
		"../dependencies/bzip2/dlltest.c",
		"../dependencies/bzip2/mk251.c",
		"../dependencies/bzip2/spewG.c",
		"../dependencies/bzip2/unzcrash.c"
	}

project "zlib"
	kind "StaticLib"
	language "C"
	location "projects"
	files { "../dependencies/zlib/*.h", "../dependencies/zlib/*.c" }
	includedirs { "../dependencies/zlib/" }

project "enet"
	kind "StaticLib"
	language "C"
	location "projects"
	files { "../dependencies/enet/**.h", "../dependencies/enet/**.c" }
	includedirs { "../dependencies/enet/include/" }

	filter "system:linux"
		defines { "HAS_SOCKLEN_T" }

-- project "mathfu"
-- 	kind "None"
-- 	files { "../dependencies/mathfu/include/**" }

project "pugixml"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/pugixml/src/**" }
	flags { "NoPCH", "NoMinimalRebuild" }
	editandcontinue "Off"
	uuid "89A1E353-E2DC-495C-B403-742BE206ACED"
