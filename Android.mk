LOCAL_PATH := $(call my-dir)

# Build the gamelib:
include $(CLEAR_VARS)

LOCAL_MODULE := gamelib

# All of the files needed to build the Android gamelib:
LOCAL_C_INCLUDES := "../src/include"
LOCAL_SRC_FILES  := $(wildcard $(LOCAL_PATH)/src/gamelib/Android/*.c)
LOCAL_SRC_FILES  := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS := -DUSE_ANDROID_GAMELIB

LOCAL_EXPORT_LDLIBS := -ljnigraphics -llog

include $(BUILD_STATIC_LIBRARY)


# Build the rest of the game logic:
include $(CLEAR_VARS)

LOCAL_MODULE := ctunneltank

# The main game files:
LOCAL_C_INCLUDES := "../src/include"
LOCAL_SRC_FILES  := $(wildcard $(LOCAL_PATH)/src/*.c)
LOCAL_SRC_FILES  := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := gamelib

include $(BUILD_SHARED_LIBRARY)

