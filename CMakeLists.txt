cmake_minimum_required(VERSION 3.16)
project(Brenda)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# --- Dependencies ---

# Find system-installed SDL3 and SDL3_ttf
# This requires the development packages (e.g., libsdl3-dev, libsdl3-ttf-dev) to be installed
find_package(SDL3 REQUIRED)

# Use pkg-config for SDL3_ttf
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)

include(FetchContent)

# --- nlohmann_json (FetchContent is fine for this one) ---
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# --- cpr (FetchContent is fine for this one) ---
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

# Add all source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
    "src/ECS/*.cpp"
    "src/ECS/*.h"
    "src/ECS/components/*.h"
)

# Create the executable
add_executable(Brenda ${SOURCES})

# Add debug flags specifically for this target
target_compile_options(Brenda PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -g3                      # Maximum debug information
    -ggdb                    # GDB-specific debug info
    -O0                      # No optimization for debugging
    -fno-omit-frame-pointer  # Keep frame pointers
    -DDEBUG                  # Define DEBUG macro
)

# Set debug-specific link flags
target_link_options(Brenda PRIVATE
    -g                       # Include debug info in executable
    -rdynamic                # Export symbols for better stack traces
)

# Include directories
target_include_directories(Brenda PRIVATE
    src
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(Brenda 
    ${SDL3_LIBRARIES}
    ${SDL3_TTF_LIBRARIES}
    nlohmann_json::nlohmann_json
    cpr::cpr
)

# Add compiler flags from pkg-config
target_compile_options(Brenda PRIVATE ${SDL3_CFLAGS_OTHER} ${SDL3_TTF_CFLAGS_OTHER})
target_link_options(Brenda PRIVATE ${SDL3_LDFLAGS_OTHER} ${SDL3_TTF_LDFLAGS_OTHER})

# This command attaches a build step to the 'brenda' target.
# It runs every time you run 'make', after the executable is built.

add_custom_command(
    TARGET Brenda POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/entities.json"
            "$<TARGET_FILE_DIR:Brenda>/entities.json"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/environment.json"
            "$<TARGET_FILE_DIR:Brenda>/environment.json"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/config.json"
            "$<TARGET_FILE_DIR:Brenda>/config.json"
    COMMENT "Copying asset files to build directory..."
)

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

    add_custom_target(doc_doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
