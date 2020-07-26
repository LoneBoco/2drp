// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CRequestFile.proto

#ifndef PROTOBUF_CRequestFile_2eproto__INCLUDED
#define PROTOBUF_CRequestFile_2eproto__INCLUDED

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

namespace protobuf_CRequestFile_2eproto {
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
void InitDefaultsCRequestFileImpl();
void InitDefaultsCRequestFile();
inline void InitDefaults() {
  InitDefaultsCRequestFile();
}
}  // namespace protobuf_CRequestFile_2eproto
namespace tdrp {
namespace packet {
class CRequestFile;
class CRequestFileDefaultTypeInternal;
extern CRequestFileDefaultTypeInternal _CRequestFile_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace tdrp {
namespace packet {

// ===================================================================

class CRequestFile : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.CRequestFile) */ {
 public:
  CRequestFile();
  virtual ~CRequestFile();

  CRequestFile(const CRequestFile& from);

  inline CRequestFile& operator=(const CRequestFile& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  CRequestFile(CRequestFile&& from) noexcept
    : CRequestFile() {
    *this = ::std::move(from);
  }

  inline CRequestFile& operator=(CRequestFile&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const CRequestFile& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const CRequestFile* internal_default_instance() {
    return reinterpret_cast<const CRequestFile*>(
               &_CRequestFile_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(CRequestFile* other);
  friend void swap(CRequestFile& a, CRequestFile& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline CRequestFile* New() const PROTOBUF_FINAL { return New(NULL); }

  CRequestFile* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const CRequestFile& from);
  void MergeFrom(const CRequestFile& from);
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
  void InternalSwap(CRequestFile* other);
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

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
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

  // @@protoc_insertion_point(class_scope:tdrp.packet.CRequestFile)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  mutable int _cached_size_;
  friend struct ::protobuf_CRequestFile_2eproto::TableStruct;
  friend void ::protobuf_CRequestFile_2eproto::InitDefaultsCRequestFileImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// CRequestFile

// string name = 1;
inline void CRequestFile::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& CRequestFile::name() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.CRequestFile.name)
  return name_.GetNoArena();
}
inline void CRequestFile::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.CRequestFile.name)
}
#if LANG_CXX11
inline void CRequestFile::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.CRequestFile.name)
}
#endif
inline void CRequestFile::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.CRequestFile.name)
}
inline void CRequestFile::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.CRequestFile.name)
}
inline ::std::string* CRequestFile::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.CRequestFile.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* CRequestFile::release_name() {
  // @@protoc_insertion_point(field_release:tdrp.packet.CRequestFile.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void CRequestFile::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.CRequestFile.name)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_CRequestFile_2eproto__INCLUDED
