cmake_minimum_required(VERSION 3.16)
project(Jotto LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt autogen
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)  # harmless even if you don't use .ui

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Svg SvgWidgets Network Widgets Multimedia MultimediaWidgets
)

# Executable
qt_add_executable(${PROJECT_NAME}
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
    globals.cpp
    mainstack_2.cpp
    overlaydialogbox.cpp
    mytime.cpp
    winbox.cpp
    qresources.qrc
    word_lists.qrc
    styles.qrc

)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

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

# ---- Windows: resource + deploy Qt рядом с .exe ----
if(WIN32)
    # Optional: app icon/version resources
    set(RC_FILE myapp.rc)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${RC_FILE}")
        target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
    endif()

    # Find windeployqt (best effort)
    # 1) If Qt was found properly, Qt's bin dir is usually in Qt6_DIR/../../bin
    get_target_property(_qmake_exe Qt6::qmake IMPORTED_LOCATION)
    if(_qmake_exe)
        get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)
    endif()

    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt.exe
        HINTS "${_qt_bin_dir}"
    )

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-translations
                    --no-opengl-sw
                    "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Running windeployqt to deploy Qt runtime and plugins"
        )
    else()
        message(WARNING "windeployqt not found. Qt plugins (e.g. platforms/qwindows.dll) will NOT be deployed automatically.")
    endif()
endif()

# ---- macOS bundle bits ----
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/jotto.icns")
        set_source_files_properties(jotto.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(${PROJECT_NAME} PRIVATE jotto.icns)
    endif()

    # NOTE: CMAKE doesn't use QMAKE_INFO_PLIST; set MACOSX_BUNDLE_INFO_PLIST instead.
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/custom.plist")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/custom.plist"
        )
    endif()
endif()

# ---- iOS ----
if(IOS)
    file(GLOB BUNDLE_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/icon/ios/*.png")
    if(BUNDLE_DATA_FILES)
        set_source_files_properties(${BUNDLE_DATA_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(${PROJECT_NAME} PRIVATE ${BUNDLE_DATA_FILES})
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
    )
endif()

# ---- Android ----
qt_finalize_executable(${PROJECT_NAME})
if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
endif()

if(WIN32)
  target_sources(${PROJECT_NAME} PRIVATE myapp.rc)
endif()

if(ANDROID)
  target_sources(${PROJECT_NAME} PRIVATE android/AndroidManifest.xml)
endif()

if(APPLE)
  target_sources(${PROJECT_NAME} PRIVATE custom.plist)
endif()

target_sources(${PROJECT_NAME} PRIVATE style.qss)


# # IDE grouping (optional)
# set(OTHER_FILES
#     myapp.rc
#     style.qss
#     android/AndroidManifest.xml
#     custom.plist
# )
# source_group("Resources" FILES ${OTHER_FILES})
