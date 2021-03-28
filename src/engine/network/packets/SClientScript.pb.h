// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SClientScript.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_SClientScript_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_SClientScript_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_SClientScript_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_SClientScript_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_SClientScript_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_SClientScript_2eproto_metadata_getter(int index);
namespace tdrp {
namespace packet {
class SClientScript;
struct SClientScriptDefaultTypeInternal;
extern SClientScriptDefaultTypeInternal _SClientScript_default_instance_;
}  // namespace packet
}  // namespace tdrp
PROTOBUF_NAMESPACE_OPEN
template<> ::tdrp::packet::SClientScript* Arena::CreateMaybeMessage<::tdrp::packet::SClientScript>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace tdrp {
namespace packet {

// ===================================================================

class SClientScript PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:tdrp.packet.SClientScript) */ {
 public:
  inline SClientScript() : SClientScript(nullptr) {}
  virtual ~SClientScript();
  explicit constexpr SClientScript(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  SClientScript(const SClientScript& from);
  SClientScript(SClientScript&& from) noexcept
    : SClientScript() {
    *this = ::std::move(from);
  }

  inline SClientScript& operator=(const SClientScript& from) {
    CopyFrom(from);
    return *this;
  }
  inline SClientScript& operator=(SClientScript&& from) noexcept {
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
  static const SClientScript& default_instance() {
    return *internal_default_instance();
  }
  static inline const SClientScript* internal_default_instance() {
    return reinterpret_cast<const SClientScript*>(
               &_SClientScript_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(SClientScript& a, SClientScript& b) {
    a.Swap(&b);
  }
  inline void Swap(SClientScript* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(SClientScript* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline SClientScript* New() const final {
    return CreateMaybeMessage<SClientScript>(nullptr);
  }

  SClientScript* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<SClientScript>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const SClientScript& from);
  void MergeFrom(const SClientScript& from);
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
  void InternalSwap(SClientScript* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "tdrp.packet.SClientScript";
  }
  protected:
  explicit SClientScript(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_SClientScript_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNameFieldNumber = 1,
    kScriptFieldNumber = 2,
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

  // string script = 2;
  void clear_script();
  const std::string& script() const;
  void set_script(const std::string& value);
  void set_script(std::string&& value);
  void set_script(const char* value);
  void set_script(const char* value, size_t size);
  std::string* mutable_script();
  std::string* release_script();
  void set_allocated_script(std::string* script);
  private:
  const std::string& _internal_script() const;
  void _internal_set_script(const std::string& value);
  std::string* _internal_mutable_script();
  public:

  // @@protoc_insertion_point(class_scope:tdrp.packet.SClientScript)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr script_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_SClientScript_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SClientScript

// string name = 1;
inline void SClientScript::clear_name() {
  name_.ClearToEmpty();
}
inline const std::string& SClientScript::name() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SClientScript.name)
  return _internal_name();
}
inline void SClientScript::set_name(const std::string& value) {
  _internal_set_name(value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SClientScript.name)
}
inline std::string* SClientScript::mutable_name() {
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SClientScript.name)
  return _internal_mutable_name();
}
inline const std::string& SClientScript::_internal_name() const {
  return name_.Get();
}
inline void SClientScript::_internal_set_name(const std::string& value) {
  
  name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void SClientScript::set_name(std::string&& value) {
  
  name_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SClientScript.name)
}
inline void SClientScript::set_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SClientScript.name)
}
inline void SClientScript::set_name(const char* value,
    size_t size) {
  
  name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SClientScript.name)
}
inline std::string* SClientScript::_internal_mutable_name() {
  
  return name_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* SClientScript::release_name() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SClientScript.name)
  return name_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void SClientScript::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), name,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SClientScript.name)
}

// string script = 2;
inline void SClientScript::clear_script() {
  script_.ClearToEmpty();
}
inline const std::string& SClientScript::script() const {
  // @@protoc_insertion_point(field_get:tdrp.packet.SClientScript.script)
  return _internal_script();
}
inline void SClientScript::set_script(const std::string& value) {
  _internal_set_script(value);
  // @@protoc_insertion_point(field_set:tdrp.packet.SClientScript.script)
}
inline std::string* SClientScript::mutable_script() {
  // @@protoc_insertion_point(field_mutable:tdrp.packet.SClientScript.script)
  return _internal_mutable_script();
}
inline const std::string& SClientScript::_internal_script() const {
  return script_.Get();
}
inline void SClientScript::_internal_set_script(const std::string& value) {
  
  script_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void SClientScript::set_script(std::string&& value) {
  
  script_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:tdrp.packet.SClientScript.script)
}
inline void SClientScript::set_script(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  script_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:tdrp.packet.SClientScript.script)
}
inline void SClientScript::set_script(const char* value,
    size_t size) {
  
  script_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:tdrp.packet.SClientScript.script)
}
inline std::string* SClientScript::_internal_mutable_script() {
  
  return script_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* SClientScript::release_script() {
  // @@protoc_insertion_point(field_release:tdrp.packet.SClientScript.script)
  return script_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void SClientScript::set_allocated_script(std::string* script) {
  if (script != nullptr) {
    
  } else {
    
  }
  script_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), script,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:tdrp.packet.SClientScript.script)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_SClientScript_2eproto
