LOCAL_PATH := $(call my-dir)  
  
include $(CLEAR_VARS)  
LOCAL_MODULE := msgclient   			# ģ����
LOCAL_SRC_FILES := libmsgclient.so		# ģ����ļ�·��������� LOCAL_PATH��

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
  					$(LOCAL_PATH)/ \

include $(PREBUILT_SHARED_LIBRARY) 		# ע�����ﲻ�� BUILD_SHARED_LIBRARY