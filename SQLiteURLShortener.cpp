#include "SQLiteURLShortener.h"
#include "SQLiteException.h"
#include <iostream>

SQLiteURLShortener::SQLiteURLShortener(const std::string& baseURL)
    : IURLShortener(baseURL), db(nullptr)
{
    initializeDatabase();
}

SQLiteURLShortener::~SQLiteURLShortener()
{
    if (db) {
        sqlite3_close(db);
    }
}

void SQLiteURLShortener::initializeDatabase()
{
    // Open the database connection
    int rc = sqlite3_open("url_shortener.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
        return;
    }

    // Create the URL mapping table if it doesn't exist
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS url_mappings (
            id INTEGER PRIMARY KEY,
            short_code TEXT UNIQUE NOT NULL,
            long_url TEXT NOT NULL
        );
    )";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string errorMsg = "SQL error in creating table: ";
        errorMsg += errMsg;
        sqlite3_free(errMsg);
        throw SQLiteException(errorMsg);
    }
}

bool SQLiteURLShortener::shortCodeExists(const std::string& shortCode)
{
    const char* checkSQL = "SELECT COUNT(*) FROM url_mappings WHERE short_code = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, checkSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string errorMsg = "SQL prepare error: ";
        errorMsg += sqlite3_errmsg(db);
        throw SQLiteException(errorMsg);
    }

    sqlite3_bind_text(stmt, 1, shortCode.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
        std::string errorMsg = "SQL step error: ";
        errorMsg += sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw SQLiteException(errorMsg);
    }

    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    return count > 0;
}

std::string SQLiteURLShortener::shortenURL(const std::string& longURL)
{
    std::lock_guard<std::mutex> lock(dbMutex);

    // Check cache first
    auto cacheIt = longToShortCache.find(longURL);
    if (cacheIt != longToShortCache.end()) {
        return baseURL + cacheIt->second;
    }

    // Check if the long URL already has a short code
    const char* selectSQL = "SELECT short_code FROM url_mappings WHERE long_url = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string errorMsg = "SQL prepare error: ";
        errorMsg += sqlite3_errmsg(db);
        throw SQLiteException(errorMsg);
    }

    sqlite3_bind_text(stmt, 1, longURL.c_str(), -1, SQLITE_STATIC);

    std::string shortCode;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        shortCode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);

        // Update caches
        if (longToShortCache.size() >= cacheCapacity) {
            longToShortCache.clear();
        }
        if (shortToLongCache.size() >= cacheCapacity) {
            shortToLongCache.clear();
        }
        longToShortCache[longURL] = shortCode;
        shortToLongCache[shortCode] = longURL;

        return baseURL + shortCode;
    } else if (rc != SQLITE_DONE) {
        std::string errorMsg = "SQL step error: ";
        errorMsg += sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw SQLiteException(errorMsg);
    }
    sqlite3_finalize(stmt);

    // Generate a unique short code
    do {
        shortCode = codeGenerator.generateShortCode();
    } while (shortCodeExists(shortCode));

    // Insert the new mapping into the database
    const char* insertSQL = "INSERT INTO url_mappings (short_code, long_url) VALUES (?, ?);";
    rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string errorMsg = "SQL prepare error: ";
        errorMsg += sqlite3_errmsg(db);
        throw SQLiteException(errorMsg);
    }

    sqlite3_bind_text(stmt, 1, shortCode.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, longURL.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::string errorMsg = "SQL insert error: ";
        errorMsg += sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw SQLiteException(errorMsg);
    }

    sqlite3_finalize(stmt);

    // Update caches
    if (longToShortCache.size() >= cacheCapacity) {
        longToShortCache.clear();
    }
    if (shortToLongCache.size() >= cacheCapacity) {
        shortToLongCache.clear();
    }
    longToShortCache[longURL] = shortCode;
    shortToLongCache[shortCode] = longURL;

    return baseURL + shortCode;
}

std::string SQLiteURLShortener::getOriginalURL(const std::string& shortCode)
{
    std::lock_guard<std::mutex> lock(dbMutex);

    // Check cache first
    auto cacheIt = shortToLongCache.find(shortCode);
    if (cacheIt != shortToLongCache.end()) {
        return cacheIt->second;
    }

    const char* selectSQL = "SELECT long_url FROM url_mappings WHERE short_code = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string errorMsg = "SQL prepare error: ";
        errorMsg += sqlite3_errmsg(db);
        throw SQLiteException(errorMsg);
    }

    sqlite3_bind_text(stmt, 1, shortCode.c_str(), -1, SQLITE_STATIC);

    std::string longURL;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        longURL = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        // Update caches
        if (shortToLongCache.size() >= cacheCapacity) {
            shortToLongCache.clear();
        }
        if (longToShortCache.size() >= cacheCapacity) {
            longToShortCache.clear();
        }
        shortToLongCache[shortCode] = longURL;
        longToShortCache[longURL] = shortCode;
    } else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        // If no rows are returned, the short code does not exist
        throw SQLiteException("Short code not found: " + shortCode);
    } else {
        std::string errorMsg = "SQL step error: ";
        errorMsg += sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw SQLiteException(errorMsg);
    }

    sqlite3_finalize(stmt);
    return longURL;
}