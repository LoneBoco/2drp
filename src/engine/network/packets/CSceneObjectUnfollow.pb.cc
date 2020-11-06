// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CSceneObjectUnfollow.proto

#include "CSceneObjectUnfollow.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace tdrp {
namespace packet {
class CSceneObjectUnfollowDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<CSceneObjectUnfollow>
      _instance;
} _CSceneObjectUnfollow_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace protobuf_CSceneObjectUnfollow_2eproto {
void InitDefaultsCSceneObjectUnfollowImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tdrp::packet::_CSceneObjectUnfollow_default_instance_;
    new (ptr) ::tdrp::packet::CSceneObjectUnfollow();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tdrp::packet::CSceneObjectUnfollow::InitAsDefaultInstance();
}

void InitDefaultsCSceneObjectUnfollow() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsCSceneObjectUnfollowImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::CSceneObjectUnfollow, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::CSceneObjectUnfollow, id_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tdrp::packet::CSceneObjectUnfollow)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tdrp::packet::_CSceneObjectUnfollow_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "CSceneObjectUnfollow.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\032CSceneObjectUnfollow.proto\022\013tdrp.packe"
      "t\"\"\n\024CSceneObjectUnfollow\022\n\n\002id\030\001 \003(\007b\006p"
      "roto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 85);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "CSceneObjectUnfollow.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_CSceneObjectUnfollow_2eproto
namespace tdrp {
namespace packet {

// ===================================================================

void CSceneObjectUnfollow::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int CSceneObjectUnfollow::kIdFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

CSceneObjectUnfollow::CSceneObjectUnfollow()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_CSceneObjectUnfollow_2eproto::InitDefaultsCSceneObjectUnfollow();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tdrp.packet.CSceneObjectUnfollow)
}
CSceneObjectUnfollow::CSceneObjectUnfollow(const CSceneObjectUnfollow& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      id_(from.id_),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:tdrp.packet.CSceneObjectUnfollow)
}

void CSceneObjectUnfollow::SharedCtor() {
  _cached_size_ = 0;
}

CSceneObjectUnfollow::~CSceneObjectUnfollow() {
  // @@protoc_insertion_point(destructor:tdrp.packet.CSceneObjectUnfollow)
  SharedDtor();
}

void CSceneObjectUnfollow::SharedDtor() {
}

void CSceneObjectUnfollow::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CSceneObjectUnfollow::descriptor() {
  ::protobuf_CSceneObjectUnfollow_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_CSceneObjectUnfollow_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const CSceneObjectUnfollow& CSceneObjectUnfollow::default_instance() {
  ::protobuf_CSceneObjectUnfollow_2eproto::InitDefaultsCSceneObjectUnfollow();
  return *internal_default_instance();
}

CSceneObjectUnfollow* CSceneObjectUnfollow::New(::google::protobuf::Arena* arena) const {
  CSceneObjectUnfollow* n = new CSceneObjectUnfollow;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void CSceneObjectUnfollow::Clear() {
// @@protoc_insertion_point(message_clear_start:tdrp.packet.CSceneObjectUnfollow)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  id_.Clear();
  _internal_metadata_.Clear();
}

bool CSceneObjectUnfollow::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tdrp.packet.CSceneObjectUnfollow)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated fixed32 id = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED32>(
                 input, this->mutable_id())));
        } else if (
            static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(13u /* 13 & 0xFF */)) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitiveNoInline<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED32>(
                 1, 10u, input, this->mutable_id())));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:tdrp.packet.CSceneObjectUnfollow)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tdrp.packet.CSceneObjectUnfollow)
  return false;
#undef DO_
}

void CSceneObjectUnfollow::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tdrp.packet.CSceneObjectUnfollow)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated fixed32 id = 1;
  if (this->id_size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteTag(1, ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
    output->WriteVarint32(static_cast< ::google::protobuf::uint32>(
        _id_cached_byte_size_));
    ::google::protobuf::internal::WireFormatLite::WriteFixed32Array(
      this->id().data(), this->id_size(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tdrp.packet.CSceneObjectUnfollow)
}

::google::protobuf::uint8* CSceneObjectUnfollow::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tdrp.packet.CSceneObjectUnfollow)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated fixed32 id = 1;
  if (this->id_size() > 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteTagToArray(
      1,
      ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
      target);
    target = ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(
        static_cast< ::google::protobuf::int32>(
            _id_cached_byte_size_), target);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteFixed32NoTagToArray(this->id_, target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tdrp.packet.CSceneObjectUnfollow)
  return target;
}

size_t CSceneObjectUnfollow::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tdrp.packet.CSceneObjectUnfollow)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // repeated fixed32 id = 1;
  {
    unsigned int count = static_cast<unsigned int>(this->id_size());
    size_t data_size = 4UL * count;
    if (data_size > 0) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
            static_cast< ::google::protobuf::int32>(data_size));
    }
    int cached_size = ::google::protobuf::internal::ToCachedSize(data_size);
    GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
    _id_cached_byte_size_ = cached_size;
    GOOGLE_SAFE_CONCURRENT_WRITES_END();
    total_size += data_size;
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void CSceneObjectUnfollow::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tdrp.packet.CSceneObjectUnfollow)
  GOOGLE_DCHECK_NE(&from, this);
  const CSceneObjectUnfollow* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const CSceneObjectUnfollow>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tdrp.packet.CSceneObjectUnfollow)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tdrp.packet.CSceneObjectUnfollow)
    MergeFrom(*source);
  }
}

void CSceneObjectUnfollow::MergeFrom(const CSceneObjectUnfollow& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tdrp.packet.CSceneObjectUnfollow)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  id_.MergeFrom(from.id_);
}

void CSceneObjectUnfollow::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tdrp.packet.CSceneObjectUnfollow)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CSceneObjectUnfollow::CopyFrom(const CSceneObjectUnfollow& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tdrp.packet.CSceneObjectUnfollow)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CSceneObjectUnfollow::IsInitialized() const {
  return true;
}

void CSceneObjectUnfollow::Swap(CSceneObjectUnfollow* other) {
  if (other == this) return;
  InternalSwap(other);
}
void CSceneObjectUnfollow::InternalSwap(CSceneObjectUnfollow* other) {
  using std::swap;
  id_.InternalSwap(&other->id_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata CSceneObjectUnfollow::GetMetadata() const {
  protobuf_CSceneObjectUnfollow_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_CSceneObjectUnfollow_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)