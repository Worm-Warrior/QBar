cmake_minimum_required(VERSION 3.16)

project(QBar VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt modules
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    playercontrolswidget.cpp
    playercontrolswidget.h
    playercontrolswidget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QBar
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        filebrowserwidget.h filebrowserwidget.cpp filebrowserwidget.ui
        mediaviewwidget.h mediaviewwidget.cpp mediaviewwidget.ui
    )
else()
    add_executable(QBar
        ${PROJECT_SOURCES}
    )
endif()

# Link libraries
target_link_libraries(QBar PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)

# Add root source directory to include path (needed for promoted widgets)
target_include_directories(QBar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Properties for macOS / Windows bundles
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QBar)
endif()
set_target_properties(QBar PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QBar
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Must be last!
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QBar)
endif()

