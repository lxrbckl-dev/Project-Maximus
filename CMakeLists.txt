cmake_minimum_required(VERSION 3.21)
project(maximus VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Homebrew Qt 6 on Apple Silicon lives at /opt/homebrew/opt/qt@6.
# If the caller hasn't already set CMAKE_PREFIX_PATH, append it as a hint
# so the build works out-of-the-box on macOS without extra flags.
if(APPLE AND EXISTS "/opt/homebrew/opt/qt@6")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6")
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick Qml)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(maximus
    src/main.cpp
    src/vehicle_data.h
    src/vehicle_data.cpp
    src/i_data_source.h
    src/synthetic_data_source.h
    src/synthetic_data_source.cpp
)

qt_add_qml_module(maximus
    URI Maximus
    VERSION 1.0
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/Maximus"
    QML_FILES
        qml/Main.qml
        qml/Dashboard.qml
)

target_include_directories(maximus PRIVATE src)

target_link_libraries(maximus
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Qml
)

# macOS: treat the app as a console app during development
# (no .app bundle — run directly from the build dir).
set_target_properties(maximus PROPERTIES
    MACOSX_BUNDLE FALSE
)
