cmake_minimum_required(VERSION 3.10)
project(Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets COMPONENTS core base gl REQUIRED)

if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found!")
endif()

include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME} WIN32
    MainApp.cpp
    MainFrame.cpp
    GLCanvas.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} OpenGL::GL)

# Find OpenGL
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenGL::GL)
else()
    message(FATAL_ERROR "OpenGL not found")
endif()

# Copy the icon.png to the build directory so executable can find it
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
