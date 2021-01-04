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
	cppdialect "C++20"

	linkgroups "On"

	filter "configurations:Debug"
	 	defines { "DEBUG", "_DEBUG" }
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
	filter "toolset:clang"
		buildoptions { "-std=c++17", "-Wno-switch" }

	-- Toolset specific
	filter "toolset:msc*"
		defines { "_CRT_SECURE_NO_WARNINGS" }
		disablewarnings { "5105" }
		buildoptions {
			"/guard:cf",	-- Control Flow Guard
			"/Qspectre",	-- Spectre Mitigation
			"/Zc:preprocessor",	-- Use alternative preprocessor (like GCC/clang, required for BabyDI)
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

	vectorextensions "SSE2"
	rtti "On"

	files { "../src/**" }
	removefiles { "../src/server/**" }
	includedirs { "../src" }

	-- Library includes.
	includedirs {
		"../dependencies/SFML/include/",
		"../dependencies/PlayRho/",
		"../dependencies/bzip2/",
		"../dependencies/zlib/",
		"../dependencies/enet/include/",
		"../dependencies/mathfu/include/",
		"../dependencies/mathfu/dependencies/vectorial/include/",
		"../dependencies/BabyDI/include/",
		"../dependencies/pugixml/src/",
		"../dependencies/protobuf-3.5.1/src/",
		"../dependencies/sol2/include/",
		"../dependencies/luajit-2.0/src/",
		"../dependencies/ziplib/Source/ZipLib/",
	}

	dependson { "SFML", "PlayRho", "bzip2", "zlib", "enet", "SpriterPlusPlus" }

	-- Libraries.
	links {
		"SFML",
		"PlayRho",
		"bzip2",
		"zlib",
		"enet",
		"pugixml",
		"protobuf",
		"lua51",
		"ziplib",
	}

	defines { "SFML_STATIC", "NOMINMAX" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

	-- Post-build commands.
	filter {}
		postbuildcommands { "{COPY} %{wks.location}/../doc/settings.ini %{cfg.targetdir}" }
		postbuildcommands { "{COPY} %{wks.location}/../media/packages/login/ %{cfg.targetdir}/packages/login/"}

	-- Pre-link LuaJIT building.
	filter {}
		libdirs { "../dependencies/luajit-2.0/src/" }
	filter { "system:windows", "platforms:x32" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars32.bat\" && cd \"%{wks.location}/../dependencies/luajit-2.0/src/\" && call msvcbuild.bat" }
	filter { "system:windows", "platforms:x64" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars64.bat\" && cd \"%{wks.location}/../dependencies/luajit-2.0/src/\" && call msvcbuild.bat" }
	filter { "system:windows" }
		postbuildcommands { "{COPY} %{wks.location}/../dependencies/luajit-2.0/src/lua51.dll %{cfg.targetdir}" }

	-- Per-platform libraries.
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		links { "pthread", "dl" }

	filter { "system:linux" }
		links { "X11", "GL" }


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

	-- Libraries.
	links {
		"PlayRho",
		"bzip2",
		"zlib",
		"enet",
		"pugixml",
		"protobuf",
		"lua51",
		"ziplib",
	}

	-- Library includes.
	includedirs {
		"../dependencies/PlayRho/",
		"../dependencies/bzip2/",
		"../dependencies/zlib/",
		"../dependencies/enet/include/",
		"../dependencies/mathfu/include/",
		"../dependencies/mathfu/dependencies/vectorial/include/",
		"../dependencies/pugixml/src/",
		"../dependencies/protobuf-3.5.1/src/",
		"../dependencies/sol2/include/",
		"../dependencies/luajit-2.0/src/",
		"../dependencies/ziplib/Source/ZipLib/",
	}

	dependson { "PlayRho", "bzip2", "zlib", "enet" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

	-- Pre-link LuaJIT building.
	libdirs { "../dependencies/luajit-2.0/src/" }
	filter { "system:windows", "platforms:x32" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars32.bat\" && cd \"%{wks.location}/../dependencies/luajit-2.0/src/\" && call msvcbuild.bat" }
	filter { "system:windows", "platforms:x64" }
		prebuildcommands { "call \"$(DevEnvDir)../../VC/Auxiliary/Build/vcvars64.bat\" && cd \"%{wks.location}/../dependencies/luajit-2.0/src/\" && call msvcbuild.bat" }
	filter { "system:windows" }
		postbuildcommands { "{COPY} %{wks.location}/../dependencies/luajit-2.0/src/lua51.dll %{cfg.targetdir}" }

	-- Per-platform libraries.
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		links { "pthread", "dl" }


project "SFML"
	kind "StaticLib"
	language "C++"
	location "projects"
	dependson { "flac", "ogg", "vorbis" }
	includedirs {
		"../dependencies/SFML/include/",
		"../dependencies/SFML/src/",
		"../dependencies/SFML/extlibs/headers/stb_image/",
		"../dependencies/SFML/extlibs/headers/glad/include/",
		"../dependencies/SFML/extlibs/headers/vulkan/",
		"../dependencies/flac/include/",
		"../dependencies/freetype2/include/",
		"../dependencies/_config/ogg/include/",
		"../dependencies/ogg/include/",
		"../dependencies/OpenAL/include/AL/",
		"../dependencies/vorbis/include/",
	}
	files {
		"../dependencies/SFML/src/SFML/Audio/**",
		"../dependencies/SFML/src/SFML/Graphics/**",
		"../dependencies/SFML/src/SFML/System/*.cpp",
		"../dependencies/SFML/src/SFML/Window/*",
	}
	removefiles {
		"../dependencies/SFML/src/SFML/Window/EGLCheck.*",
		"../dependencies/SFML/src/SFML/Window/EglContext.*",
	}
	links {
		"flac",
		"ogg",
		"vorbis",
		"freetype", -- external
		"OpenAL32", -- external
		"OpenGL32", -- external
	}
	defines {
		"OV_EXCLUDE_STATIC_CALLBACKS",
		"FLAC__NO_DLL",
		"SFML_STATIC",
	}
	filter { "system:windows", "platforms:x32" }
		libdirs { "../dependencies/freetype2/release dll/win32/", "../dependencies/OpenAL/lib/win32/" }
	filter { "system:windows", "platforms:x64" }
		libdirs { "../dependencies/freetype2/release dll/win64/", "../dependencies/OpenAL/lib/win64/" }
	filter "system:windows"
		files {
			"../dependencies/SFML/src/SFML/Main/MainWin32.cpp",
			"../dependencies/SFML/src/SFML/System/Win32/**",
			"../dependencies/SFML/src/SFML/Window/Win32/**",
		}
		defines { "UNICODE", "_UNICODE" }
		links { "winmm", "gdi32" }
	filter "system:linux"
		files {
			"../dependencies/SFML/src/SFML/System/Unix/**",
			"../dependencies/SFML/src/SFML/Window/Unix/**",
		}
		links { "pthread" }
	filter { "toolset:gcc", "files:ImageLoader.cpp" }
		buildoptions { "-fno-strict-aliasing" }

	-- Post-build commands
	filter { "system:windows", "platforms:x32" }
		postbuildcommands { "{COPY} \"%{wks.location}/../dependencies/freetype2/release dll/win32/freetype.dll\" %{cfg.targetdir}" }
	filter { "system:windows", "platforms:x64" }
		postbuildcommands { "{COPY} \"%{wks.location}/../dependencies/freetype2/release dll/win64/freetype.dll\" %{cfg.targetdir}" }

project "flac"
	kind "StaticLib"
	language "C"
	compileas "C"
	location "projects"
	dependson { "ogg" }
	includedirs {
		"../dependencies/flac/include/",
		"../dependencies/flac/src/libFLAC/include/",
		"../dependencies/_config/ogg/include/",
		"../dependencies/ogg/include/"
	}
	files { "../dependencies/flac/src/libFLAC/*.c" }
	links { "ogg" }
	defines {
		"_LIB",
		"FLAC__HAS_OGG",
		"FLAC__CPU_X86_64",
		"FLAC__NO_ASM",
		"FLAC__HAS_X86INTRIN",
		"FLAC__ALIGN_MALLOC_DATA",
		"PACKAGE_VERSION=\"1.3.2\"",
		"FLAC__NO_DLL",
		"FLaC__INLINE=_inline",
	}

project "vorbis"
	kind "StaticLib"
	language "C"
	compileas "C"
	location "projects"
	dependson "ogg"
	includedirs {
		"../dependencies/vorbis/include/",
		"../dependencies/vorbis/lib/",
		"../dependencies/_config/ogg/include/",
		"../dependencies/ogg/include/",
	}
	files {
		"../dependencies/vorbis/lib/analysis.c",
		"../dependencies/vorbis/lib/bitrate.c",
		"../dependencies/vorbis/lib/block.c",
		"../dependencies/vorbis/lib/codebook.c",
		"../dependencies/vorbis/lib/envelope.c",
		"../dependencies/vorbis/lib/floor0.c",
		"../dependencies/vorbis/lib/floor1.c",
		"../dependencies/vorbis/lib/info.c",
		"../dependencies/vorbis/lib/lookup.c",
		"../dependencies/vorbis/lib/lpc.c",
		"../dependencies/vorbis/lib/lsp.c",
		"../dependencies/vorbis/lib/mapping0.c",
		"../dependencies/vorbis/lib/mdct.c",
		"../dependencies/vorbis/lib/psy.c",
		"../dependencies/vorbis/lib/registry.c",
		"../dependencies/vorbis/lib/res0.c",
		"../dependencies/vorbis/lib/sharedbook.c",
		"../dependencies/vorbis/lib/smallft.c",
		"../dependencies/vorbis/lib/synthesis.c",
		"../dependencies/vorbis/lib/vorbisenc.c",
		"../dependencies/vorbis/lib/window.c",
		"../dependencies/vorbis/lib/vorbisenc.c",
		"../dependencies/vorbis/lib/vorbisfile.c",
	}
	links { "ogg" }
	callingconvention "Cdecl"
	defines { "_USRDLL", "LIBVORBIS_EXPORTS" }
	filter "toolset:msc*"
		disablewarnings { "4244", "4100", "4267", "4189", "4305", "4127", "4706" }

project "ogg"
	kind "StaticLib"
	language "C"
	compileas "C"
	location "projects"
	includedirs {
		"../dependencies/_config/ogg/include/",
		"../dependencies/ogg/include/",
	}
	files { "../dependencies/ogg/src/*.c" }
	defines { "INCLUDE_STDINT_H" }

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

project "pugixml"
	kind "StaticLib"
	language "C++"
	location "projects"
	files { "../dependencies/pugixml/src/**" }
	flags { "NoPCH", "NoMinimalRebuild" }
	editandcontinue "Off"
	uuid "89A1E353-E2DC-495C-B403-742BE206ACED"

project "protobuf"
	kind "StaticLib"
	language "C++"
	location "projects"
	includedirs { "../dependencies/protobuf-3.5.1/src/", "../dependencies/zlib/" }
	links { "zlib" }
	files {
		"../dependencies/protobuf-3.5.1/src/google/protobuf/any.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/any.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/api.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/compiler/importer.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/compiler/parser.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/descriptor.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/descriptor.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/descriptor_database.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/duration.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/dynamic_message.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/empty.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/extension_set_heavy.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/field_mask.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/generated_message_reflection.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/generated_message_table_driven.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/gzip_stream.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/printer.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/strtod.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/tokenizer.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/zero_copy_stream_impl.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/map_field.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/message.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/reflection_ops.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/service.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/source_context.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/struct.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/mathlimits.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/substitute.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/text_format.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/timestamp.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/type.pb.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/unknown_field_set.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/delimited_message_util.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/field_comparator.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/field_mask_util.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/datapiece.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/default_value_objectwriter.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/error_listener.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/field_mask_utility.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/json_escaping.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/json_objectwriter.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/json_stream_parser.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/object_writer.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/proto_writer.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/protostream_objectsource.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/protostream_objectwriter.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/type_info.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/type_info_test_helper.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/utility.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/json_util.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/message_differencer.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/time_util.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/type_resolver_util.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/wire_format.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/wrappers.pb.cc",
	}
	files {
		"../dependencies/protobuf-3.5.1/src/google/protobuf/any.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/any.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/api.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/compiler/importer.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/compiler/parser.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/descriptor.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/descriptor.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/descriptor_database.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/duration.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/dynamic_message.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/empty.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/field_mask.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/generated_message_reflection.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/gzip_stream.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/printer.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/strtod.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/tokenizer.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/zero_copy_stream_impl.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/map_field.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/message.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/reflection_ops.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/service.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/source_context.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/struct.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/mathlimits.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/substitute.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/text_format.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/timestamp.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/type.pb.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/unknown_field_set.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/delimited_message_util.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/field_comparator.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/field_mask_util.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/datapiece.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/default_value_objectwriter.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/error_listener.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/field_mask_utility.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/json_escaping.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/json_objectwriter.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/json_stream_parser.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/object_writer.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/proto_writer.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/protostream_objectsource.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/protostream_objectwriter.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/type_info.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/type_info_test_helper.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/internal/utility.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/json_util.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/message_differencer.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/time_util.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/util/type_resolver_util.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/wire_format.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/wrappers.pb.h",
	}
	files {
		"../dependencies/protobuf-3.5.1/src/google/protobuf/arena.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/arenastring.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/extension_set.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/generated_message_table_driven_lite.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/generated_message_util.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/coded_stream.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/zero_copy_stream.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/zero_copy_stream_impl_lite.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/message_lite.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/repeated_field.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/bytestream.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/common.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/int128.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/io_win32.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/once.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/status.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/statusor.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/stringpiece.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/stringprintf.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/structurally_valid.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/strutil.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/time.cc",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/wire_format_lite.cc",
	}
	files {
		"../dependencies/protobuf-3.5.1/src/google/protobuf/arena.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/arenastring.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/extension_set.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/generated_message_util.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/coded_stream.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/zero_copy_stream.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/io/zero_copy_stream_impl_lite.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/message_lite.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/repeated_field.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/atomicops_internals_x86_msvc.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/bytestream.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/common.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/int128.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/once.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/status.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/statusor.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/stringpiece.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/stringprintf.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/strutil.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/stubs/time.h",
		"../dependencies/protobuf-3.5.1/src/google/protobuf/wire_format_lite.h",
	}
	filter "toolset:msc*"
		disablewarnings { "4018", "4146", "4244", "4251", "4267", "4305", "4355", "4800", "4996" }
	filter { "system:linux or system:macosx or system:bsd or system:solaris" }
		defines { "HAVE_PTHREAD" }

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
		"../dependencies/SFML/include/",
		"../dependencies/pugixml/src/",
	}
	files {
		"../dependencies/SpriterPlusPlus/spriterengine/**",
		"../dependencies/SpriterPlusPlus/example/override/sfml*",
		"../dependencies/SpriterPlusPlus/example/override/pugixml*",
	}
	links { "pugixml", "SFML" }
	dependson { "pugixml", "SFML" }
	filter "toolset:msc*"
		disablewarnings { "26812" }
