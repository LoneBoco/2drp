// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: STransferFile.proto

#ifndef PROTOBUF_STransferFile_2eproto__INCLUDED
#define PROTOBUF_STransferFile_2eproto__INCLUDED

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
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_STransferFile_2eproto {
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
void InitDefaultsSTransferFileImpl();
void InitDefaultsSTransferFile();
inline void InitDefaults() {
  InitDefaultsSTransferFile();
}
}  // namespace protobuf_STransferFile_2eproto
namespace tdrp {
namespace packet {
class STransferFile;
class STransferFileDefaultTypeInternal;
extern STransferFileDefaultTypeInternal _STransferFile_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace tdrp {
namespace packet {

enum STransferFile_Type {
  STransferFile_Type_PACKAGE = 0,
  STransferFile_Type_IMAGE = 1,
  STransferFile_Type_STransferFile_Type_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  STransferFile_Type_STransferFile_Type_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool STransferFile_Type_IsValid(int value);
const STransferFile_Type STransferFile_Type_Type_MIN = STransferFile_Type_PACKAGE;
const STransferFile_Type STransferFile_Type_Type_MAX = STransferFile_Type_IMAGE;
const int STransferFile_Type_Type_ARRAYSIZE = STransferFile_Type_Type_MAX + 1;

const ::google::protobuf::EnumDescriptor* STransferFile_Type_descriptor();
inline const ::std::string& STransferFile_Type_Name(STransferFile_Type value) {
  return ::google::protobuf::internal::NameOfEnum(
    STransferFile_Type_descriptor(), value);
}
inline bool STransferFile_Type_Parse(
    const ::std::string& name, STransferFile_Type* value) {
  return ::google::protobuf::internal::ParseNamedEnum<STransferFile_Type>(
    STransferFile_Type_descriptor(), name, value);
}
// ===================================================================

class STransferFile : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.STransferFile) */ {
 public:
  STransferFile();
  virtual ~STransferFile();

  STransferFile(const STransferFile& from);

  inline STransferFile& operator=(const STransferFile& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  STransferFile(STransferFile&& from) noexcept
    : STransferFile() {
    *this = ::std::move(from);
  }

  inline STransferFile& operator=(STransferFile&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const STransferFile& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const STransferFile* internal_default_instance() {
    return reinterpret_cast<const STransferFile*>(
               &_STransferFile_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(STransferFile* other);
  friend void swap(STransferFile& a, STransferFile& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline STransferFile* New() const PROTOBUF_FINAL { return New(NULL); }

  STransferFile* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const STransferFile& from);
  void MergeFrom(const STransferFile& from);
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
  void InternalSwap(STransferFile* other);
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

  typedef STransferFile_Type Type;
  static const Type PACKAGE =
    STransferFile_Type_PACKAGE;
  static const Type IMAGE =
    STransferFile_Type_IMAGE;
  static inline bool Type_IsValid(int value) {
    return STransferFile_Type_IsValid(value);
  }
  static const Type Type_MIN =
    STransferFile_Type_Type_MIN;
  static const Type Type_MAX =
    STransferFile_Type_Type_MAX;
  static const int Type_ARRAYSIZE =
    STransferFile_Type_Type_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Type_descriptor() {
    return STransferFile_Type_descriptor();
  }
  static inline const ::std::string& Type_Name(Type value) {
    return STransferFile_Type_Name(value);
  }
  static inline bool Type_Parse(const ::std::string& name,
      Type* value) {
    return STransferFile_Type_Parse(name, value);
  }

  // accessors -------------------------------------------------------

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

  // bytes file = 4;
  void clear_file();
  static const int kFileFieldNumber = 4;
  const ::std::string& file() const;
  void set_file(const ::std::string& value);
  #if LANG_CXX11
  void set_file(::std::string&& value);
  #endif
  void set_file(const char* value);
  void set_file(const void* value, size_t size);
  ::std::string* mutable_file();
  ::std::string* release_file();
  void set_allocated_file(::std::string* file);

  // sfixed64 date = 3;
  void clear_date();
  static const int kDateFieldNumber = 3;
  ::google::protobuf::int64 date() const;
  void set_date(::google::protobuf::int64 value);

  // .tdrp.packet.STransferFile.Type type = 1;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  ::tdrp::packet::STransferFile_Type type() const;
  void set_type(::tdrp::packet::STransferFile_Type value);

  // @@protoc_insertion_point(class_scope:tdrp.packet.STransferFile)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::internal::ArenaStringPtr file_;
  ::google::protobuf::int64 date_;
  int type_;
  mutable int _cached_size_;
  friend struct ::protobuf_STransferFile_2eproto::TableStruct;
  friend void ::protobuf_STransferFile_2eproto::InitDefaultsSTransferFileImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// STransferFile

// .tdrp.packet.STransferFile.Type type = 1;
inline void STransferFile::clear_type() {
  type_ = 0;
}
inline ::tdrp::packet::STransferFile_Type STransferFile::type() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.STransferFile.type)
  return static_cast< ::tdrp::packet::STransferFile_Type >(type_);
}
inline void STransferFile::set_type(::tdrp::packet::STransferFile_Type value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.STransferFile.type)
}

// string name = 2;
inline void STransferFile::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& STransferFile::name() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.STransferFile.name)
  return name_.GetNoArena();
}
inline void STransferFile::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.STransferFile.name)
}
#if LANG_CXX11
inline void STransferFile::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.STransferFile.name)
}
#endif
inline void STransferFile::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.STransferFile.name)
}
inline void STransferFile::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.STransferFile.name)
}
inline ::std::string* STransferFile::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.STransferFile.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* STransferFile::release_name() {
  // @@protoc_insertion_point(field_release:tdrp.packet.STransferFile.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void STransferFile::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.STransferFile.name)
}

// sfixed64 date = 3;
inline void STransferFile::clear_date() {
  date_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 STransferFile::date() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.STransferFile.date)
  return date_;
}
inline void STransferFile::set_date(::google::protobuf::int64 value) {
  
  date_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.STransferFile.date)
}

// bytes file = 4;
inline void STransferFile::clear_file() {
  file_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& STransferFile::file() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.STransferFile.file)
  return file_.GetNoArena();
}
inline void STransferFile::set_file(const ::std::string& value) {
  
  file_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.STransferFile.file)
}
#if LANG_CXX11
inline void STransferFile::set_file(::std::string&& value) {
  
  file_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.STransferFile.file)
}
#endif
inline void STransferFile::set_file(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  file_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.STransferFile.file)
}
inline void STransferFile::set_file(const void* value, size_t size) {
  
  file_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.STransferFile.file)
}
inline ::std::string* STransferFile::mutable_file() {
  
  // @@protoc_insertion_point(field_mutable:tdrp.packet.STransferFile.file)
  return file_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* STransferFile::release_file() {
  // @@protoc_insertion_point(field_release:tdrp.packet.STransferFile.file)
  
  return file_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void STransferFile::set_allocated_file(::std::string* file) {
  if (file != NULL) {
    
  } else {
    
  }
  file_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), file);
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.STransferFile.file)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace packet
}  // namespace tdrp

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::tdrp::packet::STransferFile_Type> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::tdrp::packet::STransferFile_Type>() {
  return ::tdrp::packet::STransferFile_Type_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_STransferFile_2eproto__INCLUDED
