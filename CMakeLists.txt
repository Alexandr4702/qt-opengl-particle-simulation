cmake_minimum_required(VERSION 3.16)

project(QtOpenGLParticleSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    OpenGL
    OpenGLWidgets
    Widgets
)

qt_standard_project_setup()

qt_add_executable(Qt_grapth WIN32
    body.cpp
    body.h
    camera.cpp
    camera.h
    gl_game.cpp
    gl_game.h
    main.cpp
    qmainwindows_.cpp
    qmainwindows_.h
    world.cpp
    world.h
    Shaders.qrc
)

target_link_libraries(Qt_grapth PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
)
