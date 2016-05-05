// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: meet_msg_type.proto

#import "GPBProtocolBuffers.h"

#if GOOGLE_PROTOBUF_OBJC_GEN_VERSION != 30001
#error This file was generated by a different version of protoc which is incompatible with your Protocol Buffer library sources.
#endif

// @@protoc_insertion_point(imports)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

CF_EXTERN_C_BEGIN

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Enum EMsgType

/// the msg content type
typedef GPB_ENUM(EMsgType) {
  EMsgType_Tmsg = 1,
  EMsgType_Tfile = 2,
  EMsgType_Tpic = 3,
  EMsgType_Taud = 4,
  EMsgType_Tvid = 5,
};

GPBEnumDescriptor *EMsgType_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EMsgType_IsValidValue(int32_t value);

#pragma mark - Enum EMsgTag

/// the msg tag
typedef GPB_ENUM(EMsgTag) {
  EMsgTag_Tenter = 1,
  EMsgTag_Tleave = 2,
  EMsgTag_Tchat = 3,
  EMsgTag_Tnotify = 4,
};

GPBEnumDescriptor *EMsgTag_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EMsgTag_IsValidValue(int32_t value);

#pragma mark - Enum EMsgHead

/// the msg get tag
typedef GPB_ENUM(EMsgHead) {
  EMsgHead_Hsnd = 1,
  EMsgHead_Hget = 2,
};

GPBEnumDescriptor *EMsgHead_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EMsgHead_IsValidValue(int32_t value);

#pragma mark - MeetMsgTypeRoot

/// Exposes the extension registry for this file.
///
/// The base class provides:
/// @code
///   + (GPBExtensionRegistry *)extensionRegistry;
/// @endcode
/// which is a @c GPBExtensionRegistry that includes all the extensions defined by
/// this file and all files that it depends on.
@interface MeetMsgTypeRoot : GPBRootObject
@end

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

#pragma clang diagnostic pop

// @@protoc_insertion_point(global_scope)