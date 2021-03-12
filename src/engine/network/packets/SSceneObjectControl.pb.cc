// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SSceneObjectControl.proto

#include "SSceneObjectControl.pb.h"

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
class SSceneObjectControlDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<SSceneObjectControl>
      _instance;
} _SSceneObjectControl_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace protobuf_SSceneObjectControl_2eproto {
void InitDefaultsSSceneObjectControlImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tdrp::packet::_SSceneObjectControl_default_instance_;
    new (ptr) ::tdrp::packet::SSceneObjectControl();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tdrp::packet::SSceneObjectControl::InitAsDefaultInstance();
}

void InitDefaultsSSceneObjectControl() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsSSceneObjectControlImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSceneObjectControl, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSceneObjectControl, old_id_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSceneObjectControl, new_id_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tdrp::packet::SSceneObjectControl)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tdrp::packet::_SSceneObjectControl_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "SSceneObjectControl.proto", schemas, file_default_instances, TableStruct::offsets, factory,
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
      "\n\031SSceneObjectControl.proto\022\013tdrp.packet"
      "\"5\n\023SSceneObjectControl\022\016\n\006old_id\030\001 \001(\007\022"
      "\016\n\006new_id\030\002 \001(\007b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 103);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "SSceneObjectControl.proto", &protobuf_RegisterTypes);
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
}  // namespace protobuf_SSceneObjectControl_2eproto
namespace tdrp {
namespace packet {

// ===================================================================

void SSceneObjectControl::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int SSceneObjectControl::kOldIdFieldNumber;
const int SSceneObjectControl::kNewIdFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

SSceneObjectControl::SSceneObjectControl()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_SSceneObjectControl_2eproto::InitDefaultsSSceneObjectControl();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tdrp.packet.SSceneObjectControl)
}
SSceneObjectControl::SSceneObjectControl(const SSceneObjectControl& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&old_id_, &from.old_id_,
    static_cast<size_t>(reinterpret_cast<char*>(&new_id_) -
    reinterpret_cast<char*>(&old_id_)) + sizeof(new_id_));
  // @@protoc_insertion_point(copy_constructor:tdrp.packet.SSceneObjectControl)
}

void SSceneObjectControl::SharedCtor() {
  ::memset(&old_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&new_id_) -
      reinterpret_cast<char*>(&old_id_)) + sizeof(new_id_));
  _cached_size_ = 0;
}

SSceneObjectControl::~SSceneObjectControl() {
  // @@protoc_insertion_point(destructor:tdrp.packet.SSceneObjectControl)
  SharedDtor();
}

void SSceneObjectControl::SharedDtor() {
}

void SSceneObjectControl::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SSceneObjectControl::descriptor() {
  ::protobuf_SSceneObjectControl_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SSceneObjectControl_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const SSceneObjectControl& SSceneObjectControl::default_instance() {
  ::protobuf_SSceneObjectControl_2eproto::InitDefaultsSSceneObjectControl();
  return *internal_default_instance();
}

SSceneObjectControl* SSceneObjectControl::New(::google::protobuf::Arena* arena) const {
  SSceneObjectControl* n = new SSceneObjectControl;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void SSceneObjectControl::Clear() {
// @@protoc_insertion_point(message_clear_start:tdrp.packet.SSceneObjectControl)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&old_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&new_id_) -
      reinterpret_cast<char*>(&old_id_)) + sizeof(new_id_));
  _internal_metadata_.Clear();
}

bool SSceneObjectControl::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tdrp.packet.SSceneObjectControl)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // fixed32 old_id = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(13u /* 13 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED32>(
                 input, &old_id_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // fixed32 new_id = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(21u /* 21 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED32>(
                 input, &new_id_)));
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
  // @@protoc_insertion_point(parse_success:tdrp.packet.SSceneObjectControl)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tdrp.packet.SSceneObjectControl)
  return false;
#undef DO_
}

void SSceneObjectControl::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tdrp.packet.SSceneObjectControl)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // fixed32 old_id = 1;
  if (this->old_id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed32(1, this->old_id(), output);
  }

  // fixed32 new_id = 2;
  if (this->new_id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed32(2, this->new_id(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tdrp.packet.SSceneObjectControl)
}

::google::protobuf::uint8* SSceneObjectControl::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tdrp.packet.SSceneObjectControl)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // fixed32 old_id = 1;
  if (this->old_id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed32ToArray(1, this->old_id(), target);
  }

  // fixed32 new_id = 2;
  if (this->new_id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed32ToArray(2, this->new_id(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tdrp.packet.SSceneObjectControl)
  return target;
}

size_t SSceneObjectControl::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tdrp.packet.SSceneObjectControl)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // fixed32 old_id = 1;
  if (this->old_id() != 0) {
    total_size += 1 + 4;
  }

  // fixed32 new_id = 2;
  if (this->new_id() != 0) {
    total_size += 1 + 4;
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SSceneObjectControl::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tdrp.packet.SSceneObjectControl)
  GOOGLE_DCHECK_NE(&from, this);
  const SSceneObjectControl* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const SSceneObjectControl>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tdrp.packet.SSceneObjectControl)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tdrp.packet.SSceneObjectControl)
    MergeFrom(*source);
  }
}

void SSceneObjectControl::MergeFrom(const SSceneObjectControl& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tdrp.packet.SSceneObjectControl)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.old_id() != 0) {
    set_old_id(from.old_id());
  }
  if (from.new_id() != 0) {
    set_new_id(from.new_id());
  }
}

void SSceneObjectControl::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tdrp.packet.SSceneObjectControl)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SSceneObjectControl::CopyFrom(const SSceneObjectControl& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tdrp.packet.SSceneObjectControl)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SSceneObjectControl::IsInitialized() const {
  return true;
}

void SSceneObjectControl::Swap(SSceneObjectControl* other) {
  if (other == this) return;
  InternalSwap(other);
}
void SSceneObjectControl::InternalSwap(SSceneObjectControl* other) {
  using std::swap;
  swap(old_id_, other->old_id_);
  swap(new_id_, other->new_id_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata SSceneObjectControl::GetMetadata() const {
  protobuf_SSceneObjectControl_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SSceneObjectControl_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)
