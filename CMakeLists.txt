cmake_minimum_required(VERSION 3.10)
project(AthensMetroManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SGG library paths
set(SGG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sgg")
set(SGG_3RDPARTY_DIR "${SGG_DIR}/3rdparty")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SGG_DIR}
    ${SGG_3RDPARTY_DIR}/include
    ${SGG_3RDPARTY_DIR}/include/SDL2
    ${SGG_3RDPARTY_DIR}/include/freetype
    /usr/include/jsoncpp
)

# Link directories
link_directories(
    ${SGG_DIR}/lib
)

# Source files
set(SOURCES
    main.cpp
)

# Create executable
add_executable(athens-metro-manager ${SOURCES})

# Link libraries
target_link_libraries(athens-metro-manager
    sgg
    jsoncpp
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework ForceFeedback"
    "-framework Carbon"
)

# On macOS, we need to find SDL2, GLEW, and freetype via Homebrew or system
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
find_library(GLEW_LIBRARY GLEW)
find_library(FREETYPE_LIBRARY freetype)

if(SDL2_LIBRARY)
    target_link_libraries(athens-metro-manager ${SDL2_LIBRARY})
endif()

if(SDL2_MIXER_LIBRARY)
    target_link_libraries(athens-metro-manager ${SDL2_MIXER_LIBRARY})
endif()

if(GLEW_LIBRARY)
    target_link_libraries(athens-metro-manager ${GLEW_LIBRARY})
endif()

if(FREETYPE_LIBRARY)
    target_link_libraries(athens-metro-manager ${FREETYPE_LIBRARY})
endif()
