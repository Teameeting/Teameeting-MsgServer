APP_BUILD_SCRIPT := $(call my-dir)/Android.mk
APP_MODULES := rtklog 

NDK_PATH := /cygdrive/c/Android/NDK/android-ndk-r8
NDK_STL_INC := $(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/include

APP_OPTIM        := release 
APP_CFLAGS       += -Os
#APP_STL := stlport_static
APP_STL := gnustl_shared

APP_PLATFORM := android-9 