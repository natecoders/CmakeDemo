# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

file(GLOB SOURCE ./*.c ./*.cpp )

add_library(
        cjson

        STATIC

        ${SOURCE}
        )


