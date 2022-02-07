#pragma once

#include "Y2KaoZ/Database/Sql/Sqlite3/Connection.hpp"
#include "Y2KaoZ/Database/Visibility.hpp"

namespace Y2KaoZ::Database::Sql::Sqlite3 {

class Y2KAOZDATABASE_EXPORT Transaction {
public:
  Transaction() = delete;
  Transaction(const Transaction&) = delete;
  auto operator=(const Transaction&) -> Transaction& = delete;
  auto operator=(Transaction&&) -> Transaction& = delete;

  explicit Transaction(Connection connection);
  Transaction(Transaction&& other) noexcept;

  /// @brief Commit the transaction
  void commit();

  /// @brief Rolls back the transaction
  void rollBack();

  /// @brief Calls rollBack if commit has not already been called.
  ~Transaction();

private:
  Connection connection_;
  bool finished_;
};

} // namespace Y2KaoZ::Database::Sql::Sqlite3