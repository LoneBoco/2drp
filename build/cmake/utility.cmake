function(set_default_compiler_options target)
	# C++23 mode.
	target_compile_features(${target} PUBLIC cxx_std_23)
	set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
	if(MSVC)
		if(MSVC_VERSION GREATER_EQUAL 1910)
			target_compile_options(${target} PUBLIC "/permissive-")
		endif()
		if(MSVC_VERSION GREATER_EQUAL 1914)
			target_compile_options(${target} PUBLIC "/Zc:__cplusplus")
		endif()
		if(MSVC_VERSION GREATER_EQUAL 1925)
			target_compile_options(${target} PUBLIC "/Zc:preprocessor")
		endif()
	endif()

	# Compiler options.
	set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	if(MSVC)
		target_compile_options(${target} PUBLIC
			"$<$<CONFIG:Release>:/guard:cf>"		# Control Flow Guard
			"$<$<CONFIG:Release>:/Qspectre>"		# Spectre Mitigation
		)
	endif()

	# If windows, set the standard defines.
	if(WIN32)
		target_compile_definitions(${target} PUBLIC _WIN32 WIN32 _WINDOWS NOMINMAX)

		# If 64-bit windows...
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			target_compile_definitions(${target} PUBLIC _WIN64 WIN64)
		endif()
	endif()
endfunction()
