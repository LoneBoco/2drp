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

	-- OS defines
	filter "system:windows"
		defines { "TDRP_WINDOWS" }
	filter "system:linux"
		defines { "TDRP_LINUX" }
	filter "system:macosx"
		defines { "TDRP_MACOSX" }

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
	includedirs { "../src" }

	-- Libraries.
	links {
		"bgfx",
		"Box2D",
		"bzip2",
		"zlib",
		"enet",
		"pugixml",
		"protobuf",
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
		"../dependencies/protobuf-3.5.1/src/",
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
	includedirs { "../src" }

	-- Libraries.
	links {
		"Box2D",
		"bzip2",
		"zlib",
		"enet",
		"pugixml",
		"protobuf",
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
		"../dependencies/protobuf-3.5.1/src/",
	}

	dependson { "box2d", "bzip2", "zlib", "enet" }

	-- Boost
	includedirs { os.getenv("BOOST_ROOT") or "../dependencies/boost/" }
	libdirs { path.join(os.getenv("BOOST_ROOT") or "../dependencies/boost/", "/stage/lib") }

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
		"../dependencies/bzip2/unzcrash.c",
	}

project "zlib"
	kind "StaticLib"
	language "C"
	location "projects"
	files { "../dependencies/zlib/*.h", "../dependencies/zlib/*.c" }
	includedirs { "../dependencies/zlib/" }
	filter "toolset:msc*"
		disablewarnings { "4996" }

project "enet"
	kind "StaticLib"
	language "C"
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
