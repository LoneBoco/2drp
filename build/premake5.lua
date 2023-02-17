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

	-- C++20 support
	-- C++latest until <ranges> is supported outside of it.
	cppdialect "C++latest"

	linkgroups "On"
	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "DEBUG", "_DEBUG" }
		editandcontinue "Off"
		optimize "Off"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "Size"
		flags { "LinkTimeOptimization" }
		staticruntime "On"

	-- Set our platform architectures.
	filter "platforms:x32"
		architecture "x32"
	filter "platforms:x64"
		architecture "x64"

	-- C++20 support.
	-- filter "toolset:clang*"
	-- 	links { "c++experimental", "stdc++fs" }
	-- 	buildoptions { "-std=c++20", "-Wno-switch" }

	-- Toolset specific
	filter "toolset:msc*"
		defines { "_CRT_SECURE_NO_WARNINGS" }
		disablewarnings {
			"5105",
			"26812",	-- Enum unscoped
		}
		buildoptions {
			"/guard:cf",	-- Control Flow Guard
			"/Qspectre",	-- Spectre Mitigation
			"/Zc:preprocessor",	-- Use alternative preprocessor (like GCC/clang, required for BabyDI)
			--"/Ob3",			-- Even more memory for inline expansion (this causes pugi_xml to crash?)
		}

	-- Windows defines.
	filter "system:windows"
		defines { "WIN32", "_WIN32", "_WINDOWS" }
	filter { "system:windows", "platforms:x64" }
		defines { "WIN64", "_WIN64" }

	-- OS defines
	filter "system:windows"
		defines { "TDRP_WINDOWS" }
	filter "system:linux"
		defines { "TDRP_LINUX" }
	filter "system:macosx"
		defines { "TDRP_MACOSX" }

project "2drp"
	kind "ConsoleApp"
	-- kind "WindowedApp"
	language "C++"
	location "projects"
	targetdir "../bin"
	debugdir "../bin"

	vectorextensions "AVX"
	rtti "On"

	files { "../src/**" }
	removefiles { "../src/server/**" }
	includedirs { "../src" }

	-- Library includes.
	includedirs {
		-- Header only.
		"../dependencies/mathfu/include/",
		"../dependencies/mathfu/dependencies/vectorial/include/",
		"../dependencies/BabyDI/include/",
		"../dependencies/pugixml/src/",

		"../dependencies/bzip2/",
		"../dependencies/zlib/",

		-- Libs.
		"../dependencies/ziplib/Source/ZipLib/",
		"../dependencies/enet/include/",
		"../dependencies/protobuf/src/",
		"../dependencies/SFML/include/",
		"../dependencies/freetype-windows-binaries/include/",
		"../dependencies/PlayRho/",
		"../dependencies/RmlUi/include/",
		"../dependencies/RmlSolLua/include/",
		"../dependencies/sol2/include/",
		"../dependencies/luajit-2.0/src/",
		"../dependencies/SpriterPlusPlus/",
		"../dependencies/tmxlite/tmxlite/include/",
		"../dependencies/Clipper2/CPP/Clipper2Lib/include/",
		"../dependencies/polypartition/src/",
	}

	dependson { "SFML", "PlayRho", "bzip2", "ziplib", "enet", "SpriterPlusPlus", "tmxlite", "RmlUi", "RmlSolLua", "Clipper2", "polypartition" }

	-- Libraries.
	links {
		-- "bzip2",
		-- "zlib",
		"ziplib",
		"enet",
		"protobuf",
		-- "freetype",
		"SFML",
		"PlayRho",
		"RmlUi",
		"RmlSolLua",
		"lua51",
		"SpriterPlusPlus",
		"tmxlite",
		"Clipper2",
		"polypartition",
	}

	defines { "SFML_STATIC", "RMLUI_STATIC_LIB", "NOMINMAX", "PUGIXML_HEADER_ONLY" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

	-- Debug name.
	filter "configurations:Debug"
		targetname "2drp_d"

	-- Disable MSVC warnings because 3rd party libraries never update.
	filter "toolset:msc*"
		defines { "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING", "_SILENCE_CXX20_IS_POD_DEPRECATION_WARNING" }

	-- Per-platform libraries.
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		links { "pthread", "dl" }
	filter { "system:linux" }
		links { "X11", "GL" }

	-- Freetype
	filter { "system:windows", "platforms:x32" }
		libdirs { "../dependencies/freetype-windows-binaries/release dll/win32/" }
	filter { "system:windows", "platforms:x64" }
		libdirs { "../dependencies/freetype-windows-binaries/release dll/win64/" }

	-- Per-platform file cleanup.
	filter { "system:windows" }
		removefiles { "../src/client/ui/interface/macosx/**", "../src/client/ui/interface/x11/**" }
	filter { "system:linux or system:bsd or system:solaris" }
		removefiles { "../src/client/ui/interface/macosx/**", "../src/client/ui/interface/win32/**" }
	filter { "system:macosx" }
		removefiles { "../src/client/ui/interface/win32/**", "../src/client/ui/interface/x11/**" }

	-- Post-build commands.
	filter {}
		postbuildcommands { "{COPY} \"%{wks.location}/../doc/settings.ini\" \"%{cfg.targetdir}\"" }
		postbuildcommands { "{COPY} \"%{wks.location}/../media/packages/\" \"%{cfg.targetdir}/packages/\""}

	-- SFML post-build.
	filter { "system:windows", "platforms:x32" }
		postbuildcommands { "{COPY} \"%{wks.location}/../dependencies/SFML/extlibs/bin/x86/openal32.dll\" \"%{cfg.targetdir}\"" }
		postbuildcommands { "{COPY} \"%{wks.location}/../dependencies/freetype-windows-binaries/release dll/win32/freetype.dll\" \"%{cfg.targetdir}\"" }
	filter { "system:windows", "platforms:x64" }
		postbuildcommands { "{COPY} \"%{wks.location}/../dependencies/SFML/extlibs/bin/x64/openal32.dll\" \"%{cfg.targetdir}\"" }
		postbuildcommands { "{COPY} \"%{wks.location}/../dependencies/freetype-windows-binaries/release dll/win64/freetype.dll\" \"%{cfg.targetdir}\"" }

	-- Pre-link LuaJIT building.
	filter {}
		libdirs { "../dependencies/luajit-2.0/src/" }
	filter { "system:windows", "platforms:x32" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars32.bat\" && cd \"%{wks.location}\" && call build_lua.bat static" }
	filter { "system:windows", "platforms:x64" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars64.bat\" && cd \"%{wks.location}\" && call build_lua.bat static" }

--[[
project "2drp_server"
	kind "ConsoleApp"
	language "C++"
	location "projects"
	targetdir "../bin_server"

	vectorextensions "SSE2"
	rtti "On"

	files { "../src/**" }
	removefiles { "../src/client/**" }
	includedirs { "../src" }

	-- Library includes.
	includedirs {
		-- Header only.
		"../dependencies/mathfu/include/",
		"../dependencies/mathfu/dependencies/vectorial/include/",
		"../dependencies/BabyDI/include/",
		"../dependencies/pugixml/src/",

		"../dependencies/bzip2/",
		"../dependencies/zlib/",

		-- Libs.
		"../dependencies/ziplib/Source/ZipLib/",
		"../dependencies/enet/include/",
		"../dependencies/protobuf/src/",
		"../dependencies/PlayRho/",
		"../dependencies/sol2/include/",
		"../dependencies/luajit-2.0/src/",
		"../dependencies/tmxlite/tmxlite/include/",
		"../dependencies/Clipper2/CPP/Clipper2Lib/include/",
		"../dependencies/polypartition/src/",
	}

	dependson { "PlayRho", "bzip2", "ziplib", "enet", "tmxlite", "Clipper2", "polypartition" }

	-- Libraries.
	links {
		"ziplib",
		"enet",
		"protobuf",
		"PlayRho",
		"lua51",
		"tmxlite",
		"Clipper2",
		"polypartition",
	}

	defines { "NOMINMAX", "PUGIXML_HEADER_ONLY" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

	-- Disable MSVC warnings because 3rd party libraries never update.
	filter "toolset:msc*"
		defines { "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING", "_SILENCE_CXX20_IS_POD_DEPRECATION_WARNING" }

	-- Per-platform libraries.
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		links { "pthread", "dl" }

	-- Pre-link LuaJIT building.
	libdirs { "../dependencies/luajit-2.0/src/" }
	filter { "system:windows", "platforms:x32" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars32.bat\" && cd \"%{wks.location}\" && call build_lua.bat static" }
	filter { "system:windows", "platforms:x64" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars64.bat\" && cd \"%{wks.location}\" && call build_lua.bat static" }
--]]

project "SFML"
	kind "StaticLib"
	language "C++"
	location "projects"
	-- dependson { "flac", "ogg", "vorbis" }
	includedirs {
		"../dependencies/SFML/include/",
		"../dependencies/SFML/src/",
		"../dependencies/SFML/include/SFML/Audio/",
		"../dependencies/SFML/include/SFML/Graphics/",
		"../dependencies/SFML/include/SFML/Main/",
		"../dependencies/SFML/include/SFML/System/",
		"../dependencies/SFML/include/SFML/Window/",
		"../dependencies/SFML/extlibs/headers/",
		"../dependencies/SFML/extlibs/headers/AL/",
		"../dependencies/SFML/extlibs/headers/freetype2/",
		"../dependencies/SFML/extlibs/headers/glad/include/",
		"../dependencies/SFML/extlibs/headers/minimp3/",
		"../dependencies/SFML/extlibs/headers/stb_image/",
		"../dependencies/SFML/extlibs/headers/vulkan/",
		-- "../dependencies/flac/include/",
		-- "../dependencies/freetype2/include/",
		"../dependencies/freetype-windows-binaries/include/",
		-- "../dependencies/_config/ogg/include/",
		-- "../dependencies/ogg/include/",
		-- "../dependencies/OpenAL/include/AL/",
		-- "../dependencies/vorbis/include/",
	}
	files {
		-- "../dependencies/SFML/extlibs/headers/AL/*",
		"../dependencies/SFML/extlibs/headers/FLAC/*",
		-- "../dependencies/SFML/extlibs/headers/freetype2/*",
		-- "../dependencies/SFML/extlibs/headers/glad/include/*",
		-- "../dependencies/SFML/extlibs/headers/minimp3/*",
		"../dependencies/SFML/extlibs/headers/ogg/*",
		-- "../dependencies/SFML/extlibs/headers/stb_image/*",
		"../dependencies/SFML/extlibs/headers/vorbis/*",
		-- "../dependencies/SFML/extlibs/headers/vulkan/*",
	}
	files {
		"../dependencies/SFML/src/SFML/Audio/**",
		"../dependencies/SFML/src/SFML/Graphics/**",
		"../dependencies/SFML/src/SFML/System/*",
		"../dependencies/SFML/src/SFML/Window/*",
	}
	removefiles {
		"../dependencies/SFML/src/SFML/Window/EGLCheck.*",
		"../dependencies/SFML/src/SFML/Window/EglContext.*",
	}
	links {
		"flac",
		"freetype",
		"ogg",
		"OpenAL32",
		"vorbis",
		"vorbisenc",
		"vorbisfile",
		"OpenGL32", -- external
	}
	defines {
		-- Avoid warnings in vorbisfile.h
		"OV_EXCLUDE_STATIC_CALLBACKS",
		"FLAC__NO_DLL",

		"STBI_FAILURE_USERMSG",
		"SFML_STATIC",
	}

	filter "system:windows"
		files {
			"../dependencies/SFML/src/SFML/Main/MainWin32.cpp",
			"../dependencies/SFML/src/SFML/System/Win32/**",
			"../dependencies/SFML/src/SFML/Window/Win32/**",
		}
		disablewarnings { "4068" }
		defines { "UNICODE", "_UNICODE", "_CRT_SECURE_NO_DEPRECATE", "_SCL_SECURE_NO_WARNINGS" }
		links { "winmm", "gdi32" }
	filter "system:linux"
		files {
			"../dependencies/SFML/src/SFML/System/Unix/**",
			"../dependencies/SFML/src/SFML/Window/Unix/**",
		}
		links { "rt", "pthread", "X11", "UDev", "dl" }
	filter { "toolset:gcc", "files:ImageLoader.cpp" }
		buildoptions { "-fno-strict-aliasing" }

	filter { "toolset:msc*"}
		links { "legacy_stdio_definitions" }

	filter { "system:windows", "platforms:x32" }
		libdirs {
			"../dependencies/freetype-windows-binaries/release dll/win32/",
			"../dependencies/SFML/extlibs/libs-msvc-universal/x86/",
		}
	filter { "system:windows", "platforms:x64" }
		libdirs {
			"../dependencies/freetype-windows-binaries/release dll/win64/",
			"../dependencies/SFML/extlibs/libs-msvc-universal/x64/",
		}

project "PlayRho"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/PlayRho/PlayRho/**.h", "../dependencies/PlayRho/PlayRho/**.cpp" }
	includedirs { "../dependencies/PlayRho/" }

project "bzip2"
	kind "StaticLib"
	language "C"
	compileas "C"
	location "projects"
	files { "../dependencies/bzip2/**.h", "../dependencies/bzip2/**.c" }
	includedirs { "../dependencies/bzip2/" }
	removefiles {
		"../dependencies/bzip2/bzip2.c",
		"../dependencies/bzip2/bzip2recover.c",
		"../dependencies/bzip2/dlltest.c",
		"../dependencies/bzip2/mk251.c",
		"../dependencies/bzip2/spewG.c",
		"../dependencies/bzip2/unzcrash.c",
	}

project "zlib"
	kind "StaticLib"
	language "C"
	compileas "C"
	location "projects"
	files { "../dependencies/zlib/*.h", "../dependencies/zlib/*.c" }
	includedirs { "../dependencies/zlib/" }
	filter "toolset:msc*"
		disablewarnings { "4996" }

project "enet"
	kind "StaticLib"
	language "C"
	compileas "C"
	location "projects"
	files { "../dependencies/enet/**.h", "../dependencies/enet/**.c" }
	includedirs { "../dependencies/enet/include/" }

	filter "system:windows"
		links { "ws2_32", "Winmm" }

	filter "system:linux"
		defines { "HAS_SOCKLEN_T" }

-- project "mathfu"
-- 	kind "None"
-- 	files { "../dependencies/mathfu/include/**" }

-- project "pugixml"
-- 	kind "StaticLib"
-- 	language "C++"
-- 	location "projects"
-- 	files { "../dependencies/pugixml/src/**" }
-- 	flags { "NoPCH", "NoMinimalRebuild" }
-- 	editandcontinue "Off"
-- 	uuid "89A1E353-E2DC-495C-B403-742BE206ACED"

project "protobuf"
	kind "StaticLib"
	language "C++"
	location "projects"
	includedirs { "../dependencies/protobuf/src/", "../dependencies/zlib/" }
	defines { "HAVE_ZLIB", "UNICODE", "_UNICODE" }
	links { "zlib" }
	optimize "Size"
	-- staticruntime "On"

	cppdialect "C++17"

	-- libprotobuf-lite.cmake
	files {
		"../dependencies/protobuf/src/google/protobuf/any_lite.cc",
		"../dependencies/protobuf/src/google/protobuf/arena.cc",
		"../dependencies/protobuf/src/google/protobuf/arenastring.cc",
		"../dependencies/protobuf/src/google/protobuf/arenaz_sampler.cc",
		"../dependencies/protobuf/src/google/protobuf/extension_set.cc",
		"../dependencies/protobuf/src/google/protobuf/generated_enum_util.cc",
		"../dependencies/protobuf/src/google/protobuf/generated_message_tctable_lite.cc",
		"../dependencies/protobuf/src/google/protobuf/generated_message_util.cc",
		"../dependencies/protobuf/src/google/protobuf/implicit_weak_message.cc",
		"../dependencies/protobuf/src/google/protobuf/inlined_string_field.cc",
		"../dependencies/protobuf/src/google/protobuf/io/coded_stream.cc",
		"../dependencies/protobuf/src/google/protobuf/io/io_win32.cc",
		"../dependencies/protobuf/src/google/protobuf/io/strtod.cc",
		"../dependencies/protobuf/src/google/protobuf/io/zero_copy_stream.cc",
		"../dependencies/protobuf/src/google/protobuf/io/zero_copy_stream_impl.cc",
		"../dependencies/protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.cc",
		"../dependencies/protobuf/src/google/protobuf/map.cc",
		"../dependencies/protobuf/src/google/protobuf/message_lite.cc",
		"../dependencies/protobuf/src/google/protobuf/parse_context.cc",
		"../dependencies/protobuf/src/google/protobuf/repeated_field.cc",
		"../dependencies/protobuf/src/google/protobuf/repeated_ptr_field.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/bytestream.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/common.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/int128.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/status.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/statusor.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/stringpiece.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/stringprintf.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/structurally_valid.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/strutil.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/time.cc",
		"../dependencies/protobuf/src/google/protobuf/wire_format_lite.cc",
	}

	-- libprotobuf.cmake
	files {
		"../dependencies/protobuf/src/google/protobuf/any.cc",
		"../dependencies/protobuf/src/google/protobuf/any.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/api.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/compiler/importer.cc",
		"../dependencies/protobuf/src/google/protobuf/compiler/parser.cc",
		"../dependencies/protobuf/src/google/protobuf/descriptor.cc",
		"../dependencies/protobuf/src/google/protobuf/descriptor.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/descriptor_database.cc",
		"../dependencies/protobuf/src/google/protobuf/duration.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/dynamic_message.cc",
		"../dependencies/protobuf/src/google/protobuf/empty.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/extension_set_heavy.cc",
		"../dependencies/protobuf/src/google/protobuf/field_mask.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/generated_message_bases.cc",
		"../dependencies/protobuf/src/google/protobuf/generated_message_reflection.cc",
		"../dependencies/protobuf/src/google/protobuf/generated_message_tctable_full.cc",
		"../dependencies/protobuf/src/google/protobuf/io/gzip_stream.cc",
		"../dependencies/protobuf/src/google/protobuf/io/printer.cc",
		"../dependencies/protobuf/src/google/protobuf/io/tokenizer.cc",
		"../dependencies/protobuf/src/google/protobuf/map_field.cc",
		"../dependencies/protobuf/src/google/protobuf/message.cc",
		"../dependencies/protobuf/src/google/protobuf/reflection_internal.h",
		"../dependencies/protobuf/src/google/protobuf/reflection_ops.cc",
		"../dependencies/protobuf/src/google/protobuf/service.cc",
		"../dependencies/protobuf/src/google/protobuf/source_context.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/struct.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/stubs/substitute.cc",
		"../dependencies/protobuf/src/google/protobuf/text_format.cc",
		"../dependencies/protobuf/src/google/protobuf/timestamp.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/type.pb.cc",
		"../dependencies/protobuf/src/google/protobuf/unknown_field_set.cc",
		"../dependencies/protobuf/src/google/protobuf/util/delimited_message_util.cc",
		"../dependencies/protobuf/src/google/protobuf/util/field_comparator.cc",
		"../dependencies/protobuf/src/google/protobuf/util/field_mask_util.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/datapiece.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/default_value_objectwriter.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/error_listener.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/field_mask_utility.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/json_escaping.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/json_objectwriter.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/json_stream_parser.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/object_writer.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/proto_writer.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/protostream_objectsource.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/protostream_objectwriter.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/type_info.cc",
		"../dependencies/protobuf/src/google/protobuf/util/internal/utility.cc",
		"../dependencies/protobuf/src/google/protobuf/util/json_util.cc",
		"../dependencies/protobuf/src/google/protobuf/util/message_differencer.cc",
		"../dependencies/protobuf/src/google/protobuf/util/time_util.cc",
		"../dependencies/protobuf/src/google/protobuf/util/type_resolver_util.cc",
		"../dependencies/protobuf/src/google/protobuf/wire_format.cc",
		"../dependencies/protobuf/src/google/protobuf/wrappers.pb.cc",
	}

	filter "toolset:msc*"
		disablewarnings { "4018", "4065", "4146", "4244", "4251", "4267", "4305", "4307", "4309", "4334", "4355", "4506", "4800", "4996" }

	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		defines { "HAVE_PTHREAD" }
		links { "pthread" }

project "ziplib"
	kind "StaticLib"
	language "C++"
	location "projects"
	defines { "_LIB" }
	includedirs {
		"../dependencies/ziplib/Source/ZipLib/"
	}
	files {
		"../dependencies/ziplib/Source/ZipLib/**.h",
		"../dependencies/ziplib/Source/ZipLib/**.cpp",
		"../dependencies/ziplib/Source/ZipLib/**.c",
	}
	filter "system:windows"
		removefiles {
			"../dependencies/ziplib/Source/ZipLib/extlibs/lzma/unix/**",
		}
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		links { "pthread" }
	filter "toolset:msc*"
		disablewarnings { "4996" }

project "SpriterPlusPlus"
	kind "StaticLib"
	language "C++"
	location "projects"
	includedirs {
		"../dependencies/SpriterPlusPlus/spriterengine",
	}
	files {
		"../dependencies/SpriterPlusPlus/spriterengine/**",
	}
	filter "toolset:msc*"
		disablewarnings { "26812" }

project "tmxlite"
	kind "StaticLib"
	language "C++"
	location "projects"
	includedirs {
		"../dependencies/tmxlite/tmxlite/include",
	}
	files {
		"../dependencies/tmxlite/tmxlite/include/**",
		"../dependencies/tmxlite/tmxlite/src/**"
	}
	defines { "TMXLITE_STATIC" }

	-- Use header-only pugixml to avoid linker errors.
	defines { "PUGIXML_HEADER_ONLY" }

project "RmlUi"
	kind "StaticLib"
	language "C++"
	location "projects"
	includedirs {
		"../dependencies/RmlUi/Include/",
		"../dependencies/luajit-2.0/src/",
		"../dependencies/freetype-windows-binaries/include/",
	}
	files {
		"../dependencies/RmlUi/Source/Core/**",
		"../dependencies/RmlUi/Source/Debugger/**",
		-- "../dependencies/RmlUi/Source/Lua/**",
	}
	defines { "RMLUI_STATIC_LIB", "_CRT_SECURE_NO_WARNINGS" }
	links { "freetype" }
	libdirs { "../dependencies/luajit-2.0/src/" }

	-- For freetype2.
	filter { "system:windows", "platforms:x32" }
		libdirs {
			"../dependencies/freetype-windows-binaries/release dll/win32/",
			-- "../dependencies/SFML/extlibs/libs-msvc-universal/x86/",
		}
	filter { "system:windows", "platforms:x64" }
		libdirs {
			"../dependencies/freetype-windows-binaries/release dll/win64/",
			-- "../dependencies/SFML/extlibs/libs-msvc-universal/x64/",
		}

project "RmlSolLua"
	kind "StaticLib"
	language "C++"
	location "projects"

	-- Add files.
	files {
		"../dependencies/RmlSolLua/src/**",
		"../dependencies/RmlSolLua/include/**",
	}
	includedirs {
		"../dependencies/RmlSolLua/src",
		"../dependencies/RmlSolLua/include",
	}

	-- Library includes.
	includedirs {
		"../dependencies/sol2/include/",
		"../dependencies/RmlUi/Include/",
		"../dependencies/luajit-2.0/src/",
	}

	-- Links.
	links {
		"lua51",
		"RmlUi",
	}
	libdirs { "../dependencies/luajit-2.0/src/" }

	-- Defines
	defines {
		"RMLUI_STATIC_LIB",
		-- "RMLUI_NO_THIRDPARTY_CONTAINERS".	-- Enable to use STL containers.
	}

	dependson { "RmlUi" }

project "Clipper2"
	kind "StaticLib"
	language "C++"
	location "projects"

	-- Add files.
	files {
		"../dependencies/Clipper2/CPP/Clipper2Lib/include/**",
		"../dependencies/Clipper2/CPP/Clipper2Lib/src/**",
	}
	includedirs {
		"../dependencies/Clipper2/CPP/Clipper2Lib/include/",
	}

project "polypartition"
	kind "StaticLib"
	language "C++"
	location "projects"

	files { "../dependencies/polypartition/src/**" }
	includedirs { "../dependencies/polypartition/src/" }
