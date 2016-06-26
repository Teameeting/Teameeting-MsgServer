LOCAL_PATH := $(call my-dir)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := msgclient-tmp
LOCAL_SRC_FILES := $(LOCAL_PATH)/../client_common/lib_android_a/librtcmsgclient-android.a

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
  					$(LOCAL_PATH)/ \

include $(PREBUILT_STATIC_LIBRARY) 		# ע�����ﲻ�� BUILD_SHARED_LIBRARY