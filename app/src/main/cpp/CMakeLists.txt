# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("jnidemo20201117")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.



#file(GLOB SOURCE src/main/cpp/*.cpp src/main/cpp/*.h)
#add_library(
#        native-lib
#        SHARED
#        native-lib
#         引入 SOURCE 下的所有源文件
#        ${SOURCE}
#)
#set_target_properties(native-lib PROPERTIES LINKER_LANGUAGE CXX)



#add_library( # Sets the name of the library.
#        native-lib
#
#        # Sets the library as a shared library.
#        SHARED
#
#        # Provides a relative path to your source file(s).
#        native-lib.cpp
#        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library( # Sets the name of the path variable.
#        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
#        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.



# ==================引入外部 so===================
#message("ANDROID_ABI : ${ANDROID_ABI}")
#message("CMAKE_SOURCE_DIR : ${CMAKE_SOURCE_DIR}")
#message("PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")
# external 代表第三方 so - libexternal.so
# SHARED 代表动态库，静态库是 STATIC；
# IMPORTED: 表示是以导入的形式添加进来(预编译库)
#add_library(external SHARED IMPORTED)
#设置 external 的 导入路径(IMPORTED_LOCATION) 属性,不可以使用相对路径
# CMAKE_SOURCE_DIR: 当前cmakelists.txt的路径 （cmake工具内置的）
# android cmake 内置的 ANDROID_ABI :  当前需要编译的cpu架构
#set_target_properties(external PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/x86/libexternal.so)
#set_target_properties(external PROPERTIES LINKER_LANGUAGE CXX)
# ==================引入外部 so end===================


#target_link_libraries( # Specifies the target library.
#        native-lib

        # Links the target library to the log library
        # included in the NDK.
#        ${log-lib})

# Copyright (c) 2014 Spotify AB
#aux_source_directory(. DIR_LIB_SRCS)

#ADD_LIBRARY(native-lib SHARED ${DIR_LIB_SRCS})

#set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/../jniLibs)


#file(GLOB libJniHelpers_SOURCES *.cpp)
#file(GLOB libJniHelpers_HEADERS *.h)
#
#link_directories(${JNI_LIBRARIES})
#add_library(jnidemo20201117 ${libJniHelpers_SOURCES} ${libJniHelpers_HEADERS})
#
#if(${UNIX})
#    set(CMAKE_CXX_FLAGS "-fPIC")
#    set_target_properties(jnidemo20201117 PROPERTIES CMAKE_SHARED_LINKER_FLAGS "-fPIC")
#elseif(${WINDOWS})
#    set(CMAKE_CXX_FLAGS "/MP /EHsc")
#endif(${UNIX})








#设置生成的so动态库最后输出的路径
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})




#生成so动态库

# 查找当前目录下的所有源文件，并保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 生成链接库
ADD_LIBRARY(native-lib SHARED ${DIR_LIB_SRCS})

# 导入当前目录下的所有头文件
include_directories(.)

find_library(log-lib log)

target_link_libraries(native-lib ${log-lib})





