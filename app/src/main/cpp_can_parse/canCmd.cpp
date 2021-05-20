//
// Created by Administrator on 2021/5/19.
//

#include <sys/types.h>
#include <pthread.h>
#include "android/log.h"
#include "bmwParse.h"

static const char *TAG = "CanCmd";
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

void wheelKeyInformParse(char *bytearr) {
    LOGE("wheelKeyInformParse");
}

void airInformParse(char *bytearr) {
    LOGE("airInformParse");
}

void rRadarInformParse(char *bytearr) {
    LOGE("rRadarInformParse");

}

void fRadarInformParse(char *bytearr) {
    LOGE("fRadarInformParse");


}

void doorInformParse(char *bytearr) {
    LOGE("doorInformParse");

}

void wheelTrackInformParse(char *bytearr) {
    LOGE("wheelTrackInformParse");


//     这里创建子线程模拟异步回调
    pthread_t ntid;
    pthread_create(&ntid, nullptr, async_thread_func, nullptr);

}