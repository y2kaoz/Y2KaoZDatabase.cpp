#include "Y2KaoZ/Database/Sql/Sqlite3.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Connection Creation") { // NOLINT
  using Y2KaoZ::Database::Sql::Sqlite3::Connection;
  using Y2KaoZ::Database::Sql::Sqlite3::Exception;
  SECTION("The default constructor") {
    CHECK_NOTHROW(Connection{});
  }
  SECTION("Invalid filename") {
    CHECK_THROWS_AS(Connection{"/home/notauser/test.sqlite3"}, Exception);
  }
  SECTION("Valid filename") {
    std::filesystem::path tmp = std::filesystem::temp_directory_path() / "weirdFileNameToTestSqliteDatabase.sqlite3";
    REQUIRE(!std::filesystem::exists(tmp));
    CHECK_NOTHROW(Connection{tmp});
    REQUIRE(std::filesystem::exists(tmp));
    if (std::filesystem::exists(tmp)) {
      std::filesystem::remove(tmp);
    }
    REQUIRE(!std::filesystem::exists(tmp));
  }
}

TEST_CASE("Creating some data in :memory: database") { // NOLINT
  using Y2KaoZ::Database::Sql::Sqlite3::Connection;
  using Y2KaoZ::Database::Sql::Sqlite3::Exception;
  Connection connection;

  SECTION("Creating transactions don't throw") {
    CHECK_NOTHROW(connection.beginTransaction());
  }

  SECTION("Execute with invalid SQL") {
    auto transaction = connection.beginTransaction();
    CHECK_THROWS_AS(connection.execute("CRATE YAVLE invalid (a, b);"), Exception);
    CHECK_THROWS_AS(connection.execute("DROP TABLE invalid;"), Exception);
  }

  SECTION("Execute with valid SQL and some data") {
    auto transaction = connection.beginTransaction();
    CHECK_NOTHROW(connection.execute("CREATE TABLE valid (a, b);"
                                     "INSERT INTO valid VALUES (1,2), (2,3), (3,4);"));
    CHECK(connection.rowCount() == 3);
    CHECK_NOTHROW(connection.execute("INSERT INTO valid VALUES (5,6);"));
    CHECK(connection.lastInsertRowId() == 4);
    CHECK_NOTHROW(connection.execute("DELETE FROM valid;"
                                     "DROP TABLE valid;"));
    CHECK(connection.rowCount() == 4);
  }

  SECTION("Preparing statements") {
    auto transaction = connection.beginTransaction();
    CHECK_NOTHROW(connection.execute("CREATE TABLE valid (a, b);"));
    CHECK_THROWS_AS(connection.prepare("INSRT TO valid VALUES (?,?);"), Exception);
    CHECK_NOTHROW(connection.prepare("INSERT INTO valid VALUES (?,?);"));
  }
}