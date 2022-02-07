#include "Y2KaoZ/Database/Sql/Sqlite3/Statement.hpp"
#include "Y2KaoZ/Database/Sql/Sqlite3/Exception.hpp"
#include "Y2KaoZ/Database/Types.hpp"
#include <cstring>
#include <gsl/gsl_util>

namespace {

using Y2KaoZ::Database::Sql::Sqlite3::Exception;
using NullType = Y2KaoZ::Database::NullType;
using BlobType = Y2KaoZ::Database::BlobType;
using ResultType = Y2KaoZ::Database::ResultType;
using ResultTypeValue = ResultType::Value;

[[nodiscard]] auto prepareSqlite3(gsl::not_null<sqlite3*> db, const std::string_view& statement)
  -> gsl::not_null<sqlite3_stmt*> {

  sqlite3_stmt* stmt = nullptr;
  if (sqlite3_prepare_v2(db, statement.data(), -1, &stmt, nullptr) != SQLITE_OK) {
    gsl::finally([&]() { sqlite3_finalize(stmt); });
    throw Exception("Error in '" + std::string(statement) + "': " + sqlite3_errmsg(db));
  }
  return stmt;
}

struct StmtValueBinder {
  StmtValueBinder(gsl::not_null<sqlite3_stmt*> stmt, int i) : stmt(stmt.get()), i(i) {
  }
  void operator()(const NullType& /*unused*/) const {
    resultCode(sqlite3_bind_null(stmt, i));
  }
  void operator()(const std::int8_t& v) const {
    resultCode(sqlite3_bind_int(stmt, i, v));
  }
  void operator()(const std::uint8_t& v) const {
    resultCode(sqlite3_bind_int(stmt, i, v));
  }
  void operator()(const std::int16_t& v) const {
    resultCode(sqlite3_bind_int(stmt, i, v));
  }
  void operator()(const std::uint16_t& v) const {
    resultCode(sqlite3_bind_int(stmt, i, v));
  }
  void operator()(const std::int32_t& v) const {
    resultCode(sqlite3_bind_int(stmt, i, v));
  }
  void operator()(const std::uint32_t& v) const {
    resultCode(sqlite3_bind_int64(stmt, i, v));
  }
  void operator()(const std::int64_t& v) const {
    resultCode(sqlite3_bind_int64(stmt, i, v));
  }
  void operator()(const std::uint64_t& v) const {
    resultCode(sqlite3_bind_int64(stmt, i, gsl::narrow<std::int64_t>(v)));
  }
  void operator()(const float& v) const {
    resultCode(sqlite3_bind_double(stmt, i, v));
  }
  void operator()(const double& v) const {
    resultCode(sqlite3_bind_double(stmt, i, v));
  }
  void operator()(const std::string& v) const {
    const int length = gsl::narrow<int>(v.length());
    resultCode(sqlite3_bind_text(stmt, i, v.data(), length, SQLITE_STATIC));
  }
  void operator()(const BlobType& v) const {
    const int size = gsl::narrow<int>(v.size());
    resultCode(sqlite3_bind_blob(stmt, i, v.data(), size, SQLITE_STATIC));
  }

private:
  void resultCode(int rc) const {
    if (rc != SQLITE_OK) {
      throw Exception(sqlite3_errmsg(sqlite3_db_handle(stmt)));
    }
  }
  sqlite3_stmt* stmt;
  int i;
};

[[nodiscard]] auto getColumn(gsl::not_null<sqlite3_stmt*> stmt, int i) -> ResultTypeValue {

  auto type = sqlite3_column_type(stmt, i);
  switch (type) {
    case SQLITE_INTEGER: {
      return sqlite3_column_int64(stmt, i);
    } break;
    case SQLITE_FLOAT: {
      return sqlite3_column_double(stmt, i);
    } break;
    case SQLITE_TEXT: {
      const auto* const text = sqlite3_column_text(stmt, i);
      if (text == nullptr) {
        return std::string{};
      }
      auto size = gsl::narrow<std::size_t>(sqlite3_column_bytes(stmt, i));
      std::string result(size, 0);
      std::memcpy(result.data(), text, result.size());
      return result;
    } break;
    case SQLITE_BLOB: {
      const auto* const blob = sqlite3_column_blob(stmt, i);
      if (blob == nullptr) {
        return BlobType{};
      }
      auto size = gsl::narrow<std::size_t>(sqlite3_column_bytes(stmt, i));
      BlobType result(size);
      std::memcpy(result.data(), blob, result.size());
      return result;
    } break;
    case SQLITE_NULL: {
      return NullType{};
    } break;
    default: {
      // this should never happen
      std::terminate();
    } break;
  }
}

} // namespace

namespace Y2KaoZ::Database::Sql::Sqlite3 {

Statement::Statement(Connection connection, std::string_view statement)
  : connection_(std::move(connection))
  , stmt_(prepareSqlite3(connection_.backend(), statement), sqlite3_finalize)
  , rows_(false) {
  int parameterCount = sqlite3_bind_parameter_count(stmt_.get());
  parameters_.resize(gsl::narrow<std::size_t>(parameterCount), NullValue);
}

auto Statement::connection() const -> const Connection& {
  return connection_;
}

auto Statement::parameters() const noexcept -> const ParamVector& {
  return parameters_;
}

[[nodiscard]] auto Statement::rows() const noexcept -> bool {
  return rows_;
}

auto Statement::bind(std::size_t index, const ParamType& value) -> Statement& {
  if (index < 1 || index > parameters_.size()) {
    throw std::out_of_range("The index '" + std::to_string(index) + "' is not in the statement.");
  }
  parameters_[index - 1] = value;
  std::visit(StmtValueBinder(stmt_.get(), gsl::narrow<int>(index)), parameters_[index - 1]);
  return *this;
}

auto Statement::bind(std::string_view name, const ParamType& value) -> Statement& {
  auto index = gsl::narrow<std::size_t>(sqlite3_bind_parameter_index(stmt_.get(), name.data()));
  if (index < 1 || index > parameters_.size()) {
    throw std::out_of_range("The name '" + std::string(name) + "' is not a parameter in the statement.");
  }
  bind(index, value);
  return *this;
}

auto Statement::bind(const ParamVector& parameters) -> Statement& {
  auto old = this->parameters_;
  try {
    for (std::size_t i = 0; i < parameters.size(); ++i) {
      bind(i + 1, parameters[i]);
    }
    return *this;
  } catch (const std::out_of_range& e) {
    for (std::size_t i = 0; i < this->parameters_.size(); ++i) {
      bind(i + 1, old[i]);
    }
    throw e;
  }
}

auto Statement::bind(const ParamMap& parameters) -> Statement& {
  auto old = this->parameters_;
  try {
    for (const auto& [name, value] : parameters) {
      bind(name, value);
    }
    return *this;
  } catch (const std::out_of_range& e) {
    for (std::size_t i = 0; i < this->parameters_.size(); ++i) {
      bind(i + 1, old[i]);
    }
    throw e;
  }
}

auto Statement::execute() -> Statement& {
  int rc = sqlite3_step(stmt_.get());
  switch (rc) {
    case SQLITE_DONE:
    case SQLITE_OK: {
      reset();
    } break;
    case SQLITE_ROW:
      rows_ = true;
      break;
    default:
      gsl::finally([&]() {
        sqlite3_reset(stmt_.get());
        rows_ = false;
      });
      throw Exception(sqlite3_errmsg(sqlite3_db_handle(stmt_.get())));
      break;
  }
  return *this;
}

auto Statement::reset() -> Statement& {
  if (sqlite3_reset(stmt_.get()) != SQLITE_OK) {
    throw Exception(sqlite3_errmsg(sqlite3_db_handle(stmt_.get())));
  }
  rows_ = false;
  return *this;
}

auto Statement::clearParameters() -> Statement& {
  if (sqlite3_clear_bindings(stmt_.get()) != SQLITE_OK) {
    throw Exception(sqlite3_errmsg(sqlite3_db_handle(stmt_.get())));
  }
  std::fill(parameters_.begin(), parameters_.end(), NullValue);
  return *this;
}

auto Statement::columnCount() const -> std::size_t {
  return static_cast<std::size_t>(sqlite3_column_count(stmt_.get()));
}

auto Statement::columnName(std::size_t i) const -> std::string {
  return sqlite3_column_name(stmt_.get(), gsl::narrow<int>(i));
}

auto Statement::fetchVector() -> std::optional<ResultVector> {
  if (!rows_) {
    return {};
  }
  int count = gsl::narrow<int>(columnCount());
  ResultVector result;
  result.reserve(static_cast<std::size_t>(count));
  for (int i = 0; i < count; ++i) {
    result.emplace_back(::getColumn(stmt_.get(), i));
  }
  execute();
  return result;
}

auto Statement::fetchMap() -> std::optional<ResultMap> {
  if (!rows_) {
    return {};
  }
  int count = gsl::narrow<int>(columnCount());
  ResultMap result;
  for (int i = 0; i < count; ++i) {
    result[columnName(static_cast<std::size_t>(i))] = ResultType(::getColumn(stmt_.get(), i));
  }
  execute();
  return result;
}

auto Statement::fetchColumn(std::size_t i) -> std::optional<ResultType> {
  if (!rows_) {
    return {};
  }
  if (i >= columnCount()) {
    throw std::out_of_range("The column index is out of bounds.");
  }
  ResultType result(::getColumn(stmt_.get(), gsl::narrow<int>(i)));
  execute();
  return result;
}

auto Statement::fetchAllVector() -> std::vector<ResultVector> {
  std::vector<ResultVector> result;
  while (auto row = fetchVector()) {
    result.emplace_back(*row);
  }
  return result;
}

auto Statement::fetchAllMap() -> std::vector<ResultMap> {
  std::vector<ResultMap> result;
  while (auto row = fetchMap()) {
    result.emplace_back(*row);
  }
  return result;
}

} // namespace Y2KaoZ::Database::Sql::Sqlite3