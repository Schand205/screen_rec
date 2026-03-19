cmake_minimum_required(VERSION 3.16)
project(ScreenRecognition)

set(CMAKE_CXX_STANDARD 17)

add_executable(screenshot
    main.cpp      # reines C++
    capture.mm    # nur diese Datei ist Obj-C++
)

set_source_files_properties(capture.mm
    PROPERTIES COMPILE_FLAGS "-x objective-c++")

target_link_libraries(screenshot
    "-framework ScreenCaptureKit"
    "-framework CoreGraphics"
    "-framework CoreImage"
    "-framework CoreMedia"
    "-framework CoreVideo"
    "-framework Foundation"
)
