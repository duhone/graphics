set(root "${CMAKE_CURRENT_LIST_DIR}/..")

find_package(Vulkan REQUIRED)

###############################################
#library
###############################################
set(PUBLIC_HDRS
    ${root}/inc/Graphics/Engine.h
    ${root}/inc/Graphics/Sprite.h
    ${root}/inc/Graphics/SpriteType.h
    ${root}/inc/Graphics/SpriteTemplate.h
    ${root}/inc/Graphics/TextureSet.h
)

set(SRCS
    ${root}/src/AssetLoadingThread.h
    ${root}/src/AssetLoadingThread.cpp
    ${root}/src/Commands.h
    ${root}/src/CommandPool.h
    ${root}/src/Constants.h
    ${root}/src/DescriptorPool.h
    ${root}/src/DescriptorPool.cpp
    ${root}/src/UniformBufferDynamic.h
    ${root}/src/Sprite.cpp
    ${root}/src/SpriteTypeImpl.h
    ${root}/src/SpriteType.cpp
    ${root}/src/SpriteTemplateImpl.h
    ${root}/src/SpriteTemplate.cpp
    ${root}/src/SpriteManager.h
    ${root}/src/SpriteManager.cpp
    ${root}/src/TextureSets.h
    ${root}/src/TextureSets.cpp
    ${root}/src/vulkan/EngineInternal.h
    ${root}/src/vulkan/Engine.cpp
    ${root}/src/vulkan/UniformBufferDynamic.cpp
    ${root}/src/vulkan/CommandPool.cpp
    ${root}/src/vulkan/Commands.cpp
    ${root}/src/Pipeline.h
    ${root}/src/vulkan/Pipeline.cpp
)

set(BUILD
    ${root}/build/build.cmake
)

add_library(graphics OBJECT  
	${PUBLIC_HDRS} 
	${SRCS} 
	${BUILD}
)
	
settingsCR(graphics)	
			
target_precompile_headers(graphics PRIVATE 
	<3rdParty/robinmap.h>
	<3rdParty/glm.h>
	<3rdParty/function2.h>
	<3rdParty/fmt.h>
)

target_include_directories(graphics PUBLIC
	"${root}/inc"
)
target_compile_definitions(graphics PRIVATE VK_USE_PLATFORM_WIN32_KHR)
target_include_directories(graphics PRIVATE
	"${root}/src"
	$ENV{VULKAN_SDK}/include
	Vulkan::Vulkan
)		

target_link_libraries(graphics PUBLIC
    doctest
    robinmap
    core
    datacompression
	glm
    function2
)
	
				
###############################################
#unit tests
###############################################
set(SRCS
  ${root}/tests/TestFixture.h
  ${root}/tests/main.cpp
  ${root}/tests/Engine.cpp
  ${root}/tests/UniformBufferDynamic.cpp
  ${root}/tests/TextureSet.cpp
  ${root}/tests/FrameBuffer.cpp
  ${root}/tests/Sprite.cpp
  ${root}/tests/SpriteType.cpp
  ${root}/tests/SpriteTemplate.cpp
)

set(TEST_DATA
  ${root}/tests/data/simple.vert
  ${root}/tests/data/simple.frag
  ${root}/tests/data/simple.crsm
  ${root}/tests/data/BonusHarrySelect.crtexd
  ${root}/tests/data/CompletionScreen.crtexd
)

add_executable(graphics_tests
					${SRCS}
					${TEST_DATA}
)

settingsCR(graphics_tests)
					
set_property(TARGET graphics_tests APPEND PROPERTY FOLDER tests)

target_include_directories(graphics_tests PRIVATE
	"${root}/src"
)	

target_link_libraries(graphics_tests 
	doctest
	fmt
	zstd
	core
	datacompression
	platform
	graphics
	Vulkan::Vulkan 
	glfw
)

add_custom_command(TARGET graphics_tests POST_BUILD        
COMMAND ${CMAKE_COMMAND} -E copy_if_different  
	"${glfw_dll}"
	$<TARGET_FILE_DIR:graphics_tests>)
	
add_custom_command(TARGET graphics_tests POST_BUILD        
COMMAND ${CMAKE_COMMAND} -E copy_if_different  
	${TEST_DATA}
	$<TARGET_FILE_DIR:graphics_tests>)

