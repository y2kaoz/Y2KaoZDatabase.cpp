#include "Y2KaoZ/Database/Types.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>
#include <stdexcept>

TEST_CASE("ResultType Creation") { // NOLINT

  SECTION("The default constructor") {
    using Y2KaoZ::Database::ResultType;

    ResultType resultType;

    CHECK(resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(!resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::Null);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getString(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK(resultType.asInteger64() == 0);
    CHECK(resultType.asReal() == 0.0);
    CHECK(resultType.asString().empty());
    CHECK(resultType.asBlob().empty());
  }

  SECTION("With a NullType Value") {
    using Y2KaoZ::Database::NullValue;
    using Y2KaoZ::Database::ResultType;

    ResultType resultType(NullValue);

    CHECK(resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(!resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::Null);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getString(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK(resultType.asInteger64() == 0);
    CHECK(resultType.asReal() == 0.0);
    CHECK(resultType.asString().empty());
    CHECK(resultType.asBlob().empty());
  }

  SECTION("With an Integer Value") {
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    const int AN_INTEGER = 0xFF;

    ResultType resultType(AN_INTEGER);

    CHECK(!resultType.isNull());
    CHECK(resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(!resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::Integer);
    CHECK_NOTHROW(resultType.getInteger());
    CHECK(resultType.getInteger() == AN_INTEGER);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getString(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK(resultType.asInteger64() == 0xFF);
    CHECK(resultType.asReal() == AN_INTEGER);
    CHECK(resultType.asString() == "255");
    CHECK(!resultType.asBlob().empty());
    CHECK(resultType.asBlob().size() == 8);
    CHECK(resultType.asBlob()[0] == std::byte(0xFF));
    CHECK(resultType.asBlob()[1] == std::byte(0x00));
    CHECK(resultType.asBlob()[2] == std::byte(0x00));
    CHECK(resultType.asBlob()[3] == std::byte(0x00));
    CHECK(resultType.asBlob()[4] == std::byte(0x00));
    CHECK(resultType.asBlob()[5] == std::byte(0x00));
    CHECK(resultType.asBlob()[6] == std::byte(0x00));
    CHECK(resultType.asBlob()[7] == std::byte(0x00));
  }

  SECTION("With a Real Value") {
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    const float A_FLOAT = 3.14F;

    ResultType resultType(A_FLOAT);

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(resultType.isReal());
    CHECK(!resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::Real);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getReal());
    CHECK(resultType.getReal() == Catch::Approx(A_FLOAT));
    CHECK_THROWS_AS(resultType.getString(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK(resultType.asInteger64() == 3);
    CHECK(resultType.asReal() == Catch::Approx(A_FLOAT));
    REQUIRE_THAT(resultType.asString(), Catch::Matchers::StartsWith("3.14"));
    CHECK(!resultType.asBlob().empty());
    CHECK(resultType.asBlob().size() == 8);
  }

  SECTION("With an empty String Value") {
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    ResultType resultType("");

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::String);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getString());
    CHECK(resultType.getString().empty());
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.asInteger64(), std::invalid_argument);
    CHECK_THROWS_AS(resultType.asReal(), std::invalid_argument);
    CHECK(resultType.asString().empty());
    CHECK(resultType.asBlob().empty());
  }

  SECTION("With a Hexadecimal String Value") {
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    ResultType resultType("0x0102");

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::String);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getString());
    CHECK(resultType.getString() == "0x0102");
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK(resultType.asInteger64() == 258);
    CHECK(resultType.asReal() == 258);
    CHECK(resultType.asString() == "0x0102");
    CHECK(!resultType.asBlob().empty());
    CHECK(resultType.asBlob().size() == 6);
    CHECK(resultType.asBlob()[0] == std::byte('0'));
    CHECK(resultType.asBlob()[1] == std::byte('x'));
    CHECK(resultType.asBlob()[2] == std::byte('0'));
    CHECK(resultType.asBlob()[3] == std::byte('1'));
    CHECK(resultType.asBlob()[4] == std::byte('0'));
    CHECK(resultType.asBlob()[5] == std::byte('2'));
  }

  SECTION("With a Decimal String Value") {
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    ResultType resultType("258");

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::String);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getString());
    CHECK(resultType.getString() == "258");
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK(resultType.asInteger64() == 258);
    CHECK(resultType.asReal() == 258);
    CHECK(resultType.asString() == "258");
    CHECK(!resultType.asBlob().empty());
    CHECK(resultType.asBlob().size() == 3);
    CHECK(resultType.asBlob()[0] == std::byte('2'));
    CHECK(resultType.asBlob()[1] == std::byte('5'));
    CHECK(resultType.asBlob()[2] == std::byte('8'));
  }

  SECTION("With a Text String Value") {
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    ResultType resultType("hello");

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(resultType.isString());
    CHECK(!resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::String);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getString());
    CHECK(resultType.getString() == "hello");
    CHECK_THROWS_AS(resultType.getBlob(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.asInteger64(), std::invalid_argument);
    CHECK_THROWS_AS(resultType.asReal(), std::invalid_argument);
    CHECK(resultType.asString() == "hello");
    CHECK(!resultType.asBlob().empty());
    CHECK(resultType.asBlob().size() == 5);
    CHECK(resultType.asBlob()[0] == std::byte('h'));
    CHECK(resultType.asBlob()[1] == std::byte('e'));
    CHECK(resultType.asBlob()[2] == std::byte('l'));
    CHECK(resultType.asBlob()[3] == std::byte('l'));
    CHECK(resultType.asBlob()[4] == std::byte('o'));
  }

  SECTION("With a Empty Blob Value") {
    using Y2KaoZ::Database::BlobType;
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    BlobType blob{};

    ResultType resultType(blob);

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(!resultType.isString());
    CHECK(resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::Blob);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getString(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getBlob());
    CHECK(resultType.asBlob() == blob);
    CHECK_THROWS_AS(resultType.asInteger64(), std::out_of_range);
    CHECK_THROWS_AS(resultType.asReal(), std::out_of_range);
    CHECK(resultType.asString().empty());
    CHECK(resultType.asBlob().empty());
  }

  SECTION("With a Blob Value") {
    using Y2KaoZ::Database::BlobType;
    using Y2KaoZ::Database::NullValue;
    using ResultType = Y2KaoZ::Database::ResultType;

    BlobType blob{std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04)};

    ResultType resultType(blob);

    CHECK(!resultType.isNull());
    CHECK(!resultType.isInteger());
    CHECK(!resultType.isReal());
    CHECK(!resultType.isString());
    CHECK(resultType.isBlob());
    CHECK(resultType.getType() == ResultType::Type::Blob);
    CHECK_THROWS_AS(resultType.getInteger(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getReal(), std::bad_variant_access);
    CHECK_THROWS_AS(resultType.getString(), std::bad_variant_access);
    CHECK_NOTHROW(resultType.getBlob());
    CHECK(resultType.asBlob() == blob);
    CHECK(resultType.asInteger64() == 67305985);
    // GOT THIS VALUE CHEATING
    static const double FROM_HEX_CALCULATOR = 1.53998961e-36;
    CHECK(resultType.asReal() == Catch::Approx(FROM_HEX_CALCULATOR));
    CHECK(resultType.asString() == std::string({0x01, 0x02, 0x03, 0x04}));
    CHECK(!resultType.asBlob().empty());
    CHECK(resultType.asBlob().size() == 4);
    CHECK(resultType.asBlob()[0] == std::byte(0x01));
    CHECK(resultType.asBlob()[1] == std::byte(0x02));
    CHECK(resultType.asBlob()[2] == std::byte(0x03));
    CHECK(resultType.asBlob()[3] == std::byte(0x04));
  }
}