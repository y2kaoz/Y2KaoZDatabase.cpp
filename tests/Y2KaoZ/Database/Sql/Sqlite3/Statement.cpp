#include "Y2KaoZ/Database/Sql/Sqlite3.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Statement Creation") { // NOLINT
  using Y2KaoZ::Database::Sql::Sqlite3::Connection;
  Connection connection{};
  connection.execute("CREATE TABLE valid (a, b);");

  SECTION("Invalid tablename") {
    using Y2KaoZ::Database::Sql::Sqlite3::Exception;
    using Y2KaoZ::Database::Sql::Sqlite3::Statement;
    REQUIRE_THROWS_AS(Statement(connection, "SELECT * FROM invalid;"), Exception);
  }

  SECTION("Invalid fields") {
    using Y2KaoZ::Database::Sql::Sqlite3::Exception;
    using Y2KaoZ::Database::Sql::Sqlite3::Statement;
    REQUIRE_THROWS_AS(Statement(connection, "SELECT * FROM valid WHERE c=?;"), Exception);
  }

  SECTION("Invalid field count") {
    using Y2KaoZ::Database::Sql::Sqlite3::Exception;
    using Y2KaoZ::Database::Sql::Sqlite3::Statement;
    REQUIRE_THROWS_AS(Statement(connection, "INSERT INTO valid (?);"), Exception);
  }

  SECTION("Valid table and fields") {
    using Y2KaoZ::Database::Sql::Sqlite3::Statement;
    REQUIRE_NOTHROW(Statement(connection, "SELECT * FROM valid WHERE a=?;"));
  }
}

TEST_CASE("Binding values") { // NOLINT
  using Y2KaoZ::Database::Sql::Sqlite3::Connection;
  Connection connection{};
  connection.execute("CREATE TABLE valid (a, b NOT NULL);");

  SECTION("Invalid parameter index or name") {
    using Y2KaoZ::Database::NullType;
    using Y2KaoZ::Database::NullValue;
    using Y2KaoZ::Database::ParamMap;
    using Y2KaoZ::Database::ParamVector;
    using Y2KaoZ::Database::Sql::Sqlite3::Statement;
    Statement stmt(connection, "INSERT INTO valid VALUES (?,:name)");
    const auto& bindings = stmt.parameters();

    REQUIRE(std::holds_alternative<NullType>(bindings[0]));
    REQUIRE(std::holds_alternative<NullType>(bindings[1]));
    REQUIRE(std::get<NullType>(bindings[0]) == NullValue);
    REQUIRE(std::get<NullType>(bindings[1]) == NullValue);

    REQUIRE_THROWS_AS(stmt.bind(0, 1), std::out_of_range);
    REQUIRE_THROWS_AS(stmt.bind(3, 1), std::out_of_range);
    REQUIRE_THROWS_AS(stmt.bind("badName", 1), std::out_of_range);
    REQUIRE_THROWS_AS(stmt.bind(ParamMap{{"a", 1}}), std::out_of_range);
    REQUIRE_THROWS_AS(stmt.bind(ParamVector{1, 2, 3}), std::out_of_range);

    REQUIRE(std::holds_alternative<NullType>(bindings[0]));
    REQUIRE(std::holds_alternative<NullType>(bindings[1]));
    REQUIRE(std::get<NullType>(bindings[0]) == NullValue);
    REQUIRE(std::get<NullType>(bindings[1]) == NullValue);
  }

  SECTION("Valid parameter index or name") {
    using Y2KaoZ::Database::NullType;
    using Y2KaoZ::Database::NullValue;
    using Y2KaoZ::Database::ParamMap;
    using Y2KaoZ::Database::ParamVector;
    using Y2KaoZ::Database::Sql::Sqlite3::Statement;
    Statement stmt(connection, "INSERT INTO valid VALUES (?,:name)");
    const auto& bindings = stmt.parameters();

    REQUIRE_NOTHROW(stmt.bind(1, 1));
    REQUIRE(std::holds_alternative<int>(bindings[0]));
    REQUIRE(std::holds_alternative<NullType>(bindings[1]));
    REQUIRE(std::get<int>(bindings[0]) == 1);
    REQUIRE(std::get<NullType>(bindings[1]) == NullValue);

    REQUIRE_NOTHROW(stmt.bind(2, "2"));
    REQUIRE(std::holds_alternative<int>(bindings[0]));
    REQUIRE(std::holds_alternative<std::string>(bindings[1]));
    REQUIRE(std::get<int>(bindings[0]) == 1);
    REQUIRE(std::get<std::string>(bindings[1]) == "2");

    REQUIRE_NOTHROW(stmt.bind(":name", 0.0));
    REQUIRE(std::holds_alternative<int>(bindings[0]));
    REQUIRE(std::holds_alternative<double>(bindings[1]));
    REQUIRE(std::get<int>(bindings[0]) == 1);
    REQUIRE(std::get<double>(bindings[1]) == 0.0);

    REQUIRE_NOTHROW(stmt.bind(ParamMap{{":name", 4}}));
    REQUIRE(std::holds_alternative<int>(bindings[0]));
    REQUIRE(std::holds_alternative<int>(bindings[1]));
    REQUIRE(std::get<int>(bindings[0]) == 1);
    REQUIRE(std::get<int>(bindings[1]) == 4);

    REQUIRE_NOTHROW(stmt.bind(ParamVector{"5", 0.0F}));
    REQUIRE(std::holds_alternative<std::string>(bindings[0]));
    REQUIRE(std::holds_alternative<float>(bindings[1]));
    REQUIRE(std::get<std::string>(bindings[0]) == "5");
    REQUIRE(std::get<float>(bindings[1]) == 0.0F);

    REQUIRE_NOTHROW(stmt.clearParameters());
    REQUIRE(std::holds_alternative<NullType>(bindings[0]));
    REQUIRE(std::holds_alternative<NullType>(bindings[1]));
    REQUIRE(std::get<NullType>(bindings[0]) == NullValue);
    REQUIRE(std::get<NullType>(bindings[1]) == NullValue);
  }
}

TEST_CASE("Executing statements") { // NOLINT
  using Y2KaoZ::Database::Sql::Sqlite3::Connection;
  Connection connection{};
  connection.execute("CREATE TABLE valid (a, b NOT NULL);");

  SECTION("Executing statements with no parameters") {
    using Y2KaoZ::Database::Sql::Sqlite3::Exception;
    REQUIRE_NOTHROW(connection.prepare("INSERT INTO valid VALUES (1,2), (2,3), (3,4);").execute());
    REQUIRE(connection.rowCount() == 3);
    REQUIRE_THROWS_AS(connection.prepare("INSERT INTO valid VALUES (1,NULL)").execute(), Exception);
    REQUIRE_NOTHROW(connection.prepare("DELETE FROM valid;").execute());
    REQUIRE(connection.rowCount() == 3);
  }
  SECTION("Executing statements with parameters") {
    using Y2KaoZ::Database::Sql::Sqlite3::Exception;
    auto stmt = connection.prepare("INSERT INTO valid VALUES (?,?)");
    REQUIRE_THROWS_AS(stmt.execute(), Exception);
    REQUIRE(connection.rowCount() == 0);
    REQUIRE_THROWS_AS(stmt.bind(1, 1).execute(), Exception);
    REQUIRE(connection.rowCount() == 0);
    stmt.clearParameters();
    REQUIRE_NOTHROW(stmt.bind(2, 1).execute());
    REQUIRE(connection.rowCount() == 1);
  }
}

TEST_CASE("Fetchig data") { // NOLINT
  using Y2KaoZ::Database::Sql::Sqlite3::Connection;
  Connection connection{};
  connection.execute("CREATE TABLE valid (a NOT NULL, b);"
                     "INSERT INTO valid VALUES (1,'2'), (2,3), (3, NULL);");
  SECTION("On an empty result") {
    auto stmt = connection.prepare("SELECT * FROM valid WHERE a IS NULL;");
    REQUIRE(!stmt.rows());
    stmt.execute();
    REQUIRE(!stmt.rows());
    REQUIRE(stmt.columnCount() == 2);
    REQUIRE(stmt.columnName(0) == "a");
    REQUIRE(stmt.columnName(1) == "b");
    REQUIRE(!stmt.fetchVector());
    REQUIRE(!stmt.fetchMap());
    REQUIRE(stmt.fetchAllVector().empty());
    REQUIRE(stmt.fetchAllMap().empty());
  }

  SECTION("One Row at a time") {
    auto stmt = connection.prepare("SELECT * FROM valid;");
    REQUIRE(!stmt.rows());
    stmt.execute();
    REQUIRE(stmt.rows());
    REQUIRE(stmt.columnCount() == 2);
    REQUIRE(stmt.columnName(0) == "a");
    REQUIRE(stmt.columnName(1) == "b");

    auto row1 = stmt.fetchVector();
    REQUIRE(row1);
    REQUIRE(stmt.rows());
    REQUIRE(row1->at(0).isInteger());
    REQUIRE(row1->at(0).getInteger() == 1);
    REQUIRE(row1->at(1).isString());
    REQUIRE(row1->at(1).getString() == "2");

    auto row2 = stmt.fetchMap();
    REQUIRE(row2);
    REQUIRE(stmt.rows());
    REQUIRE(row2->at("a").isInteger());
    REQUIRE(row2->at("a").getInteger() == 2);
    REQUIRE(row2->at("b").isInteger());
    REQUIRE(row2->at("b").getInteger() == 3);

    auto row3 = stmt.fetchMap();
    REQUIRE(row3);
    REQUIRE(!stmt.rows());
    REQUIRE(row3->at("a").isInteger());
    REQUIRE(row3->at("a").getInteger() == 3);
    REQUIRE(row3->at("b").isNull());

    auto noRow = stmt.fetchMap();
    REQUIRE(!noRow);
  }

  SECTION("All rows at once as Vectors") {
    auto stmt = connection.prepare("SELECT * FROM valid;");
    REQUIRE(!stmt.rows());
    stmt.execute();
    REQUIRE(stmt.rows());
    auto rows = stmt.fetchAllVector();
    REQUIRE(!stmt.rows());

    REQUIRE(rows.size() == 3);
    REQUIRE(rows[0][0].isInteger());
    REQUIRE(rows[0][0].getInteger() == 1);
    REQUIRE(rows[0][1].isString());
    REQUIRE(rows[0][1].getString() == "2");

    REQUIRE(rows[1][0].isInteger());
    REQUIRE(rows[1][0].getInteger() == 2);
    REQUIRE(rows[1][1].isInteger());
    REQUIRE(rows[1][1].getInteger() == 3);
  }

  SECTION("All rows at once as Maps") {
    auto stmt = connection.prepare("SELECT * FROM valid;");
    REQUIRE(!stmt.rows());
    stmt.execute();
    REQUIRE(stmt.rows());
    auto rows = stmt.fetchAllMap();
    REQUIRE(!stmt.rows());

    REQUIRE(rows.size() == 3);
    REQUIRE(rows[0].at("a").isInteger());
    REQUIRE(rows[0].at("a").getInteger() == 1);
    REQUIRE(rows[0].at("b").isString());
    REQUIRE(rows[0].at("b").getString() == "2");

    REQUIRE(rows[1].at("a").isInteger());
    REQUIRE(rows[1].at("a").getInteger() == 2);
    REQUIRE(rows[1].at("b").isInteger());
    REQUIRE(rows[1].at("b").getInteger() == 3);
  }
}