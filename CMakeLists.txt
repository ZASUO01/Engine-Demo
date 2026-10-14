cmake_minimum_required(VERSION 3.20)
project(EngineDemo LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("Engine")

add_executable(${PROJECT_NAME}
    main.cpp
        Source/Data/GameRecipe.cpp
        Source/Data/GameRecipe.h
        Source/Game.cpp
        Source/Game.h
        Source/Core/Actors/Ball.cpp
        Source/Core/Actors/Ball.h
        Source/Core/Cameras/MainCamera.cpp
        Source/Core/Cameras/MainCamera.h
        Source/Core/Actors/Desk.cpp
        Source/Core/Actors/Desk.h
        Source/Core/Actors/Wall.cpp
        Source/Core/Actors/Wall.h
        Source/Core/Actors/Divider.cpp
        Source/Core/Actors/Divider.h
        Source/Core/Actors/Trigger.cpp
        Source/Core/Actors/Trigger.h
        Source/Sound/Crystal.cpp
        Source/Sound/Crystal.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Assets"
        "${CMAKE_CURRENT_BINARY_DIR}/Source/Assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders"
        "${CMAKE_CURRENT_BINARY_DIR}/Source/Shaders"
)

# Flappy Bird: demo minima usando a Engine (um unico arquivo de jogo)
add_executable(FlappyBird Flappy/FlappyBird.cpp)
target_link_libraries(FlappyBird PRIVATE Engine SDL2::SDL2)
add_custom_command(TARGET FlappyBird POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Flappy/Assets"
        "${CMAKE_CURRENT_BINARY_DIR}/Flappy/Assets"
)
