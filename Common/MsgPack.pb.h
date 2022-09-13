// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: MsgPack.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_MsgPack_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_MsgPack_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021005 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_MsgPack_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_MsgPack_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_MsgPack_2eproto;
class EchoPack;
struct EchoPackDefaultTypeInternal;
extern EchoPackDefaultTypeInternal _EchoPack_default_instance_;
class MessagePack;
struct MessagePackDefaultTypeInternal;
extern MessagePackDefaultTypeInternal _MessagePack_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::EchoPack* Arena::CreateMaybeMessage<::EchoPack>(Arena*);
template<> ::MessagePack* Arena::CreateMaybeMessage<::MessagePack>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class MessagePack final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:MessagePack) */ {
 public:
  inline MessagePack() : MessagePack(nullptr) {}
  ~MessagePack() override;
  explicit PROTOBUF_CONSTEXPR MessagePack(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  MessagePack(const MessagePack& from);
  MessagePack(MessagePack&& from) noexcept
    : MessagePack() {
    *this = ::std::move(from);
  }

  inline MessagePack& operator=(const MessagePack& from) {
    CopyFrom(from);
    return *this;
  }
  inline MessagePack& operator=(MessagePack&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const MessagePack& default_instance() {
    return *internal_default_instance();
  }
  static inline const MessagePack* internal_default_instance() {
    return reinterpret_cast<const MessagePack*>(
               &_MessagePack_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(MessagePack& a, MessagePack& b) {
    a.Swap(&b);
  }
  inline void Swap(MessagePack* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(MessagePack* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  MessagePack* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<MessagePack>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const MessagePack& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const MessagePack& from) {
    MessagePack::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(MessagePack* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "MessagePack";
  }
  protected:
  explicit MessagePack(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kDataFieldNumber = 1,
    kMethodFieldNumber = 2,
    kServiceFieldNumber = 3,
    kSequenceFieldNumber = 4,
  };
  // bytes data = 1;
  void clear_data();
  const std::string& data() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_data(ArgT0&& arg0, ArgT... args);
  std::string* mutable_data();
  PROTOBUF_NODISCARD std::string* release_data();
  void set_allocated_data(std::string* data);
  private:
  const std::string& _internal_data() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_data(const std::string& value);
  std::string* _internal_mutable_data();
  public:

  // string Method = 2;
  void clear_method();
  const std::string& method() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_method(ArgT0&& arg0, ArgT... args);
  std::string* mutable_method();
  PROTOBUF_NODISCARD std::string* release_method();
  void set_allocated_method(std::string* method);
  private:
  const std::string& _internal_method() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_method(const std::string& value);
  std::string* _internal_mutable_method();
  public:

  // string Service = 3;
  void clear_service();
  const std::string& service() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_service(ArgT0&& arg0, ArgT... args);
  std::string* mutable_service();
  PROTOBUF_NODISCARD std::string* release_service();
  void set_allocated_service(std::string* service);
  private:
  const std::string& _internal_service() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_service(const std::string& value);
  std::string* _internal_mutable_service();
  public:

  // uint32 sequence = 4;
  void clear_sequence();
  uint32_t sequence() const;
  void set_sequence(uint32_t value);
  private:
  uint32_t _internal_sequence() const;
  void _internal_set_sequence(uint32_t value);
  public:

  // @@protoc_insertion_point(class_scope:MessagePack)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr data_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr method_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr service_;
    uint32_t sequence_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_MsgPack_2eproto;
};
// -------------------------------------------------------------------

class EchoPack final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:EchoPack) */ {
 public:
  inline EchoPack() : EchoPack(nullptr) {}
  ~EchoPack() override;
  explicit PROTOBUF_CONSTEXPR EchoPack(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  EchoPack(const EchoPack& from);
  EchoPack(EchoPack&& from) noexcept
    : EchoPack() {
    *this = ::std::move(from);
  }

  inline EchoPack& operator=(const EchoPack& from) {
    CopyFrom(from);
    return *this;
  }
  inline EchoPack& operator=(EchoPack&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const EchoPack& default_instance() {
    return *internal_default_instance();
  }
  static inline const EchoPack* internal_default_instance() {
    return reinterpret_cast<const EchoPack*>(
               &_EchoPack_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(EchoPack& a, EchoPack& b) {
    a.Swap(&b);
  }
  inline void Swap(EchoPack* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(EchoPack* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  EchoPack* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<EchoPack>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const EchoPack& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const EchoPack& from) {
    EchoPack::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(EchoPack* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "EchoPack";
  }
  protected:
  explicit EchoPack(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kDataFieldNumber = 1,
    kSequenceFieldNumber = 4,
  };
  // bytes data = 1;
  void clear_data();
  const std::string& data() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_data(ArgT0&& arg0, ArgT... args);
  std::string* mutable_data();
  PROTOBUF_NODISCARD std::string* release_data();
  void set_allocated_data(std::string* data);
  private:
  const std::string& _internal_data() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_data(const std::string& value);
  std::string* _internal_mutable_data();
  public:

  // uint32 sequence = 4;
  void clear_sequence();
  uint32_t sequence() const;
  void set_sequence(uint32_t value);
  private:
  uint32_t _internal_sequence() const;
  void _internal_set_sequence(uint32_t value);
  public:

  // @@protoc_insertion_point(class_scope:EchoPack)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr data_;
    uint32_t sequence_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_MsgPack_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// MessagePack

// bytes data = 1;
inline void MessagePack::clear_data() {
  _impl_.data_.ClearToEmpty();
}
inline const std::string& MessagePack::data() const {
  // @@protoc_insertion_point(field_get:MessagePack.data)
  return _internal_data();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void MessagePack::set_data(ArgT0&& arg0, ArgT... args) {
 
 _impl_.data_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:MessagePack.data)
}
inline std::string* MessagePack::mutable_data() {
  std::string* _s = _internal_mutable_data();
  // @@protoc_insertion_point(field_mutable:MessagePack.data)
  return _s;
}
inline const std::string& MessagePack::_internal_data() const {
  return _impl_.data_.Get();
}
inline void MessagePack::_internal_set_data(const std::string& value) {
  
  _impl_.data_.Set(value, GetArenaForAllocation());
}
inline std::string* MessagePack::_internal_mutable_data() {
  
  return _impl_.data_.Mutable(GetArenaForAllocation());
}
inline std::string* MessagePack::release_data() {
  // @@protoc_insertion_point(field_release:MessagePack.data)
  return _impl_.data_.Release();
}
inline void MessagePack::set_allocated_data(std::string* data) {
  if (data != nullptr) {
    
  } else {
    
  }
  _impl_.data_.SetAllocated(data, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.data_.IsDefault()) {
    _impl_.data_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:MessagePack.data)
}

// string Method = 2;
inline void MessagePack::clear_method() {
  _impl_.method_.ClearToEmpty();
}
inline const std::string& MessagePack::method() const {
  // @@protoc_insertion_point(field_get:MessagePack.Method)
  return _internal_method();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void MessagePack::set_method(ArgT0&& arg0, ArgT... args) {
 
 _impl_.method_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:MessagePack.Method)
}
inline std::string* MessagePack::mutable_method() {
  std::string* _s = _internal_mutable_method();
  // @@protoc_insertion_point(field_mutable:MessagePack.Method)
  return _s;
}
inline const std::string& MessagePack::_internal_method() const {
  return _impl_.method_.Get();
}
inline void MessagePack::_internal_set_method(const std::string& value) {
  
  _impl_.method_.Set(value, GetArenaForAllocation());
}
inline std::string* MessagePack::_internal_mutable_method() {
  
  return _impl_.method_.Mutable(GetArenaForAllocation());
}
inline std::string* MessagePack::release_method() {
  // @@protoc_insertion_point(field_release:MessagePack.Method)
  return _impl_.method_.Release();
}
inline void MessagePack::set_allocated_method(std::string* method) {
  if (method != nullptr) {
    
  } else {
    
  }
  _impl_.method_.SetAllocated(method, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.method_.IsDefault()) {
    _impl_.method_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:MessagePack.Method)
}

// string Service = 3;
inline void MessagePack::clear_service() {
  _impl_.service_.ClearToEmpty();
}
inline const std::string& MessagePack::service() const {
  // @@protoc_insertion_point(field_get:MessagePack.Service)
  return _internal_service();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void MessagePack::set_service(ArgT0&& arg0, ArgT... args) {
 
 _impl_.service_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:MessagePack.Service)
}
inline std::string* MessagePack::mutable_service() {
  std::string* _s = _internal_mutable_service();
  // @@protoc_insertion_point(field_mutable:MessagePack.Service)
  return _s;
}
inline const std::string& MessagePack::_internal_service() const {
  return _impl_.service_.Get();
}
inline void MessagePack::_internal_set_service(const std::string& value) {
  
  _impl_.service_.Set(value, GetArenaForAllocation());
}
inline std::string* MessagePack::_internal_mutable_service() {
  
  return _impl_.service_.Mutable(GetArenaForAllocation());
}
inline std::string* MessagePack::release_service() {
  // @@protoc_insertion_point(field_release:MessagePack.Service)
  return _impl_.service_.Release();
}
inline void MessagePack::set_allocated_service(std::string* service) {
  if (service != nullptr) {
    
  } else {
    
  }
  _impl_.service_.SetAllocated(service, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.service_.IsDefault()) {
    _impl_.service_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:MessagePack.Service)
}

// uint32 sequence = 4;
inline void MessagePack::clear_sequence() {
  _impl_.sequence_ = 0u;
}
inline uint32_t MessagePack::_internal_sequence() const {
  return _impl_.sequence_;
}
inline uint32_t MessagePack::sequence() const {
  // @@protoc_insertion_point(field_get:MessagePack.sequence)
  return _internal_sequence();
}
inline void MessagePack::_internal_set_sequence(uint32_t value) {
  
  _impl_.sequence_ = value;
}
inline void MessagePack::set_sequence(uint32_t value) {
  _internal_set_sequence(value);
  // @@protoc_insertion_point(field_set:MessagePack.sequence)
}

// -------------------------------------------------------------------

// EchoPack

// bytes data = 1;
inline void EchoPack::clear_data() {
  _impl_.data_.ClearToEmpty();
}
inline const std::string& EchoPack::data() const {
  // @@protoc_insertion_point(field_get:EchoPack.data)
  return _internal_data();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void EchoPack::set_data(ArgT0&& arg0, ArgT... args) {
 
 _impl_.data_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:EchoPack.data)
}
inline std::string* EchoPack::mutable_data() {
  std::string* _s = _internal_mutable_data();
  // @@protoc_insertion_point(field_mutable:EchoPack.data)
  return _s;
}
inline const std::string& EchoPack::_internal_data() const {
  return _impl_.data_.Get();
}
inline void EchoPack::_internal_set_data(const std::string& value) {
  
  _impl_.data_.Set(value, GetArenaForAllocation());
}
inline std::string* EchoPack::_internal_mutable_data() {
  
  return _impl_.data_.Mutable(GetArenaForAllocation());
}
inline std::string* EchoPack::release_data() {
  // @@protoc_insertion_point(field_release:EchoPack.data)
  return _impl_.data_.Release();
}
inline void EchoPack::set_allocated_data(std::string* data) {
  if (data != nullptr) {
    
  } else {
    
  }
  _impl_.data_.SetAllocated(data, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.data_.IsDefault()) {
    _impl_.data_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:EchoPack.data)
}

// uint32 sequence = 4;
inline void EchoPack::clear_sequence() {
  _impl_.sequence_ = 0u;
}
inline uint32_t EchoPack::_internal_sequence() const {
  return _impl_.sequence_;
}
inline uint32_t EchoPack::sequence() const {
  // @@protoc_insertion_point(field_get:EchoPack.sequence)
  return _internal_sequence();
}
inline void EchoPack::_internal_set_sequence(uint32_t value) {
  
  _impl_.sequence_ = value;
}
inline void EchoPack::set_sequence(uint32_t value) {
  _internal_set_sequence(value);
  // @@protoc_insertion_point(field_set:EchoPack.sequence)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_MsgPack_2eproto
