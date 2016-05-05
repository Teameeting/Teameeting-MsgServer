// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sys_msg_type.proto

#ifndef PROTOBUF_sys_5fmsg_5ftype_2eproto__INCLUDED
#define PROTOBUF_sys_5fmsg_5ftype_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)

namespace pms {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_sys_5fmsg_5ftype_2eproto();
void protobuf_AssignDesc_sys_5fmsg_5ftype_2eproto();
void protobuf_ShutdownFile_sys_5fmsg_5ftype_2eproto();


enum ETransferModule {
  MCONNECTOR = 1,
  MMSGQUEUE = 2,
  MMEETING = 3,
  MP2P = 4,
  MLIVE = 5,
  MCALLCENTER = 6
};
bool ETransferModule_IsValid(int value);
const ETransferModule ETransferModule_MIN = MCONNECTOR;
const ETransferModule ETransferModule_MAX = MCALLCENTER;
const int ETransferModule_ARRAYSIZE = ETransferModule_MAX + 1;

const ::google::protobuf::EnumDescriptor* ETransferModule_descriptor();
inline const ::std::string& ETransferModule_Name(ETransferModule value) {
  return ::google::protobuf::internal::NameOfEnum(
    ETransferModule_descriptor(), value);
}
inline bool ETransferModule_Parse(
    const ::std::string& name, ETransferModule* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ETransferModule>(
    ETransferModule_descriptor(), name, value);
}
enum ETransferType {
  TCONN = 1,
  TTRANS = 2,
  TQUEUE = 3,
  TDISPATCH = 4,
  TPUSH = 5,
  TLOGIN = 6,
  TLOGOUT = 7
};
bool ETransferType_IsValid(int value);
const ETransferType ETransferType_MIN = TCONN;
const ETransferType ETransferType_MAX = TLOGOUT;
const int ETransferType_ARRAYSIZE = ETransferType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ETransferType_descriptor();
inline const ::std::string& ETransferType_Name(ETransferType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ETransferType_descriptor(), value);
}
inline bool ETransferType_Parse(
    const ::std::string& name, ETransferType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ETransferType>(
    ETransferType_descriptor(), name, value);
}
enum ETransferPriority {
  PLOW = 1,
  PNORMAL = 2,
  PHIGH = 3
};
bool ETransferPriority_IsValid(int value);
const ETransferPriority ETransferPriority_MIN = PLOW;
const ETransferPriority ETransferPriority_MAX = PHIGH;
const int ETransferPriority_ARRAYSIZE = ETransferPriority_MAX + 1;

const ::google::protobuf::EnumDescriptor* ETransferPriority_descriptor();
inline const ::std::string& ETransferPriority_Name(ETransferPriority value) {
  return ::google::protobuf::internal::NameOfEnum(
    ETransferPriority_descriptor(), value);
}
inline bool ETransferPriority_Parse(
    const ::std::string& name, ETransferPriority* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ETransferPriority>(
    ETransferPriority_descriptor(), name, value);
}
enum ETransferFlag {
  FNOACK = 1,
  FNEEDACK = 2,
  FACK = 3
};
bool ETransferFlag_IsValid(int value);
const ETransferFlag ETransferFlag_MIN = FNOACK;
const ETransferFlag ETransferFlag_MAX = FACK;
const int ETransferFlag_ARRAYSIZE = ETransferFlag_MAX + 1;

const ::google::protobuf::EnumDescriptor* ETransferFlag_descriptor();
inline const ::std::string& ETransferFlag_Name(ETransferFlag value) {
  return ::google::protobuf::internal::NameOfEnum(
    ETransferFlag_descriptor(), value);
}
inline bool ETransferFlag_Parse(
    const ::std::string& name, ETransferFlag* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ETransferFlag>(
    ETransferFlag_descriptor(), name, value);
}
enum EConnTag {
  THI = 1,
  THELLO = 2,
  THELLOHI = 3,
  TKEEPALIVE = 4
};
bool EConnTag_IsValid(int value);
const EConnTag EConnTag_MIN = THI;
const EConnTag EConnTag_MAX = TKEEPALIVE;
const int EConnTag_ARRAYSIZE = EConnTag_MAX + 1;

const ::google::protobuf::EnumDescriptor* EConnTag_descriptor();
inline const ::std::string& EConnTag_Name(EConnTag value) {
  return ::google::protobuf::internal::NameOfEnum(
    EConnTag_descriptor(), value);
}
inline bool EConnTag_Parse(
    const ::std::string& name, EConnTag* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EConnTag>(
    EConnTag_descriptor(), name, value);
}
enum EConnType {
  TTCP = 1,
  THTTP = 2
};
bool EConnType_IsValid(int value);
const EConnType EConnType_MIN = TTCP;
const EConnType EConnType_MAX = THTTP;
const int EConnType_ARRAYSIZE = EConnType_MAX + 1;

const ::google::protobuf::EnumDescriptor* EConnType_descriptor();
inline const ::std::string& EConnType_Name(EConnType value) {
  return ::google::protobuf::internal::NameOfEnum(
    EConnType_descriptor(), value);
}
inline bool EConnType_Parse(
    const ::std::string& name, EConnType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EConnType>(
    EConnType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================


// @@protoc_insertion_point(namespace_scope)

}  // namespace pms

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::pms::ETransferModule> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::ETransferModule>() {
  return ::pms::ETransferModule_descriptor();
}
template <> struct is_proto_enum< ::pms::ETransferType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::ETransferType>() {
  return ::pms::ETransferType_descriptor();
}
template <> struct is_proto_enum< ::pms::ETransferPriority> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::ETransferPriority>() {
  return ::pms::ETransferPriority_descriptor();
}
template <> struct is_proto_enum< ::pms::ETransferFlag> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::ETransferFlag>() {
  return ::pms::ETransferFlag_descriptor();
}
template <> struct is_proto_enum< ::pms::EConnTag> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::EConnTag>() {
  return ::pms::EConnTag_descriptor();
}
template <> struct is_proto_enum< ::pms::EConnType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::EConnType>() {
  return ::pms::EConnType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_sys_5fmsg_5ftype_2eproto__INCLUDED