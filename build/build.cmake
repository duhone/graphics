include_directories("${CMAKE_CURRENT_LIST_DIR}/../inc")


###############################################
#library
###############################################
set(GRAPHICS_PUBLIC_HDRS
    ${CMAKE_CURRENT_LIST_DIR}/../inc/Graphics/Graphics.h
)

set(GRAPHICS_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/../src/vulkan/Graphics.cpp
)

set(GRAPHICS_BUILD
    ${CMAKE_CURRENT_LIST_DIR}/../build/build.cmake
)

add_library(graphics OBJECT  ${GRAPHICS_PUBLIC_HDRS} ${GRAPHICS_SRCS} ${GRAPHICS_BUILD})
						
source_group("Public Headers" FILES ${GRAPHICS_PUBLIC_HDRS})
source_group("Source" FILES ${CMAKE_CURRENT_LIST_DIR})
source_group("Build" FILES ${GRAPHICS_BUILD})
	
				
if(IncludeTests)  
	###############################################
	#unit tests
	###############################################
set(GRAPHICS_TEST_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/../tests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tests/Graphics.cpp
)
	
	add_executable(graphics_tests  $<TARGET_OBJECTS:graphics>
						$<TARGET_OBJECTS:core>
						$<TARGET_OBJECTS:math>
						${GRAPHICS_TEST_SRCS})
						
	source_group("Source" FILES ${GRAPHICS_TEST_SRCS})
						
	set_property(TARGET graphics_tests APPEND PROPERTY FOLDER tests)

endif()
