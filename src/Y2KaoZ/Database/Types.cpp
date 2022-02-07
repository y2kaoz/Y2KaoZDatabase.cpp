#include "Y2KaoZ/Database/Types.hpp"

#include <cstring>
#include <gsl/gsl_util>
#include <stdexcept>

namespace Y2KaoZ::Database {

ResultType::ResultType(Value r) : value_(std::move(r)) {
}
auto ResultType::isNull() const noexcept -> bool {
  return std::holds_alternative<NullType>(value_);
}
auto ResultType::isInteger() const noexcept -> bool {
  return std::holds_alternative<std::int64_t>(value_);
}
auto ResultType::isReal() const noexcept -> bool {
  return std::holds_alternative<double>(value_);
}
auto ResultType::isString() const noexcept -> bool {
  return std::holds_alternative<std::string>(value_);
}
auto ResultType::isBlob() const noexcept -> bool {
  return std::holds_alternative<BlobType>(value_);
}
auto ResultType::getType() const noexcept -> ResultType::Type {
  switch (value_.index()) {
    case 0:
      static_assert(std::is_same_v<NullType, std::variant_alternative_t<0, Value>>);
      return Type::Null;
    case 1:
      static_assert(std::is_same_v<std::int64_t, std::variant_alternative_t<1, Value>>);
      return Type::Integer;
    case 2:
      static_assert(std::is_same_v<double, std::variant_alternative_t<2, Value>>);
      return Type::Real;
    case 3:
      static_assert(std::is_same_v<std::string, std::variant_alternative_t<3, Value>>);
      return Type::String;
    case 4:
      static_assert(std::is_same_v<BlobType, std::variant_alternative_t<4, Value>>);
      return Type::Blob;
    default:
      return Type::Unknown;
  }
}
auto ResultType::getInteger() const -> const std::int64_t& {
  return std::get<std::int64_t>(value_);
}
auto ResultType::getInteger() -> std::int64_t& {
  return std::get<std::int64_t>(value_);
}
auto ResultType::asInteger8() const -> std::int8_t {
  return gsl::narrow<std::int8_t>(asInteger64());
}
auto ResultType::asUnsigned8() const -> std::uint8_t {
  return gsl::narrow<std::uint8_t>(asInteger64());
}
auto ResultType::asInteger16() const -> std::int16_t {
  return gsl::narrow<std::int16_t>(asInteger64());
}
auto ResultType::asUnsigned16() const -> std::uint16_t {
  return gsl::narrow<std::uint16_t>(asInteger64());
}
auto ResultType::asInteger32() const -> std::int32_t {
  return gsl::narrow<std::int32_t>(asInteger64());
}
auto ResultType::asUnsigned32() const -> std::uint32_t {
  return gsl::narrow<std::uint32_t>(asInteger64());
}
auto ResultType::asInteger64() const -> std::int64_t {
  switch (getType()) {
    case Type::Null:
      return 0;
    case Type::String:
      return std::stol(getString(), nullptr, 0);
    case Type::Real:
      return static_cast<std::int64_t>(getReal());
    case Type::Blob: {
      if (getBlob().size() == sizeof(std::int8_t)) {
        std::int8_t result = 0;
        std::memcpy(&result, getBlob().data(), sizeof(result));
        return result;
      }
      if (getBlob().size() == sizeof(std::int16_t)) {
        std::int16_t result = 0;
        std::memcpy(&result, getBlob().data(), sizeof(result));
        return result;
      }
      if (getBlob().size() == sizeof(std::int32_t)) {
        std::int32_t result = 0;
        std::memcpy(&result, getBlob().data(), sizeof(result));
        return result;
      }
      if (getBlob().size() == sizeof(std::int64_t)) {
        std::int64_t result = 0;
        std::memcpy(&result, getBlob().data(), sizeof(result));
        return result;
      }
      throw std::out_of_range("The blob is too big");
    }
    default:
      return std::get<std::int64_t>(value_);
  }
}
auto ResultType::asUnsigned64() const -> std::uint64_t {
  return gsl::narrow<std::uint64_t>(asInteger64());
}
auto ResultType::getReal() const -> const double& {
  return std::get<double>(value_);
}
auto ResultType::getReal() -> double& {
  return std::get<double>(value_);
}
auto ResultType::asReal() const -> double {
  switch (getType()) {
    case Type::Null:
      return 0.0;
    case Type::String:
      return std::stod(getString(), nullptr);
    case Type::Integer:
      return static_cast<double>(getInteger());
    case Type::Blob: {
      if (getBlob().size() == sizeof(float)) {
        float result = 0;
        std::memcpy(&result, getBlob().data(), sizeof(result));
        return result;
      }
      if (getBlob().size() == sizeof(double)) {
        double result = 0;
        std::memcpy(&result, getBlob().data(), sizeof(result));
        return result;
      }
      throw std::out_of_range("The blob is too big");
    }
    default:
      return std::get<double>(value_);
  }
}

auto ResultType::getString() const -> const std::string& {
  return std::get<std::string>(value_);
}
auto ResultType::getString() -> std::string& {
  return std::get<std::string>(value_);
}
auto ResultType::asString() const -> std::string {
  switch (getType()) {
    case Type::Null:
      return std::string{};
    case Type::Integer:
      return std::to_string(getInteger());
    case Type::Real:
      return std::to_string(getReal());
    case Type::Blob: {
      std::string result(getBlob().size(), 0);
      std::memcpy(result.data(), getBlob().data(), result.size());
      return result;
    }
    default:
      return std::get<std::string>(value_);
  }
}
auto ResultType::getBlob() const -> const BlobType& {
  return std::get<BlobType>(value_);
}
auto ResultType::getBlob() -> BlobType& {
  return std::get<BlobType>(value_);
}
auto ResultType::asBlob() const -> BlobType {
  switch (getType()) {
    case Type::Null:
      return BlobType{};
    case Type::Integer: {
      auto value = getInteger();
      BlobType result(sizeof(value));
      std::memcpy(result.data(), &value, result.size());
      return result;
    }
    case Type::Real: {
      auto value = getReal();
      BlobType result(sizeof(value));
      std::memcpy(result.data(), &value, result.size());
      return result;
    }
    case Type::String: {
      auto value = getString();
      BlobType result(value.size());
      std::memcpy(result.data(), value.data(), result.size());
      return result;
    }
    default:
      return std::get<BlobType>(value_);
  }
}

} // namespace Y2KaoZ::Database