# Find all .proto files in the network/proto directory.
file(GLOB PROTO_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/network/proto/*.proto")

# Create a list to hold the generated files.
set(GENERATED_PROTO_FILES)

# Loop through each .proto file and generate the corresponding .pb.cc and .pb.h files.
find_package(Protobuf CONFIG REQUIRED)
foreach(PROTO_FILE ${PROTO_FILES})
	get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
	set(GENERATED_CC "${CMAKE_CURRENT_SOURCE_DIR}/network/packets/${PROTO_NAME}.pb.cc")
	set(GENERATED_H "${CMAKE_CURRENT_SOURCE_DIR}/network/packets/${PROTO_NAME}.pb.h")
	list(APPEND GENERATED_PROTO_FILES ${GENERATED_CC} ${GENERATED_H})

	add_custom_command(
		OUTPUT ${GENERATED_CC} ${GENERATED_H}
		COMMAND "${Protobuf_PROTOC_EXECUTABLE}"
		ARGS --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/network/packets --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/network/proto ${PROTO_FILE}
		DEPENDS ${PROTO_FILE}
		COMMENT "Running protoc on ${PROTO_FILE}"
		VERBATIM
	)
endforeach()

# Add a custom target to run protoc.
add_custom_target(generate_protos ALL DEPENDS ${GENERATED_PROTO_FILES})

# Create the library target.
add_library(2drp_engine)

# Add the sources.
target_sources(2drp_engine
	PUBLIC
		component/Component.h
		events/Events.cpp
		events/Events.h
		filesystem/watch/FileWatch.h
		filesystem/watch/FileWatchOS_Linux.cpp
		filesystem/watch/FileWatchOS_OSX.cpp
		filesystem/watch/FileWatchOS_Poll.cpp
		filesystem/watch/FileWatchOS_Windows.cpp
		filesystem/common.h
		filesystem/File.cpp
		filesystem/File.h
		filesystem/FileSystem.cpp
		filesystem/FileSystem.h
		filesystem/Log.cpp
		filesystem/Log.h
		filesystem/ProgramSettings.cpp
		filesystem/ProgramSettings.h
		helper/base64.h
		helper/flags.h
		helper/math.h
		item/Item.h
		loader/LevelLoader.cpp
		loader/Loader.h
		loader/PackageLoader.cpp
		network/construct/Attributes.h
		network/construct/Class.h
		network/construct/SceneObject.h
		network/construct/Tileset.h
		${PROTO_FILES}
		${GENERATED_PROTO_FILES}
		network/DownloadManager.cpp
		network/DownloadManager.h
		network/Network.cpp
		network/Network.h
		network/Packet.h
		network/PacketHandler.cpp
		network/PacketHandler.h
		network/PacketsInc.h
		os/Utils.h
		os/Windows.cpp
		package/Package.cpp
		package/Package.h
		physics/Physics.cpp
		physics/Physics.h
		resources/Resource.h
		scene/ObjectAttributes.cpp
		scene/ObjectAttributes.h
		scene/ObjectClass.h
		scene/ObjectProperties.cpp
		scene/ObjectProperties.h
		scene/Scene.cpp
		scene/Scene.h
		scene/SceneObject.cpp
		scene/SceneObject.h
		scene/Tileset.h
		script/helpers/AttributeHelper.cpp
		script/helpers/AttributeHelper.h
		script/modules/Attributes.cpp
		script/modules/bind.h
		script/modules/Event.cpp
		script/modules/Item.cpp
		script/modules/Physics.cpp
		script/modules/Player.cpp
		script/modules/Scene.cpp
		script/modules/SceneObject.cpp
		script/modules/Server.cpp
		script/modules/Vector.cpp
		script/Function.h
		script/Script.cpp
		script/Script.h
		server/Account.cpp
		server/Account.h
		server/Player.cpp
		server/Player.h
		server/Server.cpp
		server/Server.h
		common.h
		Enum.h
)
target_sources(2drp_engine
	PUBLIC FILE_SET HEADERS
		BASE_DIRS
			../
)

# BabyDI.
target_sources(2drp_engine PUBLIC "${DEPENDENCY_DIRECTORY}/BabyDI/include/BabyDI.hpp")

# Polypartition.
target_sources(2drp_engine
	PRIVATE
		"${DEPENDENCY_DIRECTORY}/polypartition/src/polypartition.cpp"
		"${DEPENDENCY_DIRECTORY}/polypartition/src/polypartition.h")

# Include directories for submodule libraries.
target_include_directories(2drp_engine PUBLIC
	"${DEPENDENCY_DIRECTORY}/mathfu/include/"
	"${DEPENDENCY_DIRECTORY}/mathfu/dependencies/vectorial/include/"
	"${DEPENDENCY_DIRECTORY}/BabyDI/include/"
	"${DEPENDENCY_DIRECTORY}/polypartition/src/"
)

# Include Lua.
target_include_directories(2drp_engine PRIVATE ${LUA_INCLUDE_DIRECTORIES})

# Configure our compiler options.
set_default_compiler_options(2drp_engine)

# Dependency: Compression libs.
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Zstd CONFIG REQUIRED)
target_link_libraries(2drp_engine PUBLIC zstd::libzstd)

# Dependency: Clipper2.
find_package(PkgConfig REQUIRED)
pkg_check_modules(Clipper2 REQUIRED IMPORTED_TARGET Clipper2)
target_link_libraries(2drp_engine PRIVATE PkgConfig::Clipper2)

# Dependency: Protobuf.
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(2drp_engine PUBLIC protobuf::libprotobuf protobuf::libprotobuf-lite)

# Dependency: Pugixml.
find_package(pugixml CONFIG REQUIRED)
target_link_libraries(2drp_engine PUBLIC pugixml::pugixml)

# Dependency: Tmxlite.
pkg_check_modules(tmxlite REQUIRED IMPORTED_TARGET tmxlite)
target_link_libraries(2drp_engine PUBLIC PkgConfig::tmxlite)

# Dependency: ZipLib.
FetchContent_Declare(ZipLib
	GIT_REPOSITORY https://github.com/LoneBoco/ZipLib.git
	GIT_TAG master GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(ZipLib)
target_include_directories(2drp_engine PUBLIC "${ZipLib_SOURCE_DIR}/Source/ZipLib")

# Dependency: enet.
FetchContent_Declare(enet
	GIT_REPOSITORY https://github.com/lsalzman/enet.git
	GIT_TAG v1.3.18 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(enet)
target_include_directories(2drp_engine PUBLIC "${enet_SOURCE_DIR}/include")
target_link_libraries(2drp_engine PUBLIC winmm ws2_32)

# Dependency: PlayRho.
FetchContent_Declare(PlayRho
	GIT_REPOSITORY https://github.com/louis-langholtz/PlayRho.git
	GIT_TAG v1.1.2 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(PlayRho)
include_directories("${PlayRho_SOURCE_DIR}")

# Link everything together.
target_link_libraries(2drp_engine
	PUBLIC
	${Boost_LIBRARIES}
	${LUA_LINK_LIBRARIES}
	ZipLib
	enet
	PlayRho::PlayRho
)

# Ensure the protoc target is built before 2drp_engine.
add_dependencies(2drp_engine generate_protos)
