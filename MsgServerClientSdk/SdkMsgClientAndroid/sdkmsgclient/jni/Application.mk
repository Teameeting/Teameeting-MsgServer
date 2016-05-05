APP_BUILD_SCRIPT := $(call my-dir)/toolchain/Android.mk
APP_MODULES := msgclient-jni jni_utils rtcmsgclient-android protobuf-lite

NDK_PATH := /Users/hp/Documents/tools/android-ndk-r10d
NDK_STL_INC := $(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.8/include
NDK_STL_INC += $(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/include

APP_OPTIM        := release
APP_CFLAGS       += -O3
APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION = 4.8
APP_CPPFLAGS := -std=c++11

APP_PLATFORM := android-14

APP_ABI := armeabi
APP_ABI += armeabi-v7a
####APP_ABI += arm64-v8a

####cd java
#### javac org/dync/teameeting/sdkmsgclient/jni/JMClientHelper.java
#### javac org/dync/teameeting/sdkmsgclient/jni/JMClientApp.java
#### mv org/dync/teameeting/sdkmsgclient/jni/JMClientHelper.class ../../../build/intermediates/classes/debug/org/dync/teameeting/sdkmsgclient/jni/
#### mv org/dync/teameeting/sdkmsgclient/jni/JMClientApp.class ../../../build/intermediates/classes/debug/org/dync/teameeting/sdkmsgclient/jni/
#### javah -d jni -classpath /Users/hp/Documents/tools/android-sdk-macosx/platforms/android-23/android.jar:../../../build/intermediates/classes/debug/ org.dync.teameeting.sdkmsgclient.jni.JMClientApp

#### javah -d jni -classpath /Users/Skyline/Downloads/android-sdk-macosx/platforms/android-23/android.jar:../../../build/intermediates/classes/debug/ org.dync.teameeting.sdkmsgclient.jni.JMClientApp
