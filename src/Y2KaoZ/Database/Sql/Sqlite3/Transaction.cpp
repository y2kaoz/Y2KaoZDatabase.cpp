#include "Y2KaoZ/Database/Sql/Sqlite3/Transaction.hpp"

namespace Y2KaoZ::Database::Sql::Sqlite3 {

Transaction::Transaction(Connection connection) : connection_(std::move(connection)), finished_(false) {
  this->connection_.execute("BEGIN TRANSACTION;");
}

Transaction::Transaction(Transaction&& other) noexcept
  : connection_(std::move(other.connection_))
  , finished_(other.finished_) {
  other.finished_ = true;
}

void Transaction::commit() {
  if (!finished_) {
    connection_.execute("COMMIT TRANSACTION;");
    finished_ = true;
  }
}

void Transaction::rollBack() {
  if (!finished_) {
    connection_.execute("ROLLBACK TRANSACTION;");
    finished_ = true;
  }
}

Transaction::~Transaction() {
  rollBack();
}

} // namespace Y2KaoZ::Database::Sql::Sqlite3
