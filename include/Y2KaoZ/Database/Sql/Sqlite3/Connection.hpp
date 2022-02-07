#pragma once

#include "Y2KaoZ/Database/Visibility.hpp"
#include <filesystem>
#include <gsl/pointers>
#include <sqlite3.h>

namespace Y2KaoZ::Database::Sql::Sqlite3 {

class Transaction;
class Statement;

class Y2KAOZDATABASE_EXPORT Connection {
public:
  static const std::uint32_t OPEN_READWRITE = SQLITE_OPEN_READWRITE;
  static const std::uint32_t OPEN_CREATE = SQLITE_OPEN_CREATE;
  /// A private, temporary in-memory database is created for the connection.
  Connection();

  /// Opens the database in filename for the connection.
  explicit Connection(const std::filesystem::path& filename, std::uint32_t flags = OPEN_READWRITE | OPEN_CREATE);

  /// @brief Returns the raw sqlite3 backend pointer
  [[nodiscard]] auto backend() const -> gsl::not_null<sqlite3*>;

  /// @brief Executes semicolon-separated SQL statements
  void execute(std::string_view statements) const;

  /// @brief Returns the number of rows affected by the last SQL statement
  [[nodiscard]] auto rowCount() const -> std::size_t;

  /// @brief Returns the ID of the last inserted row or sequence value
  [[nodiscard]] auto lastInsertRowId() const -> std::int64_t;

  /// @brief Prepares a statement for execution and returns a statement object
  [[nodiscard]] auto prepare(std::string_view statement) -> Statement;

  /// @brief Initiates a transaction
  [[nodiscard]] auto beginTransaction() -> Transaction;

private:
  std::shared_ptr<sqlite3> db_;
};

} // namespace Y2KaoZ::Database::Sql::Sqlite3