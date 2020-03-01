
# 背景
这个文档主要记录一下Cmake 在使用过程中一些细节问题，方面一以后查阅问题

## Cmake 介绍
什么是CMake

- 在Android Studio2.2和以上版本，构建原生库的默认工具是CMake
- CMake是一个跨平台的构建工具，可以使用简洁的语句来描述所有平台的安装(编译过程)。能够输出各种各样的makefile或者project文件。CMake并不直接构建出最终的软件，而是产生其他工具脚本比如makefile，然后在依据这个工具的构建方式使用。
- CMake是一个比make更高级的编译配置工具，它可以根据不同的平台，不同的编译器，生成相应的makefile或者vcproj项目，从而达到跨平台的目的。Android Studio利用CMake生成的是ninja。ninja是一个小型的关注速度的构建系统。我们一般不需要关心ninja的脚本，只需要知道怎么配置CMake就行了
- CMake是一个跨平台的支持产出各种不同的构建建脚本的一个工具。
 

### CMake源文件

- CMake源文件包含命令、注释、空格和换行
- 已CMakeLists.txt命名或者以.cmake为扩展名
- 可以通过add_subdirectory()命令吧子目录的CMake源文件添加进来
- CMake源文件中所有有效的语句都是命令，可以是内置命令或者自定义的函数或宏命令


### CMake的注释
单行注释使用 # ,多行注释使用#[[]]

```
#单行注释
#[[
多行注释多行注释
]]
```

### CMake变量

CMake中所有变量都是string类型。可以使用set()和unset()命令来生命或者移除一个变量

变量的引用使用${变量名}

变量名是大小写敏感的
 > 想要看到message命令打印的信息，build工程然后在路径app\externalNativeBuild\cmake\debug\armeabi-v7a\cmake_build_output.txt.txt中就能看到
 
 
```shell
#声明变量
set(name 123)
#引用变量 message是打印命令
message("name = ${name}")

```

### CMake列表

- 列表也是字符串，可以把列表看成一个特殊的变量，它有多个值
- 语法格式：set(列表名 值1 值2…)
- 引用：${列表名}


```
#声明列表
set(list_var 1 2 3 4 5 )
#或者
set(list_1 "1;2;3;4;5")
#打印
message("list_var=${list_var}")

```

## CmakeList.txt 文件配置详解
只要通过Android Studio 创建一个C++工程会默认创建一个CmakeList.txt 

```
cmake_minimum_required(VERSION 3.4.1)

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
```
上面是一个简单的CmakeList文件，如果第一次看见这个txt文件可能对上面一脸懵逼，下面一点点分析一下这个配置文件
### cmake_minimum_required(VERSION 3.4.1)
指定cmake支持的最低版本

### add_library 添加一个库或者导入预编译的库添加一个库
- 添加一个库文件，名称为 native-lib
- 指定STATIC,SHSRED,MODULE参数来指定库的类型，STATIC:静态库；SHARED:动态库；MODULE:在使用dyld的系统中有效，如果不支持dyld等同于SHARED
- EXCLUDE_FORM_ALL:表示该库不会被默认构建，可选项目
- sorece1 sorece2….sourceN用来指定库的源文件


```
add_library(<name> [STATIC|SHARED|MODULE]
[EXCLUDE_FROM_ALL] source1 source2...sourceN)
```

### target_link_libraries 将若干个文件链接到目标库文件，链接的顺序应该符合gcc链接顺序规则。

```
target_link_libraries(<name> lib1 lib2 lib3)

#如果出现互相依赖的静态库，CMake会允许依赖图中包含循环依赖比如：
add_library(A STATIC a.c)
add_library(B STATIC b.c)
target_link_libraries(A B)
target_link_libraries(B A)
add_executable(main main.c)
target_link_libraries(main A)
```
被链接的库放在依赖它的库的后面，即如果上面的命令中lib1依赖于lib2,lib2又依赖于lib3，则在前面的命令中，必须严格按照lib1 lib2 lib3的顺序排列否则会报错

### find_library 默认会到 skd的ndk路径下面去找比如：sdk\ndk-bundle\platforms\android-28\arch-arm\usr\lib
find_library( log-lib log) log-lib是起的别名字，库真正的名字是log

## CmakeList 使用
### 编译动态库
我们先看下目录结构

```c++
├── cpp
│   ├── CMakeLists.txt
│   ├── native-lib.cpp
│   └── one
│       ├── one.cpp
│       └── one.h
```
我们重点关注一下one 文件夹下的内容，就一个定义一个求和的方法

```c++
# one.h
#ifndef CMAKEDEMO_ONE_H
#define CMAKEDEMO_ONE_H

int sum(int a, int b);

#endif //CMAKEDEMO_ONE_H


# one.c
#include "one.h"

int sum(int a, int b) {
    return a + b;
}
```
native-lib.cpp 使用到了one.h 文件

```
#include <jni.h>
#include <string>
#include "one/one.h"
#include <android/log.h>

#define LOG_TAG    "nate"
#define LOGV(...) __android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR , LOG_TAG, __VA_ARGS__)
extern "C" JNIEXPORT jstring JNICALL
Java_com_nate_cmakedemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    int result = sum(2, 3);
    LOGE("result= %d", result);
    return env->NewStringUTF(hello.c_str());
}

```
逻辑很简单，这里面就是尝试涌cmake编译one.so,这是个动态so
我们看一下完成CMakeLists文件

```
cmake_minimum_required(VERSION 3.4.1)
add_library(
        one
        SHARED
        one/one.cpp)

add_library( 
        native-lib
        SHARED
        native-lib.cpp)

find_library( 
        log-lib
        log)
        
target_link_libraries(
        native-lib
        one
        ${log-lib})
```
这个地方首先编译出one.so,因为native-lib.so 用到了sum方法，所以target_link_libraries 要配置依赖关 native-lib依赖one

### 编译静态库
还是上面例子，我们把one.so变成静态库，首先先修改CMakeLists.txt文件

```
cmake_minimum_required(VERSION 3.4.1)
add_library(
        one
        STATIC
        one/one.cpp)

add_library( 
        native-lib
        SHARED
        native-lib.cpp)

find_library( 
        log-lib
        log)
        
target_link_libraries(
        native-lib
        one
        ${log-lib})
```
编译运行，没有任何问题，但是怎么证明one编译成静态库呢，我们去build的目录看一下

![image](http://note.youdao.com/yws/res/3827/6F3459C07BEF4391A0C1519258A82E7A)
.a 文件就是表明这是一个静态库

### 编译子文件夹
还是以上面的例子来讲解吧，one目录下只有一个.c 文件，但是在实际的开发过程中一个目录下可定不止一个c文件，那么如何有多个c文件需要如何配置呢？

解法有很多种类
- 最笨的方法是把用到c文件都配置到CMakeLists文件中
- 通过 aux_source_directory，这个方法可以把一个目录下的文件都编译进来，但是如果该目录下还有子目录就可以了
- 通过file方式，file方式可以把一个目录下的所以文件都编译进来，包括子目录的文件这个需要特殊方式指定才行
- 通过ADD_SUBDIRECTORY 方式，这种方式是在子目录下配置自己的CMakeLists文件，子目录的编译由自己的CMakeLists文件配置，这种方式比较好，因为他有自己的编译文件不影响到其他人

#### 通过挨个配置要编译的文件形式实现
我们在one目录下增加个c文件two.cpp文件，内容和one文件基本一致

```C++
# two.h
#ifndef CMAKEDEMO_TWO_H
#define CMAKEDEMO_TWO_H

int add(int a,int b);

#endif //CMAKEDEMO_TWO_H

# two.cpp
#include "two.h"

int add(int a, int b) {
    return a + b;
}

```
现在需要在native-lib文件中调用add 方法

```C++
#include <jni.h>
#include <string>
#include "one/one.h"
#include "one/two.h"
#include <android/log.h>

#define LOG_TAG    "nate"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR , LOG_TAG, __VA_ARGS__)
extern "C" JNIEXPORT jstring JNICALL
Java_com_nate_cmakedemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    int result = sum(2, 3);
    LOGE("result= %d", result);

    result = add(2, 3);
    LOGE("result= %d", result);
    return env->NewStringUTF(hello.c_str());
}

```
CMakeLists.txt
```
cmake_minimum_required(VERSION 3.4.1)
add_library(
        one
        STATIC
        one/one.cpp
        one/two.cpp
        )

add_library( 
        native-lib
        SHARED
        native-lib.cpp)

find_library( 
        log-lib
        log)
        
target_link_libraries(
        native-lib
        one
        ${log-lib})
```
看见没有这种方式就是你用到哪个文件就在CMakeLists文件中配置，这种情况只适合c文件不多的情况

#### 通过file方式实现
通过这种方式我们需要对CMakeLists文件进行大量修改

```
cmake_minimum_required(VERSION 3.4.1)

file(GLOB SOURCE ./one/*.c ./one/*.cpp )

add_library(
        native-lib
        SHARED
        ${SOURCE}
        native-lib.cpp)

find_library(
        log-lib
        log)
        
target_link_libraries(
        native-lib
        ${log-lib})
```
file(GLOB SOURCE ./one/*.c ./one/*.cpp ) 这行配置是把one目录下的所有.c和.cpp文件赋值给变量 SOURCE，然后在add_library配置${SOURCE}，注意file 还可以编译指定目录下的子目录，这不过这里我们还没用到


#### 通过aux_source_directory
aux_source_directory 查找在某个路径下的所有源文件。
```
aux_source_directory(< dir > < variable >)
```

```
cmake_minimum_required(VERSION 3.4.1)

aux_source_directory(./one SOURCE)

add_library(
        native-lib
        SHARED
        ${SOURCE}
        native-lib.cpp)

find_library(
        log-lib
        log)
        
target_link_libraries(
        native-lib
        ${log-lib})
```
和上面唯一的区别就是这一句aux_source_directory(./one SOURCE)，什么意思呢？
就是把one目录下的所以文件赋值给变量SOURCE，**注意如果one目下有子目录则子目录下的文件是编译不进度的**

#### 通过ADD_SUBDIRECTORY 方式实现
通过ADD_SUBDIRECTORY方式一般比较适合编译第三方库，比如说JSC哈哈

目录结构
```
├── cpp
│   ├── CMakeLists.txt
│   ├── native-lib.cpp
│   └── one
│       ├── CMakeLists.txt
│       ├── one.cpp
│       ├── one.h
│       ├── two.cpp
│       └── two.h

```

我们先看一下one目录下的CMakeLists.txt文件如何配置吧

```
cmake_minimum_required(VERSION 3.4.1)

file(GLOB SOURCE ./*.c ./*.cpp )

add_library(
        one

        SHARED

        ${SOURCE}
        )

target_link_libraries(
        one
        )
```
这个地方是one 编译成动态链接库
我们看一下主的CMakeLists文件配置是什么样子的
```
cmake_minimum_required(VERSION 3.4.1)

ADD_SUBDIRECTORY(one)

add_library(
        native-lib
        SHARED
        native-lib.cpp)

find_library(
        log-lib
        log)
        
target_link_libraries(
        native-lib
        one
        ${log-lib})
```
最关键的地方是ADD_SUBDIRECTORY(one)，查询CMake 官方文档 可以知道这条命令的作用是为构建添加一个子路径。子路径中的 CMakeLists.txt 也会被执行。

### 编译多个so
这个问题也没什么难度，我之前的one.so其实就是在一个项目中编译的多个so，现在我们调整一下我们c文件代码

```
├── cpp
│   ├── CMakeLists.txt
│   ├── native-lib.cpp
│   ├── one
│   │   ├── CMakeLists.txt
│   │   ├── one.cpp
│   │   └── one.h
│   └── two
│       ├── CMakeLists.txt
│       ├── two.cpp
│       └── two.h

```
我们把之前的two.cpp文件拆分到另外一个目录下，然后把one文件和two文件中的文件分别编译成one.so 和two.so
在two文件夹下增加CMakeLists文件

```
cmake_minimum_required(VERSION 3.4.1)

file(GLOB SOURCE ./*.c ./*.cpp )

add_library(
        two

        SHARED

        ${SOURCE}
        )

target_link_libraries(
        two
        )
```
回到主CMakeLists文件

```
cmake_minimum_required(VERSION 3.4.1)

ADD_SUBDIRECTORY(one)
ADD_SUBDIRECTORY(two)

add_library(
        native-lib
        SHARED
        native-lib.cpp)

find_library(
        log-lib
        log)
        
target_link_libraries(
        native-lib
        one
        two
        ${log-lib})
```
通过ADD_SUBDIRECTORY方式增加要编译的子目录，我们看一下编译后的产物

```
├── libnative-lib.so
├── libone.so
└── libtwo.so
```


### 集成第三方so
这个地方首先我们需要一个第三方库，暂时我们用cJson这个开源库，我们首先需要获取这个cJson的so文件，我们先编一个cJson的静态库
我们先看一下这个目录的结构
```
├── cjson
│   ├── CMakeLists.txt
│   ├── cJSON.c
│   ├── cJSON.h
│   ├── cJSON_Utils.c
│   └── cJSON_Utils.h

```
然后看一下这个目录下的CMakeLists.txt 如何配置

```
# 静态文件输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
# so文件输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

file(GLOB SOURCE ./*.c ./*.cpp )

add_library(
        cjson

        STATIC

        ${SOURCE}
        )

```
然后在主的CMakeLists文件添加子目录编译

```
cmake_minimum_required(VERSION 3.4.1)

ADD_SUBDIRECTORY(one)
ADD_SUBDIRECTORY(two)
ADD_SUBDIRECTORY(cjson)

add_library(
        native-lib

        SHARED

        native-lib.cpp)

find_library(
        log-lib

        log)

target_link_libraries(
        native-lib

        one

        two

        cjson

        ${log-lib})
```

然后编译生成这个静态的.a文件

```
├── jniLibs
│   └── Debug
│       ├── arm64-v8a
│       │   └── libcjson.a
│       └── armeabi-v7a
│           └── libcjson.a

```
下面我们假设libcjson.a这个是我们从外部引入的第三方so，我们在项目中如何使用呢？

在主的CMakeLists文件中增加如下配置信息

```
ADD_SUBDIRECTORY(one)
ADD_SUBDIRECTORY(two)
#ADD_SUBDIRECTORY(cjson)

#引入头文件，主要cjson的头文件
include_directories(include)

#引入已经编译好的so
add_library(
        cjson
        SHARED
        IMPORTED
)

# 这是固定的写法，用来描述已经编译好的so所以在的位置
set_target_properties( # Specifies the target library.
        cjson

        # Specifies the parameter you want to define.
        PROPERTIES IMPORTED_LOCATION

        # Provides the path to the library you want to import.
        ${PROJECT_SOURCE_DIR}/../jniLibs/Debug/${ANDROID_ABI}/libcjson.a )
        

target_link_libraries(
        native-lib

        one
# 这个地方是使用到的地方
        cjson 

        two

        ${log-lib})        
```
修改native-lib.cpp文件我们在这里增加获取cjson版本号码的能力

```
#include <jni.h>
#include <string>
#include "one/one.h"
#include "two/two.h"
#include <android/log.h>

#include "./include/cJSON.h"

#define LOG_TAG    "nate"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR , LOG_TAG, __VA_ARGS__)
extern "C" JNIEXPORT jstring JNICALL
Java_com_nate_cmakedemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    int result = sum(2, 3);
    LOGE("result= %d", result);

    result = add(2, 3);
    LOGE("result= %d", result);

    //获取版本号码
    const char *version = cJSON_Version();
    LOGE("cjson version= %s", version);

    return env->NewStringUTF(hello.c_str());
}

```
日志输出：


```
2020-03-01 11:03:22.150 28271-28271/? E/nate: result= 5
2020-03-01 11:03:22.150 28271-28271/? E/nate: result= -1
2020-03-01 11:03:22.150 28271-28271/? E/nate: cjson version= 1.7.10
```
总体上配置信息还是比较麻烦，一般的时候我们也用不到这种方式，但是如果有的人就提供so不提供源文件，那只能用这种方式了

还有一种更简单的方式实现

可以通过link_directories，指定依赖的so位置来使用第三方so

该指令的作用主要是指定要链接的库文件的路径，该指令有时候不一定需要。因为find_package和find_library指令可以得到库文件的绝对路径。不过你自己写的动态库文件放在自己新建的目录下时，可以用该指令指定该目录的路径以便工程能够找到

例子：

```
link_directories(
    lib
)
```
只需要在主的CMakeLists文件中增加这两个配置就可以了

```
#引入头文件，主要cjson的头文件
include_directories(include)

#5.对应的库
link_libraries(${PROJECT_SOURCE_DIR}/../jniLibs/Debug/${ANDROID_ABI}/libcjson.a)

```
和第一种方式对比，很显然这种方式很简单

第一种方式的配置很复杂

```
#引入已经编译好的so
add_library(
        cjson
        SHARED
        IMPORTED
)

# 这是固定的写法，用来描述已经编译好的so所以在的位置
set_target_properties( # Specifies the target library.
        cjson

        # Specifies the parameter you want to define.
        PROPERTIES IMPORTED_LOCATION

        # Provides the path to the library you want to import.
        ${PROJECT_SOURCE_DIR}/../jniLibs/Debug/${ANDROID_ABI}/libcjson.a )
        
# 这个地方是使用到的地方
        cjson 

        two

        ${log-lib}) 
```




## CmakeList 其他语法
### 预定义的常量
cmake中预设了一堆变量
系统信息

- CMAKE_MAJOR_VERSION cmake主版本号,如2.8.6中的2
- CMAKE_MINOR_VERSION cmake次版本号,如2.8.6中的8
- CMAKE_PATCH_VERSION cmake补丁等级,如2.8.6中的6
- CMAKE_SYSTEM 系统名称,例如Linux-2.6.22
- CAMKE_SYSTEM_NAME 不包含版本的系统名,如Linux
- CMAKE_SYSTEM_VERSION 系统版本,如2.6.22
- CMAKE_SYSTEM_PROCESSOR 处理器名称,如i686
- UNIX 在所有的类UNIX平台为TRUE,包括OS X和cygwin
- WIN32 在所有的win32平台为TRUE,包括cygwin

开关选项

- BUILD_SHARED_LIBS 控制默认的库编译方式。如果未进行设置,使用ADD_LIBRARY时又没有指定库类型,默认编译生成的库都是静态库 （可在t3中稍加修改进行验证）
- CMAKE_C_FLAGS 设置C编译选项
- CMAKE_CXX_FLAGS 设置C++编译选项

其他变量

- PROJECT_SOURCE_DIR **工程的根目录**
- PROJECT_BINARY_DIR 运行cmake命令的目录,通常是${PROJECT_SOURCE_DIR}/build
- CMAKE_INCLUDE_PATH 环境变量,非cmake变量
- CMAKE_LIBRARY_PATH 环境变量
- CMAKE_CURRENT_SOURCE_DIR **当前处理的CMakeLists.txt所在的路径**
- CMAKE_CURRENT_BINARY_DIR target编译目录
- CMAKE_CURRENT_LIST_FILE **输出调用这个变量的CMakeLists**.txt的完整路径
- CMAKE_CURRENT_LIST_LINE 输出这个变量所在的行
- CMAKE_MODULE_PATH 定义自己的cmake模块所在的路径

### messge
message 用来输出信息


```
MESSAGE([SEND_ERROR | STATUS | FATAL_ERROR] “message to display” …)
```

- 向终端输出用户定义的信息或变量的值
- SEND_ERROR, 产生错误,生成过程被跳过
- STATUS, 输出前缀为—的信息
- FATAL_ERROR, 立即终止所有cmake过程

```
message("PROJECT_SOURCE_DIR--> ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR--> ${PROJECT_BINARY_DIR}")
message("CMAKE_CURRENT_LIST_FILE--> ${CMAKE_CURRENT_LIST_FILE}")
message("name --> ${NAME}")
```
输出：

PROJECT_SOURCE_DIR--> /Users/didi/workcode/native-js/hummer_19/CmakeDemo/app/src/main/cpp

PROJECT_BINARY_DIR--> /Users/didi/workcode/native-js/hummer_19/CmakeDemo/app/.cxx/cmake/debug/arm64-v8a

CMAKE_CURRENT_LIST_FILE--> /Users/didi/workcode/native-js/hummer_19/CmakeDemo/app/src/main/cpp/CMakeLists.txt

name --> nate

这个地方有个疑问，就是这个message 是在哪里呢？

**message 输出的地方是在build_output.txt文件中**

### set
set 这个用来定义一个变量

```
# 变量名为 var，值为 hello
set(var hello) 
```
当需要引用变量时，在变量名外面加上 ${} 符合来引用变量。


```
# 引用 var 变量
${var}

```
还可以通过 message 在命令行中输出打印内容。


```
set(var hello) 
message(${var})
```

### 字符串操作
CMake 通过 string 来实现字符串的操作，这波操作有很多，包括将字符串全部大写、全部小写、求字符串长度、查找与替换等操作。

具体查看 官方文档。

```
set(var "this is  string")
set(sub "this")
set(sub1 "that")
# 字符串的查找，结果保存在 result 变量中
string(FIND ${var} ${sub1} result )
# 找到了输出 0 ，否则为 -1
message(${result})

# 将字符串全部大写
string(TOUPPER ${var} result)
message(${result})

# 求字符串的长度
string(LENGTH ${var} num)
message(${num})
```
另外，通过空白或者分隔符号可以表示字符串序列。


```
set(foo this is a list) // 实际内容为字符串序列
message(${foo})
```
当字符串中需要用到空白或者分隔符时，再用双括号""表示为同一个字符串内容。

```
set(foo "this is a list") // 实际内容为一个字符串
message(${foo})

```



### file
CMake 中通过 file 来实现文件操作，包括文件读写、下载文件、文件重命名等。


```
# 文件重命名
file(RENAME "test.txt" "new.txt")

# 文件下载
# 把文件 URL 设定为变量
set(var "http://img.zcool.cn/community/0117e2571b8b246ac72538120dd8a4.jpg")

# 使用 DOWNLOAD 下载
file(DOWNLOAD ${var} "/Users/glumes/CLionProjects/HelloCMake/image.jpg")
```
在文件的操作中，还有两个很重要的指令 GLOB 和 GLOB_RECURSE 。


```
# GLOB 的使用
file(GLOB ROOT_SOURCE *.cpp)
# GLOB_RECURSE 的使用
file(GLOB_RECURSE CORE_SOURCE ./detail/*.cpp)
```
其中，GLOB 指令会将所有匹配 *.cpp 表达式的文件组成一个列表，并保存在 ROOT_SOURCE 变量中。

而 GLOB_RECURSE 指令和 GLOB 类似，但是它会遍历匹配目录的所有文件以及子目录下面的文件。

使用  GLOB 和 GLOB_RECURSE
- 有好处：就是当添加需要编译的文件时，不用再一个一个手动添加了，同一目录下的内容都被包含在对应变量中了，
- **但也有弊端，就是新建了文件，但是 CMake 并没有改，导致在编译时也会重新产生构建文件，要解决这个问题，就是动一动 CMake，让编译器检测到它有改变就好了。**






# 参考资料

[NDK开发总结](https://ejin66.github.io/2018/01/08/android-ndk.html)

[android studio 使用Cmake编译jni](http://www.z-gelen.com/index.php/archives/150/)

[Android.mk与Cmake配置](https://blog.csdn.net/hongxue8888/article/details/102473727)

[多级目录下cmake CMakeLists.txt使用方法（多个CmakeLists.txt编译）](https://blog.csdn.net/u012258999/article/details/87161613)

[Android NDK秘籍--编译静态库、调用静态库](https://juejin.im/post/5cbc2a3f518825324c44f433/)


[ndk-samples](https://github.com/android/ndk-samples/blob/master/webp/view/src/main/cpp/CMakeLists.txt)

 [Android NDK初步](https://rustfisher.com/2016/06/14/Android/NDK-use_sample_2/)
 
 [Android NDK开发系列教程5：局部引用，全局引用，弱全局引用](https://www.dazhuanlan.com/2019/10/14/5da3f9861c80e/)
 
 [Android develop JNI教程](https://developer.android.com/training/articles/perf-jni)
 



##  Cmake 配置相关
- [Android NDK 开发之 CMake 必知必会](https://juejin.im/post/5b9879976fb9a05d330aa206)
-  [CMake](https://chsmy.github.io/2019/05/25/technology/CMake/)
-  [配置 CMake](https://developer.android.com/studio/projects/configure-cmake?hl=zh-cn)
 
##  Cmake 编译第三方so
- [AS使用NDK Cmake方式依赖第三方库](https://segmentfault.com/a/1190000012729891)
- [使用cmake解决Android中对第三方库的依赖](https://mushuichuan.com/2017/06/11/cmake/)
- [使用cmake连接第三方so](https://guolei1130.github.io/2017/06/22/%E4%BD%BF%E7%94%A8cmake%E8%BF%9E%E6%8E%A5%E7%AC%AC%E4%B8%89%E6%96%B9so/)
- [android studio 3.2 cmake jni调用第三方库动态库
](https://blog.csdn.net/qq_34759481/article/details/83898710)
 
 [你真的了解 NDK 和 jni 的区别吗
](https://juejin.im/post/5989133ff265da3e2e56ff26)



JNI 调用相关

[Android JNI/NDK 使用全解
](https://juejin.im/post/5df774f7f265da33e97fceef)




 [AndroidJNI优化](https://juejin.im/post/5d2bf60a51882566d05f4672#heading-5)
 
 [Android NDK开发扫盲及最新CMake的编译使用](https://www.jianshu.com/p/6332418b12b1)