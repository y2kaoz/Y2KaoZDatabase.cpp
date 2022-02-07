#pragma once

#include "Y2KaoZ/Database/Visibility.hpp"
#include <stdexcept>

namespace Y2KaoZ::Database::Sql::Sqlite3 {

class Y2KAOZDATABASE_EXPORT Exception : public std::runtime_error {
public:
  explicit Exception(const std::string& what);
};

} // namespace Y2KaoZ::Database::Sql::Sqlite3