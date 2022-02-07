#include "Y2KaoZ/Database/Sql/Sqlite3/Connection.hpp"
#include "Y2KaoZ/Database/Sql/Sqlite3/Exception.hpp"
#include "Y2KaoZ/Database/Sql/Sqlite3/Statement.hpp"
#include "Y2KaoZ/Database/Sql/Sqlite3/Transaction.hpp"
#include <cstddef>
#include <fmt/format.h>
#include <gsl/gsl_util>

namespace {

[[nodiscard]] auto connect(const std::string_view& filename, std::uint32_t flags) -> gsl::not_null<sqlite3*> {
  using Y2KaoZ::Database::Sql::Sqlite3::Exception;
  sqlite3* db = nullptr;
  if (sqlite3_open_v2(filename.data(), &db, gsl::narrow<int>(flags), nullptr) != SQLITE_OK) {
    gsl::finally([&]() { sqlite3_close(db); });
    throw Exception(sqlite3_errmsg(db));
  }
  return db;
}

} // namespace

namespace Y2KaoZ::Database::Sql::Sqlite3 {

Connection::Connection() : db_(connect(":memory:", OPEN_READWRITE | OPEN_CREATE).get(), sqlite3_close) {
}

Connection::Connection(const std::filesystem::path& filename, std::uint32_t flags)
  : db_(connect(filename.string(), flags).get(), sqlite3_close) {
}

auto Connection::backend() const -> gsl::not_null<sqlite3*> {
  return db_.get();
}

void Connection::execute(std::string_view statement) const {
  char* errmsg = nullptr;
  if (sqlite3_exec(db_.get(), statement.data(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
    gsl::finally([&]() { sqlite3_free(errmsg); });
    throw Exception(fmt::format(R"(Error in "{}": "{}")", statement, errmsg));
  }
}

auto Connection::rowCount() const -> std::size_t {
  return gsl::narrow<std::size_t>(sqlite3_changes(db_.get()));
}

auto Connection::lastInsertRowId() const -> std::int64_t {
  return sqlite3_last_insert_rowid(db_.get());
}

auto Connection::prepare(std::string_view statement) -> Statement {
  return {*this, statement};
}

auto Connection::beginTransaction() -> Transaction {
  return Transaction{*this};
}

} // namespace Y2KaoZ::Database::Sql::Sqlite3