// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SServerInfo.proto

#include "SServerInfo.pb.h"

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
class SServerInfoDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<SServerInfo>
      _instance;
} _SServerInfo_default_instance_;
}  // namespace packet
}  // namespace tdrp
namespace protobuf_SServerInfo_2eproto {
void InitDefaultsSServerInfoImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tdrp::packet::_SServerInfo_default_instance_;
    new (ptr) ::tdrp::packet::SServerInfo();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tdrp::packet::SServerInfo::InitAsDefaultInstance();
}

void InitDefaultsSServerInfo() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsSServerInfoImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, uniqueid_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, name_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, package_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, version_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, host_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, port_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tdrp::packet::SServerInfo, maxplayers_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tdrp::packet::SServerInfo)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tdrp::packet::_SServerInfo_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "SServerInfo.proto", schemas, file_default_instances, TableStruct::offsets, factory,
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
      "\n\021SServerInfo.proto\022\013tdrp.packet\"\177\n\013SSer"
      "verInfo\022\020\n\010uniqueid\030\001 \001(\t\022\014\n\004name\030\002 \001(\t\022"
      "\017\n\007package\030\003 \001(\t\022\017\n\007version\030\004 \001(\t\022\014\n\004hos"
      "t\030\005 \001(\t\022\014\n\004port\030\006 \001(\r\022\022\n\nmaxplayers\030\007 \001("
      "\rb\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 169);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "SServerInfo.proto", &protobuf_RegisterTypes);
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
}  // namespace protobuf_SServerInfo_2eproto
namespace tdrp {
namespace packet {

// ===================================================================

void SServerInfo::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int SServerInfo::kUniqueidFieldNumber;
const int SServerInfo::kNameFieldNumber;
const int SServerInfo::kPackageFieldNumber;
const int SServerInfo::kVersionFieldNumber;
const int SServerInfo::kHostFieldNumber;
const int SServerInfo::kPortFieldNumber;
const int SServerInfo::kMaxplayersFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

SServerInfo::SServerInfo()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_SServerInfo_2eproto::InitDefaultsSServerInfo();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tdrp.packet.SServerInfo)
}
SServerInfo::SServerInfo(const SServerInfo& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  uniqueid_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.uniqueid().size() > 0) {
    uniqueid_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.uniqueid_);
  }
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.name().size() > 0) {
    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  package_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.package().size() > 0) {
    package_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.package_);
  }
  version_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.version().size() > 0) {
    version_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.version_);
  }
  host_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.host().size() > 0) {
    host_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.host_);
  }
  ::memcpy(&port_, &from.port_,
    static_cast<size_t>(reinterpret_cast<char*>(&maxplayers_) -
    reinterpret_cast<char*>(&port_)) + sizeof(maxplayers_));
  // @@protoc_insertion_point(copy_constructor:tdrp.packet.SServerInfo)
}

void SServerInfo::SharedCtor() {
  uniqueid_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  package_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  version_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  host_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&port_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&maxplayers_) -
      reinterpret_cast<char*>(&port_)) + sizeof(maxplayers_));
  _cached_size_ = 0;
}

SServerInfo::~SServerInfo() {
  // @@protoc_insertion_point(destructor:tdrp.packet.SServerInfo)
  SharedDtor();
}

void SServerInfo::SharedDtor() {
  uniqueid_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  package_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  version_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  host_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void SServerInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SServerInfo::descriptor() {
  ::protobuf_SServerInfo_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SServerInfo_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const SServerInfo& SServerInfo::default_instance() {
  ::protobuf_SServerInfo_2eproto::InitDefaultsSServerInfo();
  return *internal_default_instance();
}

SServerInfo* SServerInfo::New(::google::protobuf::Arena* arena) const {
  SServerInfo* n = new SServerInfo;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void SServerInfo::Clear() {
// @@protoc_insertion_point(message_clear_start:tdrp.packet.SServerInfo)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  uniqueid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  package_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  version_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  host_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&port_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&maxplayers_) -
      reinterpret_cast<char*>(&port_)) + sizeof(maxplayers_));
  _internal_metadata_.Clear();
}

bool SServerInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tdrp.packet.SServerInfo)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string uniqueid = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_uniqueid()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->uniqueid().data(), static_cast<int>(this->uniqueid().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SServerInfo.uniqueid"));
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
            "tdrp.packet.SServerInfo.name"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string package = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(26u /* 26 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_package()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->package().data(), static_cast<int>(this->package().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SServerInfo.package"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string version = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(34u /* 34 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_version()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->version().data(), static_cast<int>(this->version().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SServerInfo.version"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string host = 5;
      case 5: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(42u /* 42 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_host()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->host().data(), static_cast<int>(this->host().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tdrp.packet.SServerInfo.host"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // uint32 port = 6;
      case 6: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(48u /* 48 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &port_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // uint32 maxplayers = 7;
      case 7: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(56u /* 56 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &maxplayers_)));
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
  // @@protoc_insertion_point(parse_success:tdrp.packet.SServerInfo)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tdrp.packet.SServerInfo)
  return false;
#undef DO_
}

void SServerInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tdrp.packet.SServerInfo)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string uniqueid = 1;
  if (this->uniqueid().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->uniqueid().data(), static_cast<int>(this->uniqueid().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.uniqueid");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->uniqueid(), output);
  }

  // string name = 2;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->name(), output);
  }

  // string package = 3;
  if (this->package().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->package().data(), static_cast<int>(this->package().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.package");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->package(), output);
  }

  // string version = 4;
  if (this->version().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->version().data(), static_cast<int>(this->version().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.version");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      4, this->version(), output);
  }

  // string host = 5;
  if (this->host().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->host().data(), static_cast<int>(this->host().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.host");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      5, this->host(), output);
  }

  // uint32 port = 6;
  if (this->port() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(6, this->port(), output);
  }

  // uint32 maxplayers = 7;
  if (this->maxplayers() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(7, this->maxplayers(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tdrp.packet.SServerInfo)
}

::google::protobuf::uint8* SServerInfo::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tdrp.packet.SServerInfo)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string uniqueid = 1;
  if (this->uniqueid().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->uniqueid().data(), static_cast<int>(this->uniqueid().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.uniqueid");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->uniqueid(), target);
  }

  // string name = 2;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->name(), target);
  }

  // string package = 3;
  if (this->package().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->package().data(), static_cast<int>(this->package().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.package");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->package(), target);
  }

  // string version = 4;
  if (this->version().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->version().data(), static_cast<int>(this->version().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.version");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->version(), target);
  }

  // string host = 5;
  if (this->host().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->host().data(), static_cast<int>(this->host().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tdrp.packet.SServerInfo.host");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->host(), target);
  }

  // uint32 port = 6;
  if (this->port() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(6, this->port(), target);
  }

  // uint32 maxplayers = 7;
  if (this->maxplayers() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(7, this->maxplayers(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tdrp.packet.SServerInfo)
  return target;
}

size_t SServerInfo::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tdrp.packet.SServerInfo)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string uniqueid = 1;
  if (this->uniqueid().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->uniqueid());
  }

  // string name = 2;
  if (this->name().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->name());
  }

  // string package = 3;
  if (this->package().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->package());
  }

  // string version = 4;
  if (this->version().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->version());
  }

  // string host = 5;
  if (this->host().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->host());
  }

  // uint32 port = 6;
  if (this->port() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->port());
  }

  // uint32 maxplayers = 7;
  if (this->maxplayers() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->maxplayers());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SServerInfo::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tdrp.packet.SServerInfo)
  GOOGLE_DCHECK_NE(&from, this);
  const SServerInfo* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const SServerInfo>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tdrp.packet.SServerInfo)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tdrp.packet.SServerInfo)
    MergeFrom(*source);
  }
}

void SServerInfo::MergeFrom(const SServerInfo& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tdrp.packet.SServerInfo)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.uniqueid().size() > 0) {

    uniqueid_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.uniqueid_);
  }
  if (from.name().size() > 0) {

    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  if (from.package().size() > 0) {

    package_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.package_);
  }
  if (from.version().size() > 0) {

    version_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.version_);
  }
  if (from.host().size() > 0) {

    host_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.host_);
  }
  if (from.port() != 0) {
    set_port(from.port());
  }
  if (from.maxplayers() != 0) {
    set_maxplayers(from.maxplayers());
  }
}

void SServerInfo::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tdrp.packet.SServerInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SServerInfo::CopyFrom(const SServerInfo& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tdrp.packet.SServerInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SServerInfo::IsInitialized() const {
  return true;
}

void SServerInfo::Swap(SServerInfo* other) {
  if (other == this) return;
  InternalSwap(other);
}
void SServerInfo::InternalSwap(SServerInfo* other) {
  using std::swap;
  uniqueid_.Swap(&other->uniqueid_);
  name_.Swap(&other->name_);
  package_.Swap(&other->package_);
  version_.Swap(&other->version_);
  host_.Swap(&other->host_);
  swap(port_, other->port_);
  swap(maxplayers_, other->maxplayers_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata SServerInfo::GetMetadata() const {
  protobuf_SServerInfo_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_SServerInfo_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace packet
}  // namespace tdrp

// @@protoc_insertion_point(global_scope)
