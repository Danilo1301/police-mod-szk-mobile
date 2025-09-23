LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := PoliceModSZK
LOCAL_CPP_FEATURES += exceptions

LOCAL_SRC_FILES := mod/logger.cpp mod/config.cpp
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/windows/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/dialog/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/inventory/*.cpp)

LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
LOCAL_C_INCLUDES += ./include
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)