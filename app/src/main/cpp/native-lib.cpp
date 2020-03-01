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

    const char *version = cJSON_Version();
    LOGE("cjson version= %s", version);

    return env->NewStringUTF(hello.c_str());
}
