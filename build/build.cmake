set(root "${CMAKE_CURRENT_LIST_DIR}/..")

find_package(Vulkan REQUIRED)

###############################################
#library
###############################################
set(PUBLIC_HDRS
    ${root}/inc/Graphics/Engine.h
    ${root}/inc/Graphics/SpriteBasic.h
    ${root}/inc/Graphics/SpriteTemplateBasic.h
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
    ${root}/src/SpriteBasic.cpp
    ${root}/src/SpriteTemplateBasicImpl.h
    ${root}/src/SpriteTemplateBasic.cpp
    ${root}/src/SpriteManagerBasic.h
    ${root}/src/SpriteManagerBasic.cpp
    ${root}/src/TextureSets.h
    ${root}/src/TextureSets.cpp
    ${root}/src/EngineInternal.h
    ${root}/src/Engine.cpp
    ${root}/src/UniformBufferDynamic.h
    ${root}/src/UniformBufferDynamic.cpp
    ${root}/src/VertexBuffer.h
    ${root}/src/VertexBuffer.cpp
    ${root}/src/CommandPool.cpp
    ${root}/src/Commands.cpp
    ${root}/src/Pipeline.h
    ${root}/src/Pipeline.cpp
    ${root}/src/shaders/Basic.h
    ${root}/src/shaders/Basic.cpp
    ${root}/src/shaders/Basic.vert
    ${root}/src/shaders/Basic.frag
    ${root}/src/shaders/basic.crsm
    ${root}/src/types/SNorm.h
    ${root}/src/types/UNorm.h
)

set(BUILD
    ${root}/build/build.cmake
)

add_library(graphics OBJECT  
	${PUBLIC_HDRS} 
	${SRCS} 
	${SRCS_SHADERS} 
	${BUILD}
)
	
settingsCR(graphics)	
createPCH(graphics)	

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
  ${root}/tests/SpriteBasic.cpp
  ${root}/tests/SpriteTemplateBasic.cpp
  ${root}/tests/UNorm.cpp
  ${root}/tests/SNorm.cpp
  ${root}/tests/VertexBuffer.cpp
)

add_executable(graphics_tests
					${SRCS}
)

settingsCR(graphics_tests)	
createPCH(graphics_tests)
					
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

add_dependencies(embed shadercompiler)
add_dependencies(graphics embed shadercompiler)
add_dependencies(graphics_tests TextureProcessor)

add_custom_command(TARGET graphics_tests POST_BUILD        
COMMAND ${CMAKE_COMMAND} -E copy_if_different  
	"${glfw_dll}"
	$<TARGET_FILE_DIR:graphics_tests>)

add_custom_command(OUTPUT ${root}/src/shaders/basic.crsm
    COMMAND $<TARGET_FILE:shadercompiler> -v ${root}/src/shaders/basic.vert -f ${root}/src/shaders/basic.frag -o ${root}/src/shaders/basic.crsm
    DEPENDS ${root}/src/shaders/basic.vert ${root}/src/shaders/basic.frag
)

add_custom_command(OUTPUT ${root}/src/shaders/Basic.h ${root}/src/shaders/Basic.cpp
    COMMAND $<TARGET_FILE:embed> -i ${root}/src/shaders/basic.crsm -o ${root}/src/shaders/Basic
    DEPENDS ${root}/src/shaders/basic.crsm
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/CompletionScreen -o $<TARGET_FILE_DIR:graphics_tests>/CompletionScreen -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/BonusHarrySelect -o $<TARGET_FILE_DIR:graphics_tests>/BonusHarrySelect -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/leaf -o $<TARGET_FILE_DIR:graphics_tests>/leaf -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/brick -o $<TARGET_FILE_DIR:graphics_tests>/brick -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/wood -o $<TARGET_FILE_DIR:graphics_tests>/wood -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/ice -o $<TARGET_FILE_DIR:graphics_tests>/ice -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/diamond -o $<TARGET_FILE_DIR:graphics_tests>/diamond -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/gold -o $<TARGET_FILE_DIR:graphics_tests>/gold -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/m -o $<TARGET_FILE_DIR:graphics_tests>/m -p
)

add_custom_command(TARGET graphics_tests POST_BUILD
    COMMAND $<TARGET_FILE:TextureProcessor> -i ${root}/tests/data/question -o $<TARGET_FILE_DIR:graphics_tests>/question -p
)