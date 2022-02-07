#include "Y2KaoZ/Database/Sql/Sqlite3/Exception.hpp"

namespace Y2KaoZ::Database::Sql::Sqlite3 {

Exception::Exception(const std::string& what) : std::runtime_error(what) {
}

} // namespace Y2KaoZ::Database::Sql::Sqlite3