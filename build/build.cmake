set(graphics_root "${CMAKE_CURRENT_LIST_DIR}/..")

find_package(Vulkan REQUIRED)

###############################################
#library
###############################################
set(GRAPHICS_PUBLIC_HDRS
    ${graphics_root}/inc/Graphics/Engine.h
)

set(GRAPHICS_SRCS
    ${graphics_root}/src/Commands.h
    ${graphics_root}/src/CommandPool.h
    ${graphics_root}/src/Buffer.h
    ${graphics_root}/src/Image.h
    ${graphics_root}/src/vulkan/EngineInternal.h
    ${graphics_root}/src/vulkan/Engine.cpp
    ${graphics_root}/src/vulkan/Buffer.cpp
    ${graphics_root}/src/vulkan/Image.cpp
    ${graphics_root}/src/vulkan/CommandPool.cpp
    ${graphics_root}/src/vulkan/Commands.cpp
)

set(GRAPHICS_BUILD
    ${graphics_root}/build/build.cmake
)

add_library(graphics OBJECT  
	${GRAPHICS_PUBLIC_HDRS} 
	${GRAPHICS_SRCS} 
	${GRAPHICS_BUILD}
)
	
target_include_directories(graphics PUBLIC
	"${graphics_root}/inc"
)
target_compile_definitions(graphics PRIVATE VK_USE_PLATFORM_WIN32_KHR)
target_include_directories(graphics PRIVATE
	"${graphics_root}/src"
	$ENV{VULKAN_SDK}/include
	Vulkan::Vulkan
)		

target_link_libraries(graphics 
  core
)

source_group("Public Headers" FILES ${GRAPHICS_PUBLIC_HDRS})
source_group("Source" FILES ${GRAPHICS_SRCS})
source_group("Build" FILES ${GRAPHICS_BUILD})
	
				
if(IncludeTests)  
	###############################################
	#unit tests
	###############################################
set(GRAPHICS_TEST_SRCS
    ${graphics_root}/tests/TestFixture.h
    ${graphics_root}/tests/main.cpp
    ${graphics_root}/tests/Engine.cpp
    ${graphics_root}/tests/Buffer.cpp
    ${graphics_root}/tests/Image.cpp
    ${graphics_root}/tests/FrameBuffer.cpp
)
	
	add_executable(graphics_tests
						${GRAPHICS_TEST_SRCS}
	)

	target_include_directories(graphics_tests PRIVATE
		"${graphics_root}/src"
	)	
	target_link_libraries(graphics_tests 
		catch
		fmt
		core
		graphics
		Vulkan::Vulkan 
		glfw
	)

	source_group("Source" FILES ${GRAPHICS_TEST_SRCS})
						
	set_property(TARGET graphics_tests APPEND PROPERTY FOLDER tests)
	
	add_custom_command(TARGET graphics_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${glfw_root}/lib-vc2019/glfw3.dll"
        $<TARGET_FILE_DIR:graphics_tests>)

endif()
