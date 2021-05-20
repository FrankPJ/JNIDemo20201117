//
// Created by Administrator on 2021/5/19.
//


#include <cstdlib>
#include <jni.h>
#include "android/log.h"
#include <cstdio>
#include <malloc.h>
#include "canCmd.h"
#include "bmwParse.h"

#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ##args)
static const char *TAG = "bmwParse";



void parseData( char  *bytearr){


    unsigned char cmd = bytearr[1];

    switch (cmd) {

        case STEERING_WHEEL: {
            wheelKeyInformParse((char *)bytearr);

        }
            break;

        case AIR_CONDITIONING: {
            airInformParse((char *)bytearr);

        }
            break;

        case BASIC_INFORM: {
            doorInformParse((char *)bytearr);

        }
            break;


        case WHEEL_TRACK: {
            wheelTrackInformParse((char *)bytearr);

        }
            break;

        case FRONT_RADAR: {
            fRadarInformParse((char *)bytearr);

        }
            break;


        case REAR_RADAR: {
            rRadarInformParse((char *)bytearr);

        }
            break;
        default:
            break;


    }

};


typedef unsigned char byte;




typedef struct
{
    jobject onParseResult;
    jmethodID onWheelTrackChange;

    jclass class_onParseResult;
    jmethodID m_onParseResult_init;
} jni_callback_t;

static jni_callback_t *jni_callback;



extern "C" JNIEXPORT void JNICALL
parseCmdEvtNative(JNIEnv *env, jobject bmwParse, jbyteArray data) {

    LOGE("parseCmdEvt");


    jsize len = env->GetArrayLength(data);

//    auto *jbarray = (jbyte *)malloc(len * sizeof(jbyte));
    auto *jbarray = (jbyte *) env->GetByteArrayElements(data, nullptr);

    env->GetByteArrayRegion(data, 0, len, jbarray);

    byte *bytearr = (byte *) jbarray;

    for (int i = 0; i < len; i++) {
        LOGE("%02X", bytearr[i]);
    }
    parseData((char *)bytearr);





}





static JNINativeMethod gMethods[] = {
        {"parseCmdEvtNative", "([B)V", (void*)parseCmdEvtNative},
};


static int registerNativeMethods(JNIEnv* env)

{
    jclass clazz;
    clazz = env->FindClass("com/frank/canbus/BmwParse");
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}








typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, __unused void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = nullptr;
    jint result = -1;
    JNIEnv* env;


    LOGD("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNativeMethods(env) != JNI_TRUE) {
        LOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

    bail:
    return result;
}
