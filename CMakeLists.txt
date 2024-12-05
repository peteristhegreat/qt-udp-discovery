cmake_minimum_required(VERSION 3.16)  # Adjust based on your needs
project(Jotto)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic handling of MOC and RCC
set(CMAKE_AUTOMOC ON)   # Enables automatic generation of MOC files for headers with Q_OBJECT
set(CMAKE_AUTORCC ON)   # Enables automatic processing of .qrc resource files

# Find required Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Svg SvgWidgets Network Widgets Multimedia)

# Define the executable
add_executable(${PROJECT_NAME}
    main.cpp
    qresources.qrc
    server.cpp
    mainstack.cpp
    dictionary.cpp
    letterbutton.cpp
    flowlayout.cpp
    highlighter.cpp
    shuffle.cpp
    overlay.cpp
    slidingstackedwidget.cpp
    ${RESOURCES}  # Automatically generated from qrc
)

# Include header files for MOC
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add Qt resources (qrc file)
# qt6_add_resources(RESOURCES qresources.qrc)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Svg
    Qt6::Network
    Qt6::SvgWidgets
    Qt6::Widgets
    Qt6::Multimedia
)

# Add the application icon for different platforms
if(WIN32)
    set(RC_FILE myapp.rc)
    target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
elseif(APPLE)
    set(ICON_FILE jotto.icns)
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(${PROJECT_NAME} PRIVATE ${ICON_FILE})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif()

# Add additional resources
set(OTHER_FILES
    myapp.rc
    style.qss
)

# Include additional files (optional)
source_group("Resources" FILES ${OTHER_FILES})
