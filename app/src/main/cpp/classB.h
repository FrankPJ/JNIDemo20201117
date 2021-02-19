//
// Created by Administrator on 2021/1/28.
//


#include "classC.h"
#include "classA.h"

#ifndef JNIDEMO20201117_CLASSB_H
#define JNIDEMO20201117_CLASSB_H




class classB : public classC,classA{
public:
    classB(const char *str, int age);


};


#endif //JNIDEMO20201117_CLASSB_H
