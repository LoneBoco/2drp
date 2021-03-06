// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SSendEvent.proto

#include "SSendEvent.pb.h"

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
class SSendEventDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<SSendEvent>
      _instance;
} _SSendEvent_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace protobuf_SSendEvent_2eproto {
void InitDefaultsSSendEventImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tdrp::packet::_SSendEvent_default_instance_;
    new (ptr) ::tdrp::packet::SSendEvent();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tdrp::packet::SSendEvent::InitAsDefaultInstance();
}

void InitDefaultsSSendEvent() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsSSendEventImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, sender_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, name_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, data_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, x_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, y_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SSendEvent, radius_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tdrp::packet::SSendEvent)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tdrp::packet::_SSendEvent_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "SSendEvent.proto", schemas, file_default_instances, TableStruct::offsets, factory,
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
      "\n\020SSendEvent.proto\022\013tdrp.packet\"^\n\nSSend"
      "Event\022\016\n\006sender\030\001 \001(\007\022\014\n\004name\030\002 \001(\t\022\014\n\004d"
      "ata\030\003 \001(\t\022\t\n\001x\030\004 \001(\002\022\t\n\001y\030\005 \001(\002\022\016\n\006radiu"
      "s\030\006 \001(\002b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 135);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "SSendEvent.proto", &protobuf_RegisterTypes);
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
}  // namespace protobuf_SSendEvent_2eproto
namespace tdrp {
namespace packet {

// ===================================================================

void SSendEvent::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int SSendEvent::kSenderFieldNumber;
const int SSendEvent::kNameFieldNumber;
const int SSendEvent::kDataFieldNumber;
const int SSendEvent::kXFieldNumber;
const int SSendEvent::kYFieldNumber;
const int SSendEvent::kRadiusFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

SSendEvent::SSendEvent()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_SSendEvent_2eproto::InitDefaultsSSendEvent();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tdrp.packet.SSendEvent)
}
SSendEvent::SSendEvent(const SSendEvent& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.name().size() > 0) {
    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  data_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.data().size() > 0) {
    data_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.data_);
  }
  ::memcpy(&sender_, &from.sender_,
    static_cast<size_t>(reinterpret_cast<char*>(&radius_) -
    reinterpret_cast<char*>(&sender_)) + sizeof(radius_));
  // @@protoc_insertion_point(copy_constructor:tdrp.packet.SSendEvent)
}

void SSendEvent::SharedCtor() {
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  data_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&sender_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&radius_) -
      reinterpret_cast<char*>(&sender_)) + sizeof(radius_));
  _cached_size_ = 0;
}

SSendEvent::~SSendEvent() {
  // @@protoc_insertion_point(destructor:tdrp.packet.SSendEvent)
  SharedDtor();
}

void SSendEvent::SharedDtor() {
  name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  data_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void SSendEvent::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SSendEvent::descriptor() {
  ::protobuf_SSendEvent_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SSendEvent_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const SSendEvent& SSendEvent::default_instance() {
  ::protobuf_SSendEvent_2eproto::InitDefaultsSSendEvent();
  return *internal_default_instance();
}

SSendEvent* SSendEvent::New(::google::protobuf::Arena* arena) const {
  SSendEvent* n = new SSendEvent;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void SSendEvent::Clear() {
// @@protoc_insertion_point(message_clear_start:tdrp.packet.SSendEvent)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&sender_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&radius_) -
      reinterpret_cast<char*>(&sender_)) + sizeof(radius_));
  _internal_metadata_.Clear();
}

bool SSendEvent::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tdrp.packet.SSendEvent)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // fixed32 sender = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(13u /* 13 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED32>(
                 input, &sender_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string name = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->name().data(), static_cast<int>(this->name().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SSendEvent.name"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string data = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(26u /* 26 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_data()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->data().data(), static_cast<int>(this->data().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SSendEvent.data"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // float x = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(37u /* 37 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &x_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // float y = 5;
      case 5: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(45u /* 45 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &y_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // float radius = 6;
      case 6: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(53u /* 53 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &radius_)));
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
  // @@protoc_insertion_point(parse_success:tdrp.packet.SSendEvent)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tdrp.packet.SSendEvent)
  return false;
#undef DO_
}

void SSendEvent::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tdrp.packet.SSendEvent)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // fixed32 sender = 1;
  if (this->sender() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed32(1, this->sender(), output);
  }

  // string name = 2;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SSendEvent.name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->name(), output);
  }

  // string data = 3;
  if (this->data().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->data().data(), static_cast<int>(this->data().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SSendEvent.data");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->data(), output);
  }

  // float x = 4;
  if (this->x() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(4, this->x(), output);
  }

  // float y = 5;
  if (this->y() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(5, this->y(), output);
  }

  // float radius = 6;
  if (this->radius() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(6, this->radius(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tdrp.packet.SSendEvent)
}

::google::protobuf::uint8* SSendEvent::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tdrp.packet.SSendEvent)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // fixed32 sender = 1;
  if (this->sender() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed32ToArray(1, this->sender(), target);
  }

  // string name = 2;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SSendEvent.name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->name(), target);
  }

  // string data = 3;
  if (this->data().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->data().data(), static_cast<int>(this->data().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SSendEvent.data");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->data(), target);
  }

  // float x = 4;
  if (this->x() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(4, this->x(), target);
  }

  // float y = 5;
  if (this->y() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(5, this->y(), target);
  }

  // float radius = 6;
  if (this->radius() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(6, this->radius(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tdrp.packet.SSendEvent)
  return target;
}

size_t SSendEvent::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tdrp.packet.SSendEvent)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string name = 2;
  if (this->name().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->name());
  }

  // string data = 3;
  if (this->data().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->data());
  }

  // fixed32 sender = 1;
  if (this->sender() != 0) {
    total_size += 1 + 4;
  }

  // float x = 4;
  if (this->x() != 0) {
    total_size += 1 + 4;
  }

  // float y = 5;
  if (this->y() != 0) {
    total_size += 1 + 4;
  }

  // float radius = 6;
  if (this->radius() != 0) {
    total_size += 1 + 4;
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SSendEvent::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tdrp.packet.SSendEvent)
  GOOGLE_DCHECK_NE(&from, this);
  const SSendEvent* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const SSendEvent>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tdrp.packet.SSendEvent)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tdrp.packet.SSendEvent)
    MergeFrom(*source);
  }
}

void SSendEvent::MergeFrom(const SSendEvent& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tdrp.packet.SSendEvent)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.name().size() > 0) {

    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  if (from.data().size() > 0) {

    data_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.data_);
  }
  if (from.sender() != 0) {
    set_sender(from.sender());
  }
  if (from.x() != 0) {
    set_x(from.x());
  }
  if (from.y() != 0) {
    set_y(from.y());
  }
  if (from.radius() != 0) {
    set_radius(from.radius());
  }
}

void SSendEvent::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tdrp.packet.SSendEvent)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SSendEvent::CopyFrom(const SSendEvent& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tdrp.packet.SSendEvent)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SSendEvent::IsInitialized() const {
  return true;
}

void SSendEvent::Swap(SSendEvent* other) {
  if (other == this) return;
  InternalSwap(other);
}
void SSendEvent::InternalSwap(SSendEvent* other) {
  using std::swap;
  name_.Swap(&other->name_);
  data_.Swap(&other->data_);
  swap(sender_, other->sender_);
  swap(x_, other->x_);
  swap(y_, other->y_);
  swap(radius_, other->radius_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata SSendEvent::GetMetadata() const {
  protobuf_SSendEvent_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SSendEvent_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)
