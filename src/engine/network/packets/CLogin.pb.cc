// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CLogin.proto

#include "CLogin.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace tdrp {
namespace packet {
constexpr CLogin::CLogin(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : account_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , passwordhash_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , version_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , type_(0u){}
struct CLoginDefaultTypeInternal {
  constexpr CLoginDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CLoginDefaultTypeInternal() {}
  union {
    CLogin _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CLoginDefaultTypeInternal _CLogin_default_instance_;
}  // namespace packet
}  // namespace tdrp
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_CLogin_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_CLogin_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_CLogin_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_CLogin_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::tdrp::packet::CLogin, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::tdrp::packet::CLogin, account_),
  PROTOBUF_FIELD_OFFSET(::tdrp::packet::CLogin, passwordhash_),
  PROTOBUF_FIELD_OFFSET(::tdrp::packet::CLogin, type_),
  PROTOBUF_FIELD_OFFSET(::tdrp::packet::CLogin, version_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tdrp::packet::CLogin)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::tdrp::packet::_CLogin_default_instance_),
};

const char descriptor_table_protodef_CLogin_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\014CLogin.proto\022\013tdrp.packet\"N\n\006CLogin\022\017\n"
  "\007account\030\001 \001(\t\022\024\n\014passwordhash\030\002 \001(\t\022\014\n\004"
  "type\030\003 \001(\r\022\017\n\007version\030\004 \001(\tb\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_CLogin_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_CLogin_2eproto = {
  false, false, 115, descriptor_table_protodef_CLogin_2eproto, "CLogin.proto", 
  &descriptor_table_CLogin_2eproto_once, nullptr, 0, 1,
  schemas, file_default_instances, TableStruct_CLogin_2eproto::offsets,
  file_level_metadata_CLogin_2eproto, file_level_enum_descriptors_CLogin_2eproto, file_level_service_descriptors_CLogin_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK ::PROTOBUF_NAMESPACE_ID::Metadata
descriptor_table_CLogin_2eproto_metadata_getter(int index) {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_CLogin_2eproto);
  return descriptor_table_CLogin_2eproto.file_level_metadata[index];
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_CLogin_2eproto(&descriptor_table_CLogin_2eproto);
namespace tdrp {
namespace packet {

// ===================================================================

class CLogin::_Internal {
 public:
};

CLogin::CLogin(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:tdrp.packet.CLogin)
}
CLogin::CLogin(const CLogin& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  account_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_account().empty()) {
    account_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_account(), 
      GetArena());
  }
  passwordhash_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_passwordhash().empty()) {
    passwordhash_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_passwordhash(), 
      GetArena());
  }
  version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_version().empty()) {
    version_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_version(), 
      GetArena());
  }
  type_ = from.type_;
  // @@protoc_insertion_point(copy_constructor:tdrp.packet.CLogin)
}

void CLogin::SharedCtor() {
account_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
passwordhash_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
type_ = 0u;
}

CLogin::~CLogin() {
  // @@protoc_insertion_point(destructor:tdrp.packet.CLogin)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void CLogin::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  account_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  passwordhash_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  version_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CLogin::ArenaDtor(void* object) {
  CLogin* _this = reinterpret_cast< CLogin* >(object);
  (void)_this;
}
void CLogin::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CLogin::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CLogin::Clear() {
// @@protoc_insertion_point(message_clear_start:tdrp.packet.CLogin)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  account_.ClearToEmpty();
  passwordhash_.ClearToEmpty();
  version_.ClearToEmpty();
  type_ = 0u;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* CLogin::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string account = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_account();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "tdrp.packet.CLogin.account"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string passwordhash = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_passwordhash();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "tdrp.packet.CLogin.passwordhash"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 type = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string version = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          auto str = _internal_mutable_version();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "tdrp.packet.CLogin.version"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CLogin::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:tdrp.packet.CLogin)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string account = 1;
  if (this->account().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_account().data(), static_cast<int>(this->_internal_account().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.CLogin.account");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_account(), target);
  }

  // string passwordhash = 2;
  if (this->passwordhash().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_passwordhash().data(), static_cast<int>(this->_internal_passwordhash().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.CLogin.passwordhash");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_passwordhash(), target);
  }

  // uint32 type = 3;
  if (this->type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_type(), target);
  }

  // string version = 4;
  if (this->version().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_version().data(), static_cast<int>(this->_internal_version().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.CLogin.version");
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_version(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tdrp.packet.CLogin)
  return target;
}

size_t CLogin::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tdrp.packet.CLogin)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string account = 1;
  if (this->account().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_account());
  }

  // string passwordhash = 2;
  if (this->passwordhash().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_passwordhash());
  }

  // string version = 4;
  if (this->version().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_version());
  }

  // uint32 type = 3;
  if (this->type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_type());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CLogin::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tdrp.packet.CLogin)
  GOOGLE_DCHECK_NE(&from, this);
  const CLogin* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<CLogin>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tdrp.packet.CLogin)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tdrp.packet.CLogin)
    MergeFrom(*source);
  }
}

void CLogin::MergeFrom(const CLogin& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tdrp.packet.CLogin)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.account().size() > 0) {
    _internal_set_account(from._internal_account());
  }
  if (from.passwordhash().size() > 0) {
    _internal_set_passwordhash(from._internal_passwordhash());
  }
  if (from.version().size() > 0) {
    _internal_set_version(from._internal_version());
  }
  if (from.type() != 0) {
    _internal_set_type(from._internal_type());
  }
}

void CLogin::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tdrp.packet.CLogin)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CLogin::CopyFrom(const CLogin& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tdrp.packet.CLogin)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CLogin::IsInitialized() const {
  return true;
}

void CLogin::InternalSwap(CLogin* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  account_.Swap(&other->account_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  passwordhash_.Swap(&other->passwordhash_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  version_.Swap(&other->version_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(type_, other->type_);
}

::PROTOBUF_NAMESPACE_ID::Metadata CLogin::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace packet
}  // namespace tdrp
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::tdrp::packet::CLogin* Arena::CreateMaybeMessage< ::tdrp::packet::CLogin >(Arena* arena) {
  return Arena::CreateMessageInternal< ::tdrp::packet::CLogin >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
