// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SSceneObjectChange.proto

#ifndef PROTOBUF_SSceneObjectChange_2eproto__INCLUDED
#define PROTOBUF_SSceneObjectChange_2eproto__INCLUDED

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

namespace protobuf_SSceneObjectChange_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[2];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsSSceneObjectChange_AttributeImpl();
void InitDefaultsSSceneObjectChange_Attribute();
void InitDefaultsSSceneObjectChangeImpl();
void InitDefaultsSSceneObjectChange();
inline void InitDefaults() {
  InitDefaultsSSceneObjectChange_Attribute();
  InitDefaultsSSceneObjectChange();
}
}  // namespace protobuf_SSceneObjectChange_2eproto
namespace tdrp {
namespace packet {
class SSceneObjectChange;
class SSceneObjectChangeDefaultTypeInternal;
extern SSceneObjectChangeDefaultTypeInternal _SSceneObjectChange_default_instance_;
class SSceneObjectChange_Attribute;
class SSceneObjectChange_AttributeDefaultTypeInternal;
extern SSceneObjectChange_AttributeDefaultTypeInternal _SSceneObjectChange_Attribute_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace tdrp {
namespace packet {

// ===================================================================

class SSceneObjectChange_Attribute : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.SSceneObjectChange.Attribute) */ {
 public:
  SSceneObjectChange_Attribute();
  virtual ~SSceneObjectChange_Attribute();

  SSceneObjectChange_Attribute(const SSceneObjectChange_Attribute& from);

  inline SSceneObjectChange_Attribute& operator=(const SSceneObjectChange_Attribute& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SSceneObjectChange_Attribute(SSceneObjectChange_Attribute&& from) noexcept
    : SSceneObjectChange_Attribute() {
    *this = ::std::move(from);
  }

  inline SSceneObjectChange_Attribute& operator=(SSceneObjectChange_Attribute&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const SSceneObjectChange_Attribute& default_instance();

  enum ValueCase {
    kAsInt = 2,
    kAsUint = 3,
    kAsFloat = 4,
    kAsDouble = 5,
    kAsString = 6,
    VALUE_NOT_SET = 0,
  };

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SSceneObjectChange_Attribute* internal_default_instance() {
    return reinterpret_cast<const SSceneObjectChange_Attribute*>(
               &_SSceneObjectChange_Attribute_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(SSceneObjectChange_Attribute* other);
  friend void swap(SSceneObjectChange_Attribute& a, SSceneObjectChange_Attribute& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SSceneObjectChange_Attribute* New() const PROTOBUF_FINAL { return New(NULL); }

  SSceneObjectChange_Attribute* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const SSceneObjectChange_Attribute& from);
  void MergeFrom(const SSceneObjectChange_Attribute& from);
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
  void InternalSwap(SSceneObjectChange_Attribute* other);
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

  // uint32 id = 1;
  void clear_id();
  static const int kIdFieldNumber = 1;
  ::google::protobuf::uint32 id() const;
  void set_id(::google::protobuf::uint32 value);

  // int64 as_int = 2;
  private:
  bool has_as_int() const;
  public:
  void clear_as_int();
  static const int kAsIntFieldNumber = 2;
  ::google::protobuf::int64 as_int() const;
  void set_as_int(::google::protobuf::int64 value);

  // uint64 as_uint = 3;
  private:
  bool has_as_uint() const;
  public:
  void clear_as_uint();
  static const int kAsUintFieldNumber = 3;
  ::google::protobuf::uint64 as_uint() const;
  void set_as_uint(::google::protobuf::uint64 value);

  // float as_float = 4;
  private:
  bool has_as_float() const;
  public:
  void clear_as_float();
  static const int kAsFloatFieldNumber = 4;
  float as_float() const;
  void set_as_float(float value);

  // double as_double = 5;
  private:
  bool has_as_double() const;
  public:
  void clear_as_double();
  static const int kAsDoubleFieldNumber = 5;
  double as_double() const;
  void set_as_double(double value);

  // string as_string = 6;
  private:
  bool has_as_string() const;
  public:
  void clear_as_string();
  static const int kAsStringFieldNumber = 6;
  const ::std::string& as_string() const;
  void set_as_string(const ::std::string& value);
  #if LANG_CXX11
  void set_as_string(::std::string&& value);
  #endif
  void set_as_string(const char* value);
  void set_as_string(const char* value, size_t size);
  ::std::string* mutable_as_string();
  ::std::string* release_as_string();
  void set_allocated_as_string(::std::string* as_string);

  ValueCase value_case() const;
  // @@protoc_insertion_point(class_scope:tdrp.packet.SSceneObjectChange.Attribute)
 private:
  void set_has_as_int();
  void set_has_as_uint();
  void set_has_as_float();
  void set_has_as_double();
  void set_has_as_string();

  inline bool has_value() const;
  void clear_value();
  inline void clear_has_value();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 id_;
  union ValueUnion {
    ValueUnion() {}
    ::google::protobuf::int64 as_int_;
    ::google::protobuf::uint64 as_uint_;
    float as_float_;
    double as_double_;
    ::google::protobuf::internal::ArenaStringPtr as_string_;
  } value_;
  mutable int _cached_size_;
  ::google::protobuf::uint32 _oneof_case_[1];

  friend struct ::protobuf_SSceneObjectChange_2eproto::TableStruct;
  friend void ::protobuf_SSceneObjectChange_2eproto::InitDefaultsSSceneObjectChange_AttributeImpl();
};
// -------------------------------------------------------------------

class SSceneObjectChange : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.SSceneObjectChange) */ {
 public:
  SSceneObjectChange();
  virtual ~SSceneObjectChange();

  SSceneObjectChange(const SSceneObjectChange& from);

  inline SSceneObjectChange& operator=(const SSceneObjectChange& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SSceneObjectChange(SSceneObjectChange&& from) noexcept
    : SSceneObjectChange() {
    *this = ::std::move(from);
  }

  inline SSceneObjectChange& operator=(SSceneObjectChange&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const SSceneObjectChange& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SSceneObjectChange* internal_default_instance() {
    return reinterpret_cast<const SSceneObjectChange*>(
               &_SSceneObjectChange_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(SSceneObjectChange* other);
  friend void swap(SSceneObjectChange& a, SSceneObjectChange& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SSceneObjectChange* New() const PROTOBUF_FINAL { return New(NULL); }

  SSceneObjectChange* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const SSceneObjectChange& from);
  void MergeFrom(const SSceneObjectChange& from);
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
  void InternalSwap(SSceneObjectChange* other);
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

  typedef SSceneObjectChange_Attribute Attribute;

  // accessors -------------------------------------------------------

  // repeated .tdrp.packet.SSceneObjectChange.Attribute properties = 2;
  int properties_size() const;
  void clear_properties();
  static const int kPropertiesFieldNumber = 2;
  const ::tdrp::packet::SSceneObjectChange_Attribute& properties(int index) const;
  ::tdrp::packet::SSceneObjectChange_Attribute* mutable_properties(int index);
  ::tdrp::packet::SSceneObjectChange_Attribute* add_properties();
  ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >*
      mutable_properties();
  const ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >&
      properties() const;

  // repeated .tdrp.packet.SSceneObjectChange.Attribute attributes = 3;
  int attributes_size() const;
  void clear_attributes();
  static const int kAttributesFieldNumber = 3;
  const ::tdrp::packet::SSceneObjectChange_Attribute& attributes(int index) const;
  ::tdrp::packet::SSceneObjectChange_Attribute* mutable_attributes(int index);
  ::tdrp::packet::SSceneObjectChange_Attribute* add_attributes();
  ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >*
      mutable_attributes();
  const ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >&
      attributes() const;

  // fixed32 id = 1;
  void clear_id();
  static const int kIdFieldNumber = 1;
  ::google::protobuf::uint32 id() const;
  void set_id(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:tdrp.packet.SSceneObjectChange)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute > properties_;
  ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute > attributes_;
  ::google::protobuf::uint32 id_;
  mutable int _cached_size_;
  friend struct ::protobuf_SSceneObjectChange_2eproto::TableStruct;
  friend void ::protobuf_SSceneObjectChange_2eproto::InitDefaultsSSceneObjectChangeImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SSceneObjectChange_Attribute

// uint32 id = 1;
inline void SSceneObjectChange_Attribute::clear_id() {
  id_ = 0u;
}
inline ::google::protobuf::uint32 SSceneObjectChange_Attribute::id() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.Attribute.id)
  return id_;
}
inline void SSceneObjectChange_Attribute::set_id(::google::protobuf::uint32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.id)
}

// int64 as_int = 2;
inline bool SSceneObjectChange_Attribute::has_as_int() const {
  return value_case() == kAsInt;
}
inline void SSceneObjectChange_Attribute::set_has_as_int() {
  _oneof_case_[0] = kAsInt;
}
inline void SSceneObjectChange_Attribute::clear_as_int() {
  if (has_as_int()) {
    value_.as_int_ = GOOGLE_LONGLONG(0);
    clear_has_value();
  }
}
inline ::google::protobuf::int64 SSceneObjectChange_Attribute::as_int() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.Attribute.as_int)
  if (has_as_int()) {
    return value_.as_int_;
  }
  return GOOGLE_LONGLONG(0);
}
inline void SSceneObjectChange_Attribute::set_as_int(::google::protobuf::int64 value) {
  if (!has_as_int()) {
    clear_value();
    set_has_as_int();
  }
  value_.as_int_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_int)
}

// uint64 as_uint = 3;
inline bool SSceneObjectChange_Attribute::has_as_uint() const {
  return value_case() == kAsUint;
}
inline void SSceneObjectChange_Attribute::set_has_as_uint() {
  _oneof_case_[0] = kAsUint;
}
inline void SSceneObjectChange_Attribute::clear_as_uint() {
  if (has_as_uint()) {
    value_.as_uint_ = GOOGLE_ULONGLONG(0);
    clear_has_value();
  }
}
inline ::google::protobuf::uint64 SSceneObjectChange_Attribute::as_uint() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.Attribute.as_uint)
  if (has_as_uint()) {
    return value_.as_uint_;
  }
  return GOOGLE_ULONGLONG(0);
}
inline void SSceneObjectChange_Attribute::set_as_uint(::google::protobuf::uint64 value) {
  if (!has_as_uint()) {
    clear_value();
    set_has_as_uint();
  }
  value_.as_uint_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_uint)
}

// float as_float = 4;
inline bool SSceneObjectChange_Attribute::has_as_float() const {
  return value_case() == kAsFloat;
}
inline void SSceneObjectChange_Attribute::set_has_as_float() {
  _oneof_case_[0] = kAsFloat;
}
inline void SSceneObjectChange_Attribute::clear_as_float() {
  if (has_as_float()) {
    value_.as_float_ = 0;
    clear_has_value();
  }
}
inline float SSceneObjectChange_Attribute::as_float() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.Attribute.as_float)
  if (has_as_float()) {
    return value_.as_float_;
  }
  return 0;
}
inline void SSceneObjectChange_Attribute::set_as_float(float value) {
  if (!has_as_float()) {
    clear_value();
    set_has_as_float();
  }
  value_.as_float_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_float)
}

// double as_double = 5;
inline bool SSceneObjectChange_Attribute::has_as_double() const {
  return value_case() == kAsDouble;
}
inline void SSceneObjectChange_Attribute::set_has_as_double() {
  _oneof_case_[0] = kAsDouble;
}
inline void SSceneObjectChange_Attribute::clear_as_double() {
  if (has_as_double()) {
    value_.as_double_ = 0;
    clear_has_value();
  }
}
inline double SSceneObjectChange_Attribute::as_double() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.Attribute.as_double)
  if (has_as_double()) {
    return value_.as_double_;
  }
  return 0;
}
inline void SSceneObjectChange_Attribute::set_as_double(double value) {
  if (!has_as_double()) {
    clear_value();
    set_has_as_double();
  }
  value_.as_double_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_double)
}

// string as_string = 6;
inline bool SSceneObjectChange_Attribute::has_as_string() const {
  return value_case() == kAsString;
}
inline void SSceneObjectChange_Attribute::set_has_as_string() {
  _oneof_case_[0] = kAsString;
}
inline void SSceneObjectChange_Attribute::clear_as_string() {
  if (has_as_string()) {
    value_.as_string_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    clear_has_value();
  }
}
inline const ::std::string& SSceneObjectChange_Attribute::as_string() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.Attribute.as_string)
  if (has_as_string()) {
    return value_.as_string_.GetNoArena();
  }
  return *&::google::protobuf::internal::GetEmptyStringAlreadyInited();
}
inline void SSceneObjectChange_Attribute::set_as_string(const ::std::string& value) {
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_string)
  if (!has_as_string()) {
    clear_value();
    set_has_as_string();
    value_.as_string_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  value_.as_string_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_string)
}
#if LANG_CXX11
inline void SSceneObjectChange_Attribute::set_as_string(::std::string&& value) {
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.Attribute.as_string)
  if (!has_as_string()) {
    clear_value();
    set_has_as_string();
    value_.as_string_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  value_.as_string_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SSceneObjectChange.Attribute.as_string)
}
#endif
inline void SSceneObjectChange_Attribute::set_as_string(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  if (!has_as_string()) {
    clear_value();
    set_has_as_string();
    value_.as_string_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  value_.as_string_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SSceneObjectChange.Attribute.as_string)
}
inline void SSceneObjectChange_Attribute::set_as_string(const char* value, size_t size) {
  if (!has_as_string()) {
    clear_value();
    set_has_as_string();
    value_.as_string_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  value_.as_string_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SSceneObjectChange.Attribute.as_string)
}
inline ::std::string* SSceneObjectChange_Attribute::mutable_as_string() {
  if (!has_as_string()) {
    clear_value();
    set_has_as_string();
    value_.as_string_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SSceneObjectChange.Attribute.as_string)
  return value_.as_string_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SSceneObjectChange_Attribute::release_as_string() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SSceneObjectChange.Attribute.as_string)
  if (has_as_string()) {
    clear_has_value();
    return value_.as_string_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  } else {
    return NULL;
  }
}
inline void SSceneObjectChange_Attribute::set_allocated_as_string(::std::string* as_string) {
  if (!has_as_string()) {
    value_.as_string_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  clear_value();
  if (as_string != NULL) {
    set_has_as_string();
    value_.as_string_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
        as_string);
  }
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SSceneObjectChange.Attribute.as_string)
}

inline bool SSceneObjectChange_Attribute::has_value() const {
  return value_case() != VALUE_NOT_SET;
}
inline void SSceneObjectChange_Attribute::clear_has_value() {
  _oneof_case_[0] = VALUE_NOT_SET;
}
inline SSceneObjectChange_Attribute::ValueCase SSceneObjectChange_Attribute::value_case() const {
  return SSceneObjectChange_Attribute::ValueCase(_oneof_case_[0]);
}
// -------------------------------------------------------------------

// SSceneObjectChange

// fixed32 id = 1;
inline void SSceneObjectChange::clear_id() {
  id_ = 0u;
}
inline ::google::protobuf::uint32 SSceneObjectChange::id() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.id)
  return id_;
}
inline void SSceneObjectChange::set_id(::google::protobuf::uint32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:tdrp.packet.SSceneObjectChange.id)
}

// repeated .tdrp.packet.SSceneObjectChange.Attribute properties = 2;
inline int SSceneObjectChange::properties_size() const {
  return properties_.size();
}
inline void SSceneObjectChange::clear_properties() {
  properties_.Clear();
}
inline const ::tdrp::packet::SSceneObjectChange_Attribute& SSceneObjectChange::properties(int index) const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.properties)
  return properties_.Get(index);
}
inline ::tdrp::packet::SSceneObjectChange_Attribute* SSceneObjectChange::mutable_properties(int index) {
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SSceneObjectChange.properties)
  return properties_.Mutable(index);
}
inline ::tdrp::packet::SSceneObjectChange_Attribute* SSceneObjectChange::add_properties() {
  // @@protoc_insertion_point(field_add:tdrp.packet.SSceneObjectChange.properties)
  return properties_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >*
SSceneObjectChange::mutable_properties() {
  // @@protoc_insertion_point(field_mutable_list:tdrp.packet.SSceneObjectChange.properties)
  return &properties_;
}
inline const ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >&
SSceneObjectChange::properties() const {
  // @@protoc_insertion_point(field_list:tdrp.packet.SSceneObjectChange.properties)
  return properties_;
}

// repeated .tdrp.packet.SSceneObjectChange.Attribute attributes = 3;
inline int SSceneObjectChange::attributes_size() const {
  return attributes_.size();
}
inline void SSceneObjectChange::clear_attributes() {
  attributes_.Clear();
}
inline const ::tdrp::packet::SSceneObjectChange_Attribute& SSceneObjectChange::attributes(int index) const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SSceneObjectChange.attributes)
  return attributes_.Get(index);
}
inline ::tdrp::packet::SSceneObjectChange_Attribute* SSceneObjectChange::mutable_attributes(int index) {
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SSceneObjectChange.attributes)
  return attributes_.Mutable(index);
}
inline ::tdrp::packet::SSceneObjectChange_Attribute* SSceneObjectChange::add_attributes() {
  // @@protoc_insertion_point(field_add:tdrp.packet.SSceneObjectChange.attributes)
  return attributes_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >*
SSceneObjectChange::mutable_attributes() {
  // @@protoc_insertion_point(field_mutable_list:tdrp.packet.SSceneObjectChange.attributes)
  return &attributes_;
}
inline const ::google::protobuf::RepeatedPtrField< ::tdrp::packet::SSceneObjectChange_Attribute >&
SSceneObjectChange::attributes() const {
  // @@protoc_insertion_point(field_list:tdrp.packet.SSceneObjectChange.attributes)
  return attributes_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_SSceneObjectChange_2eproto__INCLUDED
