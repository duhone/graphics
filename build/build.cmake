set(graphics_root "${CMAKE_CURRENT_LIST_DIR}/..")

find_package(Vulkan REQUIRED)

###############################################
#library
###############################################
set(GRAPHICS_PUBLIC_HDRS
    ${graphics_root}/inc/Graphics/Engine.h
    ${graphics_root}/inc/Graphics/Sprite.h
    ${graphics_root}/inc/Graphics/SpriteType.h
    ${graphics_root}/inc/Graphics/SpriteTemplate.h
    ${graphics_root}/inc/Graphics/TextureSet.h
)

set(GRAPHICS_SRCS
    ${graphics_root}/src/AssetLoadingThread.h
    ${graphics_root}/src/AssetLoadingThread.cpp
    ${graphics_root}/src/Commands.h
    ${graphics_root}/src/CommandPool.h
    ${graphics_root}/src/Constants.h
    ${graphics_root}/src/DescriptorPool.h
    ${graphics_root}/src/DescriptorPool.cpp
    ${graphics_root}/src/UniformBufferDynamic.h
    ${graphics_root}/src/Sprite.cpp
    ${graphics_root}/src/SpriteTypeImpl.h
    ${graphics_root}/src/SpriteType.cpp
    ${graphics_root}/src/SpriteTemplateImpl.h
    ${graphics_root}/src/SpriteTemplate.cpp
    ${graphics_root}/src/SpriteManager.h
    ${graphics_root}/src/SpriteManager.cpp
    ${graphics_root}/src/TextureSets.h
    ${graphics_root}/src/TextureSets.cpp
    ${graphics_root}/src/vulkan/EngineInternal.h
    ${graphics_root}/src/vulkan/Engine.cpp
    ${graphics_root}/src/vulkan/UniformBufferDynamic.cpp
    ${graphics_root}/src/vulkan/CommandPool.cpp
    ${graphics_root}/src/vulkan/Commands.cpp
    ${graphics_root}/src/Pipeline.h
    ${graphics_root}/src/vulkan/Pipeline.cpp
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

target_link_libraries(graphics PUBLIC
    robinmap
    core
    datacompression
	glm
    function2
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
      ${graphics_root}/tests/UniformBufferDynamic.cpp
      ${graphics_root}/tests/TextureSet.cpp
      ${graphics_root}/tests/FrameBuffer.cpp
      ${graphics_root}/tests/Sprite.cpp
      ${graphics_root}/tests/SpriteType.cpp
      ${graphics_root}/tests/SpriteTemplate.cpp
  )

  set(GRAPHICS_TEST_DATA
      ${graphics_root}/tests/data/simple.vert
      ${graphics_root}/tests/data/simple.frag
      ${graphics_root}/tests/data/simple.crsm
      ${graphics_root}/tests/data/BonusHarrySelect.crtexd
      ${graphics_root}/tests/data/CompletionScreen.crtexd
  )
	
	add_executable(graphics_tests
						${GRAPHICS_TEST_SRCS}
						${GRAPHICS_TEST_DATA}
	)

	target_include_directories(graphics_tests PRIVATE
		"${graphics_root}/src"
	)	
	target_link_libraries(graphics_tests 
		catch
		fmt
        zstd
		core
        datacompression
        platform
		graphics
		Vulkan::Vulkan 
		glfw
	)

	source_group("Source" FILES ${GRAPHICS_TEST_SRCS})
	source_group("Data" FILES ${GRAPHICS_TEST_DATA})
						
	set_property(TARGET graphics_tests APPEND PROPERTY FOLDER tests)
	
	add_custom_command(TARGET graphics_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        "${glfw_root}/lib-vc2019/glfw3.dll"
        $<TARGET_FILE_DIR:graphics_tests>)
        
	add_custom_command(TARGET graphics_tests POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        ${GRAPHICS_TEST_DATA}
        $<TARGET_FILE_DIR:graphics_tests>)

endif()
