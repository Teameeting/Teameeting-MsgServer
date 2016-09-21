APP_BUILD_SCRIPT := $(call my-dir)/Android.mk
APP_MODULES := rtcmsgclient-android

NDK_PATH := /Users/hp/Documents/tools/android-ndk-r10d
NDK_STL_INC := $(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/include
NDK_STL_INC += $(NDK_PATH)/sources/cxx-stl/stlport/stlport

APP_OPTIM        := release 
APP_CFLAGS       += -Os
APP_STL					 := stlport_static
##APP_STL := gnustl_static

##APP_PLATFORM := android-9 
