#include <jni.h>
#include <android/log.h>



static const char *TAG="native-lib";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)



jstring helloWorld(JNIEnv * env, __unused jobject thiz){

    jstring str = env->NewStringUTF("hello jni glkdsghsdlkghdslkghdslghdslgh");

    return str;

}

jint intFromJni(JNIEnv * env, __unused jobject thiz){

   int i=23432;
   LOGD("intFromJni");
    return i;

}


jfloat floatFromJni(JNIEnv * env, __unused jobject thiz){

   float i=23432.45;
   LOGD("floatFromJni");
    return i;

}

jdouble doubleFromJni(JNIEnv * env, __unused jobject thiz){

   double i=23432.4599999999;
   LOGD("doubleFromJni");
    return i;

}


jbyte byteFromJni(JNIEnv * env, __unused jobject thiz){

    unsigned char i='a';
   LOGD("byteFromJni");
    return i;

}


jlong longFromJni(JNIEnv * env, __unused jobject thiz){

    long i='b';
   LOGD("longFromJni");
    return i;

}

jchar charFromJni(JNIEnv * env, __unused jobject thiz){

   unsigned char i='c';
   LOGD("charFromJni");
    return i;

}


jboolean booleanFromJni(JNIEnv * env, __unused jobject thiz){

  bool i= false;
   LOGD("booleanFromJni");
    return i;

}

jintArray intArrayFromJni(JNIEnv * env, __unused jobject thiz){


    int iArray[10]={
            10,
            10,
            10,
            10,
            10,
            10,
            10,
            10,
    };

    jsize len =10;
    jintArray jntarray = env->NewIntArray(len);

    //数组转化为 jint* 类型

    jboolean *isCopy = nullptr;
    jint * jintp = env->GetIntArrayElements(jntarray, isCopy);


    for (int i = 0; i < len; i++)
    {   //jint 本质long 所以可以赋值
        jintp[i] = iArray[i];
    }

    //释放同步
    env->ReleaseIntArrayElements(jntarray, jintp, 0);

    //返回
    return jntarray;


}


jobjectArray objectArrayFromJni(JNIEnv * env, __unused jobject thiz){


     jsize ARRAY_LENGTH =5;
    jclass objClass = env->FindClass( "java/lang/String");
    jobjectArray texts= env->NewObjectArray(ARRAY_LENGTH, objClass,nullptr);
    jstring jstr;
    char* sa[] = { "Hello,", "world!", "黄子韬", "吴亦凡", "蔡徐坤" };
    int i=0;
    for(;i<ARRAY_LENGTH;i++)
    {
        jstr = env->NewStringUTF(  sa[i] );
        env->SetObjectArrayElement(texts, i, jstr);//必须放入jstring
    }
    return texts;


}

static JNINativeMethod gMethods[] = {
        {"helloWorld", "()Ljava/lang/String;", (void*)helloWorld},
        {"intFromJni", "()I", (void*)intFromJni},
        {"floatFromJni", "()F", (void*)floatFromJni},
        {"doubleFromJni", "()D", (void*)doubleFromJni},
        {"byteFromJni", "()B", (void*)byteFromJni},
        {"longFromJni", "()J", (void*)longFromJni},
        {"charFromJni", "()C", (void*)charFromJni},
        {"booleanFromJni", "()Z", (void*)booleanFromJni},
        {"intArrayFromJni", "()[I", (void*)intArrayFromJni},
        {"objectArrayFromJni", "()[Ljava/lang/String;", (void*)objectArrayFromJni},
};


static int registerNativeMethods(JNIEnv* env)

{
    jclass clazz;
    clazz = env->FindClass("com/szchoiceway/jnidemo20201117/MainActivity");
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