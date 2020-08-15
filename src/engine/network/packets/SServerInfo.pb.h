// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SServerInfo.proto

#ifndef PROTOBUF_SServerInfo_2eproto__INCLUDED
#define PROTOBUF_SServerInfo_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_SServerInfo_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsSServerInfoImpl();
void InitDefaultsSServerInfo();
inline void InitDefaults() {
  InitDefaultsSServerInfo();
}
}  // namespace protobuf_SServerInfo_2eproto
namespace tdrp {
namespace packet {
class SServerInfo;
class SServerInfoDefaultTypeInternal;
extern SServerInfoDefaultTypeInternal _SServerInfo_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace tdrp {
namespace packet {

// ===================================================================

class SServerInfo : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.SServerInfo) */ {
 public:
  SServerInfo();
  virtual ~SServerInfo();

  SServerInfo(const SServerInfo& from);

  inline SServerInfo& operator=(const SServerInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SServerInfo(SServerInfo&& from) noexcept
    : SServerInfo() {
    *this = ::std::move(from);
  }

  inline SServerInfo& operator=(SServerInfo&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const SServerInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SServerInfo* internal_default_instance() {
    return reinterpret_cast<const SServerInfo*>(
               &_SServerInfo_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(SServerInfo* other);
  friend void swap(SServerInfo& a, SServerInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SServerInfo* New() const PROTOBUF_FINAL { return New(NULL); }

  SServerInfo* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const SServerInfo& from);
  void MergeFrom(const SServerInfo& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(SServerInfo* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string uniqueid = 1;
  void clear_uniqueid();
  static const int kUniqueidFieldNumber = 1;
  const ::std::string& uniqueid() const;
  void set_uniqueid(const ::std::string& value);
  #if LANG_CXX11
  void set_uniqueid(::std::string&& value);
  #endif
  void set_uniqueid(const char* value);
  void set_uniqueid(const char* value, size_t size);
  ::std::string* mutable_uniqueid();
  ::std::string* release_uniqueid();
  void set_allocated_uniqueid(::std::string* uniqueid);

  // string name = 2;
  void clear_name();
  static const int kNameFieldNumber = 2;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // string package = 3;
  void clear_package();
  static const int kPackageFieldNumber = 3;
  const ::std::string& package() const;
  void set_package(const ::std::string& value);
  #if LANG_CXX11
  void set_package(::std::string&& value);
  #endif
  void set_package(const char* value);
  void set_package(const char* value, size_t size);
  ::std::string* mutable_package();
  ::std::string* release_package();
  void set_allocated_package(::std::string* package);

  // string version = 4;
  void clear_version();
  static const int kVersionFieldNumber = 4;
  const ::std::string& version() const;
  void set_version(const ::std::string& value);
  #if LANG_CXX11
  void set_version(::std::string&& value);
  #endif
  void set_version(const char* value);
  void set_version(const char* value, size_t size);
  ::std::string* mutable_version();
  ::std::string* release_version();
  void set_allocated_version(::std::string* version);

  // string host = 5;
  void clear_host();
  static const int kHostFieldNumber = 5;
  const ::std::string& host() const;
  void set_host(const ::std::string& value);
  #if LANG_CXX11
  void set_host(::std::string&& value);
  #endif
  void set_host(const char* value);
  void set_host(const char* value, size_t size);
  ::std::string* mutable_host();
  ::std::string* release_host();
  void set_allocated_host(::std::string* host);

  // uint32 port = 6;
  void clear_port();
  static const int kPortFieldNumber = 6;
  ::google::protobuf::uint32 port() const;
  void set_port(::google::protobuf::uint32 value);

  // uint32 maxplayers = 7;
  void clear_maxplayers();
  static const int kMaxplayersFieldNumber = 7;
  ::google::protobuf::uint32 maxplayers() const;
  void set_maxplayers(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:tdrp.packet.SServerInfo)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr uniqueid_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::internal::ArenaStringPtr package_;
  ::google::protobuf::internal::ArenaStringPtr version_;
  ::google::protobuf::internal::ArenaStringPtr host_;
  ::google::protobuf::uint32 port_;
  ::google::protobuf::uint32 maxplayers_;
  mutable int _cached_size_;
  friend struct ::protobuf_SServerInfo_2eproto::TableStruct;
  friend void ::protobuf_SServerInfo_2eproto::InitDefaultsSServerInfoImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SServerInfo

// string uniqueid = 1;
inline void SServerInfo::clear_uniqueid() {
  uniqueid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SServerInfo::uniqueid() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.uniqueid)
  return uniqueid_.GetNoArena();
}
inline void SServerInfo::set_uniqueid(const ::std::string& value) {
  
  uniqueid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.uniqueid)
}
#if LANG_CXX11
inline void SServerInfo::set_uniqueid(::std::string&& value) {
  
  uniqueid_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SServerInfo.uniqueid)
}
#endif
inline void SServerInfo::set_uniqueid(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  uniqueid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SServerInfo.uniqueid)
}
inline void SServerInfo::set_uniqueid(const char* value, size_t size) {
  
  uniqueid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SServerInfo.uniqueid)
}
inline ::std::string* SServerInfo::mutable_uniqueid() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SServerInfo.uniqueid)
  return uniqueid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SServerInfo::release_uniqueid() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SServerInfo.uniqueid)
  
  return uniqueid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SServerInfo::set_allocated_uniqueid(::std::string* uniqueid) {
  if (uniqueid != NULL) {
    
  } else {
    
  }
  uniqueid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), uniqueid);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SServerInfo.uniqueid)
}

// string name = 2;
inline void SServerInfo::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SServerInfo::name() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.name)
  return name_.GetNoArena();
}
inline void SServerInfo::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.name)
}
#if LANG_CXX11
inline void SServerInfo::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SServerInfo.name)
}
#endif
inline void SServerInfo::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SServerInfo.name)
}
inline void SServerInfo::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SServerInfo.name)
}
inline ::std::string* SServerInfo::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SServerInfo.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SServerInfo::release_name() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SServerInfo.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SServerInfo::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SServerInfo.name)
}

// string package = 3;
inline void SServerInfo::clear_package() {
  package_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SServerInfo::package() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.package)
  return package_.GetNoArena();
}
inline void SServerInfo::set_package(const ::std::string& value) {
  
  package_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.package)
}
#if LANG_CXX11
inline void SServerInfo::set_package(::std::string&& value) {
  
  package_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SServerInfo.package)
}
#endif
inline void SServerInfo::set_package(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  package_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SServerInfo.package)
}
inline void SServerInfo::set_package(const char* value, size_t size) {
  
  package_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SServerInfo.package)
}
inline ::std::string* SServerInfo::mutable_package() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SServerInfo.package)
  return package_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SServerInfo::release_package() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SServerInfo.package)
  
  return package_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SServerInfo::set_allocated_package(::std::string* package) {
  if (package != NULL) {
    
  } else {
    
  }
  package_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), package);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SServerInfo.package)
}

// string version = 4;
inline void SServerInfo::clear_version() {
  version_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SServerInfo::version() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.version)
  return version_.GetNoArena();
}
inline void SServerInfo::set_version(const ::std::string& value) {
  
  version_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.version)
}
#if LANG_CXX11
inline void SServerInfo::set_version(::std::string&& value) {
  
  version_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SServerInfo.version)
}
#endif
inline void SServerInfo::set_version(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  version_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SServerInfo.version)
}
inline void SServerInfo::set_version(const char* value, size_t size) {
  
  version_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SServerInfo.version)
}
inline ::std::string* SServerInfo::mutable_version() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SServerInfo.version)
  return version_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SServerInfo::release_version() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SServerInfo.version)
  
  return version_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SServerInfo::set_allocated_version(::std::string* version) {
  if (version != NULL) {
    
  } else {
    
  }
  version_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), version);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SServerInfo.version)
}

// string host = 5;
inline void SServerInfo::clear_host() {
  host_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SServerInfo::host() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.host)
  return host_.GetNoArena();
}
inline void SServerInfo::set_host(const ::std::string& value) {
  
  host_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.host)
}
#if LANG_CXX11
inline void SServerInfo::set_host(::std::string&& value) {
  
  host_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SServerInfo.host)
}
#endif
inline void SServerInfo::set_host(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  host_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SServerInfo.host)
}
inline void SServerInfo::set_host(const char* value, size_t size) {
  
  host_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SServerInfo.host)
}
inline ::std::string* SServerInfo::mutable_host() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SServerInfo.host)
  return host_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SServerInfo::release_host() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SServerInfo.host)
  
  return host_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SServerInfo::set_allocated_host(::std::string* host) {
  if (host != NULL) {
    
  } else {
    
  }
  host_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), host);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SServerInfo.host)
}

// uint32 port = 6;
inline void SServerInfo::clear_port() {
  port_ = 0u;
}
inline ::google::protobuf::uint32 SServerInfo::port() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.port)
  return port_;
}
inline void SServerInfo::set_port(::google::protobuf::uint32 value) {
  
  port_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.port)
}

// uint32 maxplayers = 7;
inline void SServerInfo::clear_maxplayers() {
  maxplayers_ = 0u;
}
inline ::google::protobuf::uint32 SServerInfo::maxplayers() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SServerInfo.maxplayers)
  return maxplayers_;
}
inline void SServerInfo::set_maxplayers(::google::protobuf::uint32 value) {
  
  maxplayers_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SServerInfo.maxplayers)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_SServerInfo_2eproto__INCLUDED