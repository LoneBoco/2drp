// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SClientScript.proto

#include "SClientScript.pb.h"

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
class SClientScriptDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<SClientScript>
      _instance;
} _SClientScript_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace protobuf_SClientScript_2eproto {
void InitDefaultsSClientScriptImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tdrp::packet::_SClientScript_default_instance_;
    new (ptr) ::tdrp::packet::SClientScript();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tdrp::packet::SClientScript::InitAsDefaultInstance();
}

void InitDefaultsSClientScript() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsSClientScriptImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SClientScript, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SClientScript, name_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SClientScript, script_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tdrp::packet::SClientScript)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tdrp::packet::_SClientScript_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "SClientScript.proto", schemas, file_default_instances, TableStruct::offsets, factory,
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
      "\n\023SClientScript.proto\022\013tdrp.packet\"-\n\rSC"
      "lientScript\022\014\n\004name\030\001 \001(\t\022\016\n\006script\030\002 \001("
      "\tb\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 89);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "SClientScript.proto", &protobuf_RegisterTypes);
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
}  // namespace protobuf_SClientScript_2eproto
namespace tdrp {
namespace packet {

// ===================================================================

void SClientScript::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int SClientScript::kNameFieldNumber;
const int SClientScript::kScriptFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

SClientScript::SClientScript()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_SClientScript_2eproto::InitDefaultsSClientScript();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tdrp.packet.SClientScript)
}
SClientScript::SClientScript(const SClientScript& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.name().size() > 0) {
    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  script_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.script().size() > 0) {
    script_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.script_);
  }
  // @@protoc_insertion_point(copy_constructor:tdrp.packet.SClientScript)
}

void SClientScript::SharedCtor() {
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  script_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _cached_size_ = 0;
}

SClientScript::~SClientScript() {
  // @@protoc_insertion_point(destructor:tdrp.packet.SClientScript)
  SharedDtor();
}

void SClientScript::SharedDtor() {
  name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  script_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void SClientScript::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SClientScript::descriptor() {
  ::protobuf_SClientScript_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SClientScript_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const SClientScript& SClientScript::default_instance() {
  ::protobuf_SClientScript_2eproto::InitDefaultsSClientScript();
  return *internal_default_instance();
}

SClientScript* SClientScript::New(::google::protobuf::Arena* arena) const {
  SClientScript* n = new SClientScript;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void SClientScript::Clear() {
// @@protoc_insertion_point(message_clear_start:tdrp.packet.SClientScript)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  script_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

bool SClientScript::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tdrp.packet.SClientScript)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string name = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->name().data(), static_cast<int>(this->name().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SClientScript.name"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string script = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_script()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->script().data(), static_cast<int>(this->script().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SClientScript.script"));
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
  // @@protoc_insertion_point(parse_success:tdrp.packet.SClientScript)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tdrp.packet.SClientScript)
  return false;
#undef DO_
}

void SClientScript::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tdrp.packet.SClientScript)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string name = 1;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SClientScript.name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->name(), output);
  }

  // string script = 2;
  if (this->script().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->script().data(), static_cast<int>(this->script().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SClientScript.script");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->script(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tdrp.packet.SClientScript)
}

::google::protobuf::uint8* SClientScript::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tdrp.packet.SClientScript)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string name = 1;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SClientScript.name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->name(), target);
  }

  // string script = 2;
  if (this->script().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->script().data(), static_cast<int>(this->script().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SClientScript.script");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->script(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tdrp.packet.SClientScript)
  return target;
}

size_t SClientScript::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tdrp.packet.SClientScript)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string name = 1;
  if (this->name().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->name());
  }

  // string script = 2;
  if (this->script().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->script());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SClientScript::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tdrp.packet.SClientScript)
  GOOGLE_DCHECK_NE(&from, this);
  const SClientScript* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const SClientScript>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tdrp.packet.SClientScript)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tdrp.packet.SClientScript)
    MergeFrom(*source);
  }
}

void SClientScript::MergeFrom(const SClientScript& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tdrp.packet.SClientScript)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.name().size() > 0) {

    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  if (from.script().size() > 0) {

    script_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.script_);
  }
}

void SClientScript::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tdrp.packet.SClientScript)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SClientScript::CopyFrom(const SClientScript& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tdrp.packet.SClientScript)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SClientScript::IsInitialized() const {
  return true;
}

void SClientScript::Swap(SClientScript* other) {
  if (other == this) return;
  InternalSwap(other);
}
void SClientScript::InternalSwap(SClientScript* other) {
  using std::swap;
  name_.Swap(&other->name_);
  script_.Swap(&other->script_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata SClientScript::GetMetadata() const {
  protobuf_SClientScript_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SClientScript_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)