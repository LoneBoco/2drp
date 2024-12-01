# Create the library target.
add_executable(2drp)

# Add the sources.
target_sources(2drp
	PUBLIC
		game/Camera.cpp
		game/Camera.h
		game/Game.cpp
		game/Game.h
		loader/gani/GaniLoader.cpp
		loader/gani/GaniLoader.h
		loader/spriter/filefactory.cpp
		loader/spriter/filefactory.h
		loader/spriter/jsonspriterfileattributewrapper.cpp
		loader/spriter/jsonspriterfileattributewrapper.h
		loader/spriter/jsonspriterfiledocumentwrapper.cpp
		loader/spriter/jsonspriterfiledocumentwrapper.h
		loader/spriter/jsonspriterfileelementwrapper.cpp
		loader/spriter/jsonspriterfileelementwrapper.h
		loader/spriter/objectfactory.cpp
		loader/spriter/objectfactory.h
		loader/spriter/pugixmlspriterfileattributewrapper.cpp
		loader/spriter/pugixmlspriterfileattributewrapper.h
		loader/spriter/pugixmlspriterfiledocumentwrapper.cpp
		loader/spriter/pugixmlspriterfiledocumentwrapper.h
		loader/spriter/pugixmlspriterfileelementwrapper.cpp
		loader/spriter/pugixmlspriterfileelementwrapper.h
		loader/spriter/sfmlatlasfile.cpp
		loader/spriter/sfmlatlasfile.h
		loader/spriter/sfmlboneinstanceinfo.cpp
		loader/spriter/sfmlboneinstanceinfo.h
		loader/spriter/sfmlboxinstanceinfo.cpp
		loader/spriter/sfmlboxinstanceinfo.h
		loader/spriter/sfmlimagefile.cpp
		loader/spriter/sfmlimagefile.h
		loader/spriter/sfmlpointinstanceinfo.cpp
		loader/spriter/sfmlpointinstanceinfo.h
		loader/spriter/sfmlsoundfile.cpp
		loader/spriter/sfmlsoundfile.h
		loader/spriter/sfmlsoundobjectinforeference.cpp
		loader/spriter/sfmlsoundobjectinforeference.h
		loader/ui/UILoader.cpp
		loader/ui/UILoader.h
		loader/ResourceLoaders.h
		loader/SFMListream.h
		network/ServerPacketHandler.cpp
		network/ServerPacketHandler.h
		render/component/Animation/GaniAnimation.cpp
		render/component/Animation/GaniAnimation.h
		render/component/Animation/IAnimationSystem.h
		render/component/Animation/SpriterAnimation.cpp
		render/component/Animation/SpriterAnimation.h
		render/component/helper/SceneObject.h
		render/component/AnimationRenderComponent.cpp
		render/component/AnimationRenderComponent.h
		render/component/Renderable.h
		render/component/RenderComponent.cpp
		render/component/RenderComponent.h
		render/component/TextRenderComponent.cpp
		render/component/TextRenderComponent.h
		render/component/TileMapRenderComponent.cpp
		render/component/TileMapRenderComponent.h
		render/component/TMXRenderComponent.cpp
		render/component/TMXRenderComponent.h
		render/Window.cpp
		render/Window.h
		script/Script.cpp
		script/Script.h
		ui/interface/RmlUi_Backend.h
		ui/interface/RmlUi_Backend_SFML_GL2.cpp
		ui/interface/RmlUi_Include_Windows.h
		ui/interface/RmlUi_Include_Xlib.h
		ui/interface/RmlUi_Platform_SFML.cpp
		ui/interface/RmlUi_Platform_SFML.h
		ui/interface/RmlUi_Renderer_GL2.cpp
		ui/interface/RmlUi_Renderer_GL2.h
		ui/interface/ShellFileInterface.cpp
		ui/interface/ShellFileInterface.h
		ui/UIManager.cpp
		ui/UIManager.h
		BabyDI.Configuration.h
		main.cpp
)
target_sources(2drp
	PUBLIC FILE_SET HEADERS
		BASE_DIRS
			../
)

# Include directories for submodule header libraries.
target_include_directories(2drp PUBLIC
	"${DEPENDENCY_DIRECTORY}/mathfu/include/"
	"${DEPENDENCY_DIRECTORY}/mathfu/dependencies/vectorial/include/"
	"${DEPENDENCY_DIRECTORY}/BabyDI/include/"
)

# Include Lua.
target_include_directories(2drp PRIVATE ${LUA_INCLUDE_DIRECTORIES})

# Configure our compiler options.
set_default_compiler_options(2drp)

# Dependency: RmlUi.
add_compile_definitions(RMLUI_STATIC_LIB)
find_package(RmlUi CONFIG REQUIRED)
target_link_libraries(2drp PRIVATE RmlUi::RmlUi)

# Dependency: RmlSolLua.
FetchContent_Declare(RmlSolLua
	GIT_REPOSITORY https://github.com/LoneBoco/RmlSolLua.git
	GIT_TAG main GIT_SHALLOW TRUE OVERRIDE_FIND_PACKAGE)
FetchContent_MakeAvailable(RmlSolLua)
target_include_directories(2drp PUBLIC "${RmlSolLua_SOURCE_DIR}/include")

# Dependency: SFML.
set(SFML_BUILD_NETWORK OFF)
set(SFML_USE_STATIC_STD_LIBS TRUE)
#set(SFML_USE_SYSTEM_DEPS ON)
target_compile_definitions(2drp PUBLIC SFML_STATIC)
FetchContent_Declare(Sfml
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG master GIT_SHALLOW TRUE
	OVERRIDE_FIND_PACKAGE)
FetchContent_MakeAvailable(Sfml)
target_include_directories(2drp PUBLIC "${SFML_SOURCE_DIR}/include")

# Dependency: nlohmann json.
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(2drp PRIVATE nlohmann_json::nlohmann_json)

# Dependency: SpriterPlusPlus.
set(SKIP_EXAMPLES ON)
FetchContent_Declare(SpriterPlusPlus
	GIT_REPOSITORY https://github.com/lucidspriter/SpriterPlusPlus.git
	GIT_TAG master GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(SpriterPlusPlus)
target_include_directories(2drp PUBLIC "${SPRITER_PLUS_PLUS_SOURCE_DIR}")

# Link it up.
target_link_libraries(2drp
	PUBLIC
	${LUA_LINK_LIBRARIES}
	2drp_engine
	RmlSolLua
	SFML::System
	SFML::Window
	SFML::Graphics
	SFML::Audio
	spriterengine
)

#[[file(GLOB SUBDIRS "${CMAKE_SOURCE_DIR}/media/*")
foreach(SUBDIR ${SUBDIRS})
    if(IS_DIRECTORY ${SUBDIR})
        get_filename_component(SUBDIR_NAME ${SUBDIR} NAME)
        add_custom_command(TARGET 2drp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SUBDIR}"
            "$<TARGET_FILE_DIR:2drp>/${SUBDIR_NAME}"
        )
    endif()
endforeach()]]

# Copy the contents of the media directory into the bin folder.
add_custom_command(TARGET 2drp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/media/packages/adventure"
		"$<TARGET_FILE_DIR:2drp>/packages/adventure"
)

# Copy the doc/settings.ini file to the destination, but only if it does not already exist.
add_custom_command(TARGET 2drp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying settings.ini if it does not exist at the destination..."
    COMMAND ${CMAKE_COMMAND}
    	-Dsourcefile="${CMAKE_SOURCE_DIR}/doc/settings.ini"
    	-Ddestinationfile="$<TARGET_FILE_DIR:2drp>/settings.ini"
    	-P "${CMAKE_SOURCE_DIR}/build/cmake/copy_if_not_exists.cmake"
)
