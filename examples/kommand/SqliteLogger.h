#ifndef SQLITE_LOGGER_H
#define SQLITE_LOGGER_H

#include <sqlite3.h>
#include <string>

class SqliteLogger {
private:
    sqlite3* db;
    std::string current_timestamp();

public:
    SqliteLogger(const std::string& db_name);
    ~SqliteLogger();

    void log_to_db(const std::string& txt, int t_ms, double p);
};

#endif // SQLITE_LOGGER_H