

cmake_minimum_required(VERSION 3.10.2)



project("od_bmw_nbt_evo")

#依赖cpp目录下所有文件
aux_source_directory(. DIR_LIB_SRCS)

# 导入当前目录下的所有头文件
include_directories(.)


add_library(bmwParse SHARED ${DIR_LIB_SRCS})



#引用log库
find_library(log-lib log)


target_link_libraries(


        bmwParse  ${log-lib}




)