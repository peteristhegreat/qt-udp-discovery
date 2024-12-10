cmake_minimum_required(VERSION 3.16)  # Adjust based on your needs
project(Jotto)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic handling of MOC and RCC
set(CMAKE_AUTOMOC ON)   # Enables automatic generation of MOC files for headers with Q_OBJECT
set(CMAKE_AUTORCC ON)   # Enables automatic processing of .qrc resource files

# Find required Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Svg SvgWidgets Network Widgets Multimedia MultimediaWidgets)

# Add plugins for macOS
if(APPLE)
    set(QT_PLUGINS qavfmediaplayer qtaudio_coreaudio qtmedia_audioengine qavfcamera)
endif()

# Define the executable
add_executable(${PROJECT_NAME}
    main.cpp
    server.cpp
    mainstack.cpp
    dictionary.cpp
    letterbutton.cpp
    flowlayout.cpp
    highlighter.cpp
    shuffle.cpp
    overlay.cpp
    slidingstackedwidget.cpp
    # aspectratiosvgwidget.cpp
    globals.cpp
    mainstack_2.cpp
    overlaydialogbox.cpp
    mytime.cpp
    winbox.cpp
)

# Include additional headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add Qt resources
qt6_add_resources(RESOURCES
    qresources.qrc
    word_lists.qrc
    styles.qrc
)

# Add Qt plugins
if(QT_PLUGINS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_PLUGINS})
endif()

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Network
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

# macOS-specific settings
if(APPLE)
    set_source_files_properties(jotto.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(${PROJECT_NAME} PRIVATE jotto.icns)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    set(QMAKE_INFO_PLIST custom.plist)
endif()

# iOS-specific settings
if(IOS)
    set(BUNDLE_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/icon/ios/*.png")
    set_source_files_properties(${BUNDLE_DATA_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
endif()

# Add additional resources
set(OTHER_FILES
    myapp.rc
    style.qss
    android/AndroidManifest.xml
    custom.plist
)

# Android-specific settings
if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
endif()

# Group resources for better IDE organization
source_group("Resources" FILES ${OTHER_FILES})
