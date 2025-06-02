#pragma once

#include <sqlite3.h>
#include <vector>
#include <string>
#include <stdexcept>

class DatabaseManager {
public:
    struct ResultRow {
        std::vector<std::string> columns;
    };
    
    static DatabaseManager& instance();
    
    void connect(const std::string& dbPath);
    void disconnect();
    bool is_connected() const;
    
    void execute(const std::string& query);
    void execute(const std::string& query, const std::vector<std::string>& params);
    
    std::vector<ResultRow> fetch_all(const std::string& query, 
                                   const std::vector<std::string>& params = {});
    
    void begin_transaction();
    void commit_transaction();
    void rollback_transaction();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    
    sqlite3* db_ = nullptr;
    
    void check_connection() const;
    sqlite3_stmt* prepare_statement(const std::string& query);
};