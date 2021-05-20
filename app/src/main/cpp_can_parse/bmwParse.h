//
// Created by Administrator on 2021/5/19.
//

#ifndef CANBUS_BMWPARSE_H
#define CANBUS_BMWPARSE_H

#endif //CANBUS_BMWPARSE_H

void parseData( char  *bytearr);


void *async_thread_func(void *args);









extern "C" JNIEXPORT void JNICALL
parseCmdEvtNative(JNIEnv *env, jobject bmwParse, jbyteArray data);