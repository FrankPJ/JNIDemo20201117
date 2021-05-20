//
// Created by Administrator on 2021/5/19.
//

#ifndef CANBUS_CANCMD_H
#define CANBUS_CANCMD_H






void wheelKeyInformParse(char *bytearr);
void airInformParse(char *bytearr);
void rRadarInformParse(char *bytearr);
void fRadarInformParse(char *bytearr);
void doorInformParse(char *bytearr);
void wheelTrackInformParse(char *bytearr);












#endif //CANBUS_CANCMD_H



#define SCREEN_HEIGHT 640


const char BACK_LIGHT_INFORM = 0x14;
const char SPEED_INFORM = 0x16;
const char STEERING_WHEEL = 0x20;
const char AIR_CONDITIONING = 0x21;
const char REAR_RADAR = 0x22;
const char FRONT_RADAR = 0x23;
const char BASIC_INFORM = 0x24;
const char PARKING_ASSIST = 0x25;
const char WHEEL_TRACK = 0x29;
const char OFFENSIVE_STATE = 0x27;
const char VERSION_INFORM = 0x30;
const char LEFT_RADAR = 0x32;
const char RIGHT_RADAR = 0x33;
const char CAR_BODY_INFORM = 0x41;
