#pragma once

#include "Y2KaoZ/Database/Visibility.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Y2KaoZ::Database {

using NullType = std::monostate;
static const auto NullValue = NullType{};
using BlobType = std::vector<std::byte>;

using ParamType = std::variant<
  NullType,
  std::int8_t,
  std::uint8_t,
  std::int16_t,
  std::uint16_t,
  std::int32_t,
  std::uint32_t,
  std::int64_t,
  std::uint64_t,
  float,
  double,
  std::string,
  BlobType>;
using ParamVector = std::vector<ParamType>;
using ParamMap = std::unordered_map<std::string_view, ParamType>;

class Y2KAOZDATABASE_EXPORT ResultType {
public:
  using Value = std::variant<NullType, std::int64_t, double, std::string, BlobType>;
  enum class Type : std::uint8_t
  {
    Null = 0,
    Integer,
    Real,
    String,
    Blob,
    Unknown = static_cast<std::uint8_t>(-1)
  };
  ResultType() = default;
  explicit ResultType(Value r);
  [[nodiscard]] auto isNull() const noexcept -> bool;
  [[nodiscard]] auto isInteger() const noexcept -> bool;
  [[nodiscard]] auto isReal() const noexcept -> bool;
  [[nodiscard]] auto isString() const noexcept -> bool;
  [[nodiscard]] auto isBlob() const noexcept -> bool;
  [[nodiscard]] auto getType() const noexcept -> Type;
  [[nodiscard]] auto getInteger() const -> const std::int64_t&;
  [[nodiscard]] auto getInteger() -> std::int64_t&;
  [[nodiscard]] auto asInteger8() const -> std::int8_t;
  [[nodiscard]] auto asUnsigned8() const -> std::uint8_t;
  [[nodiscard]] auto asInteger16() const -> std::int16_t;
  [[nodiscard]] auto asUnsigned16() const -> std::uint16_t;
  [[nodiscard]] auto asInteger32() const -> std::int32_t;
  [[nodiscard]] auto asUnsigned32() const -> std::uint32_t;
  [[nodiscard]] auto asInteger64() const -> std::int64_t;
  [[nodiscard]] auto asUnsigned64() const -> std::uint64_t;
  [[nodiscard]] auto getReal() const -> const double&;
  [[nodiscard]] auto getReal() -> double&;
  [[nodiscard]] auto asReal() const -> double;
  [[nodiscard]] auto getString() const -> const std::string&;
  [[nodiscard]] auto getString() -> std::string&;
  [[nodiscard]] auto asString() const -> std::string;
  [[nodiscard]] auto getBlob() const -> const BlobType&;
  [[nodiscard]] auto getBlob() -> BlobType&;
  [[nodiscard]] auto asBlob() const -> BlobType;

private:
  Value value_;
};
using ResultVector = std::vector<ResultType>;
using ResultMap = std::unordered_map<std::string, ResultType>;

} // namespace Y2KaoZ::Database