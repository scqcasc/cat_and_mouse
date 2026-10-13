cmake_minimum_required(VERSION 3.20)
project(VulkanSDLGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define our executable target explicitly
add_executable(VulkanSDLGame src/main.cpp src/Menu.h src/Menu.cpp src/ControlsMenu.h src/ControlsMenu.cpp)

if(WIN32)
    # --- WINDOWS CROSS-COMPILING CONFIGURATION ---
    message(STATUS "Building Windows target via MinGW...")

    set(SDL3_WIN64_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/sdl3-win64")
    set(VULKAN_WIN64_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/vulkan-win64")

    # Include Headers
    target_include_directories(VulkanSDLGame PRIVATE 
        "${SDL3_WIN64_DIR}/include"
        "${VULKAN_WIN64_DIR}/include"
    )

    # Link Libraries
    target_link_libraries(VulkanSDLGame PRIVATE 
        "${SDL3_WIN64_DIR}/lib/libSDL3.dll.a"
        "-Wl,--allow-shlib-undefined"
    )

    # Prevent a command prompt window popping up AND statically link runtimes
    target_link_options(VulkanSDLGame PRIVATE 
        "-mwindows"
        "-static-libgcc"
        "-static-libstdc++"
        "-static"
    )

    # Automation: Copy the Windows SDL3.dll next to the built .exe
    add_custom_command(TARGET VulkanSDLGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_WIN64_DIR}/bin/SDL3.dll"
        $<TARGET_FILE_DIR:VulkanSDLGame>
    )

else()
    # --- NATIVE LINUX CONFIGURATION ---
    message(STATUS "Building native Linux target...")

    find_package(SDL3 REQUIRED)
    find_package(Vulkan REQUIRED)

    target_link_libraries(VulkanSDLGame PRIVATE 
        SDL3::SDL3 
        Vulkan::Vulkan
    )
endif()
