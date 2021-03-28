// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SClientScriptDelete.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_SClientScriptDelete_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_SClientScriptDelete_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3015000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3015006 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_SClientScriptDelete_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_SClientScriptDelete_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_SClientScriptDelete_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_SClientScriptDelete_2eproto_metadata_getter(int index);
namespace tdrp {
namespace packet {
class SClientScriptDelete;
struct SClientScriptDeleteDefaultTypeInternal;
extern SClientScriptDeleteDefaultTypeInternal _SClientScriptDelete_default_instance_;
}  // namespace packet
}  // namespace tdrp
PROTOBUF_NAMESPACE_OPEN
template<> ::tdrp::packet::SClientScriptDelete* Arena::CreateMaybeMessage<::tdrp::packet::SClientScriptDelete>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace tdrp {
namespace packet {

// ===================================================================

class SClientScriptDelete PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.SClientScriptDelete) */ {
 public:
  inline SClientScriptDelete() : SClientScriptDelete(nullptr) {}
  virtual ~SClientScriptDelete();
  explicit constexpr SClientScriptDelete(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  SClientScriptDelete(const SClientScriptDelete& from);
  SClientScriptDelete(SClientScriptDelete&& from) noexcept
    : SClientScriptDelete() {
    *this = ::std::move(from);
  }

  inline SClientScriptDelete& operator=(const SClientScriptDelete& from) {
    CopyFrom(from);
    return *this;
  }
  inline SClientScriptDelete& operator=(SClientScriptDelete&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const SClientScriptDelete& default_instance() {
    return *internal_default_instance();
  }
  static inline const SClientScriptDelete* internal_default_instance() {
    return reinterpret_cast<const SClientScriptDelete*>(
               &_SClientScriptDelete_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(SClientScriptDelete& a, SClientScriptDelete& b) {
    a.Swap(&b);
  }
  inline void Swap(SClientScriptDelete* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(SClientScriptDelete* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline SClientScriptDelete* New() const final {
    return CreateMaybeMessage<SClientScriptDelete>(nullptr);
  }

  SClientScriptDelete* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<SClientScriptDelete>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const SClientScriptDelete& from);
  void MergeFrom(const SClientScriptDelete& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(SClientScriptDelete* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "tdrp.packet.SClientScriptDelete";
  }
  protected:
  explicit SClientScriptDelete(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_SClientScriptDelete_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNameFieldNumber = 1,
  };
  // string name = 1;
  void clear_name();
  const std::string& name() const;
  void set_name(const std::string& value);
  void set_name(std::string&& value);
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  std::string* mutable_name();
  std::string* release_name();
  void set_allocated_name(std::string* name);
  private:
  const std::string& _internal_name() const;
  void _internal_set_name(const std::string& value);
  std::string* _internal_mutable_name();
  public:

  // @@protoc_insertion_point(class_scope:tdrp.packet.SClientScriptDelete)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_SClientScriptDelete_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SClientScriptDelete

// string name = 1;
inline void SClientScriptDelete::clear_name() {
  name_.ClearToEmpty();
}
inline const std::string& SClientScriptDelete::name() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SClientScriptDelete.name)
  return _internal_name();
}
inline void SClientScriptDelete::set_name(const std::string& value) {
  _internal_set_name(value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SClientScriptDelete.name)
}
inline std::string* SClientScriptDelete::mutable_name() {
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SClientScriptDelete.name)
  return _internal_mutable_name();
}
inline const std::string& SClientScriptDelete::_internal_name() const {
  return name_.Get();
}
inline void SClientScriptDelete::_internal_set_name(const std::string& value) {
  
  name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void SClientScriptDelete::set_name(std::string&& value) {
  
  name_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SClientScriptDelete.name)
}
inline void SClientScriptDelete::set_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SClientScriptDelete.name)
}
inline void SClientScriptDelete::set_name(const char* value,
    size_t size) {
  
  name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SClientScriptDelete.name)
}
inline std::string* SClientScriptDelete::_internal_mutable_name() {
  
  return name_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* SClientScriptDelete::release_name() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SClientScriptDelete.name)
  return name_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void SClientScriptDelete::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), name,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SClientScriptDelete.name)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_SClientScriptDelete_2eproto
