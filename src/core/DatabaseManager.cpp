#include "../../include/core/DatabaseManager.h"


DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

void DatabaseManager::disconnect() {
    if(db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void DatabaseManager::connect(const std::string& dbPath) {
    if(db_) disconnect();
    sqlite3* connection;
    if(sqlite3_open(dbPath.c_str(), &connection) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(connection));
    }
    db_ = connection;
    execute("PRAGMA foreign_keys = ON");
}

void DatabaseManager::check_connection() const {
    if(!db_) throw std::runtime_error("Database not connected");
}

sqlite3_stmt* DatabaseManager::prepare_statement(const std::string& query) {
    check_connection();
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db_));
    }
    return stmt;
}

void DatabaseManager::execute(const std::string& query) {
    sqlite3_stmt* stmt = prepare_statement(query);
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute query");
    }
    sqlite3_finalize(stmt);
}

void DatabaseManager::execute(const std::string& query, const std::vector<std::string>& params) {
    sqlite3_stmt* stmt = prepare_statement(query);
    
    for(size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, i+1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute query with parameters");
    }
    sqlite3_finalize(stmt);
}

std::vector<DatabaseManager::ResultRow> DatabaseManager::fetch_all(const std::string& query, 
                                                                 const std::vector<std::string>& params) {
    std::vector<ResultRow> result;
    sqlite3_stmt* stmt = prepare_statement(query);
    
    for(size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, i+1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        ResultRow row;
        int col_count = sqlite3_column_count(stmt);
        for(int i = 0; i < col_count; ++i) {
            row.columns.emplace_back(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))
            );
        }
        result.push_back(row);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

void DatabaseManager::begin_transaction() {
    execute("BEGIN TRANSACTION");
}

void DatabaseManager::commit_transaction() {
    execute("COMMIT");
}

void DatabaseManager::rollback_transaction() {
    execute("ROLLBACK");
}