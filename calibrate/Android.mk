# Copyright 2007-2011 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_AAPT_FLAGS += -c ldpi -c mdpi  
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_CERTIFICATE := platform

LOCAL_PACKAGE_NAME := calibrate
LOCAL_PRIVATE_PLATFORM_APIS := true
# Builds against the public SDK
#LOCAL_SDK_VERSION := current
LOCAL_DEX_PREOPT :=false

include $(BUILD_PACKAGE)

# This finds and builds the test apk as well, so a single make does both.
include $(call all-makefiles-under,$(LOCAL_PATH)) 
