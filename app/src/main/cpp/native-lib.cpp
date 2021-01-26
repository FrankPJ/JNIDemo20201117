#include <jni.h>
#include <android/log.h>
#include <string>
#include <extra.h>
#include <classC.h>

extern "C"{
#include <c_extra.h>
}


static const char *TAG="native-lib";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)



extern "C" JNIEXPORT jstring JNICALL
stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    std::string hello1 = "fasfadfadfadfadfadfadfdafdafad";
    // 1. 获取 thiz 的 class，也就是 java 中的 Class 信息
    jclass thisclazz = env->GetObjectClass(thiz);
    // 2. 根据 Class 获取 getClass 方法的 methodID，第三个参数是签名(params)return
    jmethodID mid_getClass = env->GetMethodID(thisclazz, "getClass", "()Ljava/lang/Class;");
    // 3. 执行 getClass 方法，获得 Class 对象
    jobject clazz_instance = env->CallObjectMethod(thiz, mid_getClass);
    // 4. 获取 Class 实例
    jclass clazz = env->GetObjectClass(clazz_instance);
    // 5. 根据 class  的 methodID
    jmethodID mid_getName = env->GetMethodID(clazz, "getName", "()Ljava/lang/String;");
    // 6. 调用 getName 方法
    auto name = static_cast<jstring>(env->CallObjectMethod(clazz_instance, mid_getName));
    LOGE("class name:%s", env->GetStringUTFChars(name, nullptr));

    // 7. 释放资源
    env->DeleteLocalRef(thisclazz);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(clazz_instance);
    env->DeleteLocalRef(name);

    return env->NewStringUTF(hello1.c_str());

}




extern "C" JNIEXPORT void JNICALL callPerson(JNIEnv* env,jobject jobject1)
{

    // 1. 获取 Class
    jclass pClazz = env->FindClass("com/szchoiceway/jnidemo20201117/Person");
    // 2. 获取构造方法，方法名固定为<init>
    jmethodID constructID = env->GetMethodID(pClazz,"<init>","(ILjava/lang/String;)V");
    if(constructID == nullptr){
        return;
    }
    // 3. 创建一个 Person 对象
    jstring name = env->NewStringUTF("alex");
    jobject person = env->NewObject(pClazz,constructID,1,name);

    jmethodID printId = env->GetMethodID(pClazz,"print","()V");
    if(printId == nullptr){
        return;
    }
    env->CallVoidMethod(person,printId);

    // 4. 释放资源
    env->DeleteLocalRef(name);
    env->DeleteLocalRef(pClazz);



}




extern "C" JNIEXPORT jstring JNICALL
helloWorld(JNIEnv * env, __unused jobject thiz){

//    const classC &c = classC();
    
//      std::string s=c.str;
      std::string s=getString();
    std::string s1= getStringFromC();
//      std::string s="getString()";
    return env->NewStringUTF(s1.c_str());

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
        {"stringFromJNI", "()Ljava/lang/String;", (void*)stringFromJNI},
        {"callPerson", "()V", (void*)callPerson},
};


static int registerNativeMethods(JNIEnv* env)

{
    jclass clazz;
    clazz = env->FindClass("com/szchoiceway/jnidemo20201117/JniTest");
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