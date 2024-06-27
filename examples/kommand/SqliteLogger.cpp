#include "SqliteLogger.h"
#include <ctime>
#include <iomanip>
#include <sstream>

std::string SqliteLogger::current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

SqliteLogger::SqliteLogger(const std::string& db_name) {
    int rc = sqlite3_open(db_name.c_str(), &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    
    const char* sql = "CREATE TABLE IF NOT EXISTS logs ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "timestamp TEXT,"
                      "text TEXT,"
                      "duration INTEGER,"
                      "probability REAL);";
    char* errMsg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
}

SqliteLogger::~SqliteLogger() {
    sqlite3_close(db);
}

void SqliteLogger::log_to_db(const std::string& txt, int t_ms, double p) {
    std::string timestamp = current_timestamp();
    std::string full_text = " [ " + txt + " ]";
    
    std::string sql = "INSERT INTO logs (timestamp, text, duration, probability) VALUES (?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    sqlite3_bind_text(stmt, 1, timestamp.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, full_text.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, t_ms);
    sqlite3_bind_double(stmt, 4, p);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
}