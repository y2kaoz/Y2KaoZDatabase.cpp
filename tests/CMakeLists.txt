add_executable(DatabaseTypesTests Y2KaoZ/Database/Types.cpp)
add_test(NAME DatabaseTypesTests COMMAND DatabaseTypesTests)

add_executable(Sqlite3ConnectionTests Y2KaoZ/Database/Sql/Sqlite3/Connection.cpp)
add_test(NAME Sqlite3ConnectionTests COMMAND Sqlite3ConnectionTests)

add_executable(Sqlite3StatementTests Y2KaoZ/Database/Sql/Sqlite3/Statement.cpp)
add_test(NAME Sqlite3StatementTests COMMAND Sqlite3StatementTests)

find_package(Catch2 3 REQUIRED)
target_link_libraries(DatabaseTypesTests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
target_link_libraries(Sqlite3ConnectionTests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
target_link_libraries(Sqlite3StatementTests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
