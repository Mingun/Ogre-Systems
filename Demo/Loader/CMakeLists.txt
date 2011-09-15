cmake_minimum_required(VERSION 2.6)

set(PROJECT Loader)
set(HEADERS OgreConfig.h)
set(SOURCES main.cpp)
set(CORE Core)

project(${PROJECT})

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH lib)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    include)

# add_subdirectory(${CORE} ../${CORE})

# Add some compiler flags
if(MSVC)
    add_definitions(/W4)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_EXE_LINKER_FLAGS -static)
    add_definitions(-Wall -pedantic)
else()
    message("Unknown compiler")
endif()

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})


add_executable(${PROJECT} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES})

# add_dependencies(${PROJECT} ${LIBRARIES})
