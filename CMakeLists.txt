cmake_minimum_required(VERSION 3.16)
project(ScreenRecognition)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPTONICA REQUIRED lept)

add_executable(screenshot
    main.cpp
    ocr.cpp
    capture.mm
    functions.cpp
)

set_source_files_properties(capture.mm
    PROPERTIES COMPILE_FLAGS "-x objective-c++")

target_include_directories(screenshot PRIVATE
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPTONICA_INCLUDE_DIRS}
    /opt/homebrew/include
)

target_link_directories(screenshot PRIVATE
    /opt/homebrew/lib
)

target_link_libraries(screenshot
    ${TESSERACT_LIBRARIES}
    ${LEPTONICA_LIBRARIES}
    "-framework ScreenCaptureKit"
    "-framework CoreGraphics"
    "-framework CoreImage"
    "-framework CoreMedia"
    "-framework CoreVideo"
    "-framework Foundation"
)
