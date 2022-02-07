#pragma once

#include "Y2KaoZ/Database/Sql/Sqlite3/Connection.hpp"
#include "Y2KaoZ/Database/Types.hpp"
#include "Y2KaoZ/Database/Visibility.hpp"
#include <optional>

namespace Y2KaoZ::Database::Sql::Sqlite3 {

class Y2KAOZDATABASE_EXPORT Statement {
public:
  Statement() = delete;
  Statement(const Statement&) = delete;
  Statement(Statement&& other) = default;
  Statement(Connection connection, std::string_view statement);
  ~Statement() = default;

  auto operator=(const Statement&) -> Statement& = delete;
  auto operator=(Statement&&) -> Statement& = default;

  /// @brief Returns the connection used for the creation of this statement.
  [[nodiscard]] auto connection() const -> const Connection&;

  /// @brief Returns all parameter bindings
  [[nodiscard]] auto parameters() const noexcept -> const ParamVector&;

  /// @brief Returs true if the statement has rows to fetch, false otherwise.
  [[nodiscard]] auto rows() const noexcept -> bool;

  /// @brief Binds a value to a parameter
  auto bind(std::size_t index, const ParamType& value) -> Statement&;
  auto bind(std::string_view name, const ParamType& value) -> Statement&;
  auto bind(const ParamVector& parameters) -> Statement&;
  auto bind(const ParamMap& parameters) -> Statement&;

  /// @brief Executes a prepared statement
  /// @note if The execution has no more rows it will call reset automatically.
  auto execute() -> Statement&;

  /// @brief Resets statement back to its initial state, ready to be re-executed using the same bindings.
  auto reset() -> Statement&;

  /// @brief Resets all parameter bindings to NULL.
  auto clearParameters() -> Statement&;

  /// @brief Returns the number of columns in the result set
  [[nodiscard]] auto columnCount() const -> std::size_t;

  /// @brief Returns the column name for column i
  [[nodiscard]] auto columnName(std::size_t i) const -> std::string;

  /// @brief Fetches the next row from a result set using numeric column keys
  [[nodiscard]] auto fetchVector() -> std::optional<ResultVector>;

  /// @brief Fetches the next row from a result set using string column keys
  [[nodiscard]] auto fetchMap() -> std::optional<ResultMap>;

  /// @brief Returns a single column from the next row of a result set
  [[nodiscard]] auto fetchColumn(std::size_t i) -> std::optional<ResultType>;

  /// @brief Fetches the remaining rows from a result set as a vector
  [[nodiscard]] auto fetchAllVector() -> std::vector<ResultVector>;

  /// @brief Fetches the remaining rows from a result set as a map
  [[nodiscard]] auto fetchAllMap() -> std::vector<ResultMap>;

private:
  Connection connection_;
  std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt_;
  ParamVector parameters_;
  bool rows_;
};

} // namespace Y2KaoZ::Database::Sql::Sqlite3