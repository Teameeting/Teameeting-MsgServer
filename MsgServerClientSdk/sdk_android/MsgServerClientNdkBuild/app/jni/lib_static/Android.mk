LOCAL_PATH := $(call my-dir)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := msgclient-tmp   			# ģ����
LOCAL_SRC_FILES := librtcmsgclient-android.a		# ģ����ļ�·��������� LOCAL_PATH��
LOCAL_CFLAGS := -fno-rtti

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
  					$(LOCAL_PATH)/ \

include $(PREBUILT_STATIC_LIBRARY) 		# ע�����ﲻ�� BUILD_SHARED_LIBRARY