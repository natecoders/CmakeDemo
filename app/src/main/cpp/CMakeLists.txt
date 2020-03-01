# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
#add_library( # Sets the name of the library.
#        one
#
#        # Sets the library as a shared library.
#        STATIC
#
#        # Provides a relative path to your source file(s).
#        one/one.cpp
#        one/two.cpp
#        )




ADD_SUBDIRECTORY(one)
ADD_SUBDIRECTORY(two)
#ADD_SUBDIRECTORY(cjson)

#引入头文件，主要cjson的头文件
include_directories(include)

#5.对应的库
link_libraries(${PROJECT_SOURCE_DIR}/../jniLibs/Debug/${ANDROID_ABI}/libcjson.a)

##引入已经编译好的so
#add_library(
#        cjson
#        SHARED
#        IMPORTED
#)
#
## 这是固定的写法，用来描述已经编译好的so所以在的位置
#set_target_properties( # Specifies the target library.
#        cjson
#
#        # Specifies the parameter you want to define.
#        PROPERTIES IMPORTED_LOCATION
#
#        # Provides the path to the library you want to import.
#        ${PROJECT_SOURCE_DIR}/../jniLibs/Debug/${ANDROID_ABI}/libcjson.a )


add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib

        one

#        cjson

        two

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})