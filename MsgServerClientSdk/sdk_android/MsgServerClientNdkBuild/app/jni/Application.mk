APP_BUILD_SCRIPT := $(call my-dir)/toolchain/Android.mk
APP_MODULES := msgclient-jni jni_utils

NDK_PATH := /Users/hp/Documents/tools/android-ndk-r10d
#####NDK_STL_INC := $(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.8/include
NDK_STL_INC += $(NDK_PATH)/sources/cxx-stl/stlport/stlport

APP_OPTIM        := release 
APP_CFLAGS       += -O3
#APP_STL := gnustl_shared
###APP_STL := gnustl_static
APP_STL := stlport_static
NDK_TOOLCHAIN_VERSION = 4.8

APP_PLATFORM := android-14


####javah -d jni -classpath /Users/hp/Documents/tools/android-sdk-macosx/platforms/android-21/android.jar:../../build/intermediates/classes/debug/ com.dync.teameeting.msgserverclient.jni.JMClientApp

