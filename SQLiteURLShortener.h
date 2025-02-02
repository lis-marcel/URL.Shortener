// SQLiteURLShortener.h
#ifndef SQLITEURLSHORTENER_H
#define SQLITEURLSHORTENER_H

#include "IURLShortener.h"
#include <sqlite3.h>
#include <mutex>
#include <unordered_map>

class SQLiteURLShortener : public IURLShortener {
public:
    SQLiteURLShortener(const std::string& baseURL);
    ~SQLiteURLShortener();

    std::string shortenURL(const std::string& longURL) override;
    std::string getOriginalURL(const std::string& shortCode) override;

private:
    sqlite3* db;
    std::mutex dbMutex;

    // Caches
    std::unordered_map<std::string, std::string> longToShortCache;
    std::unordered_map<std::string, std::string> shortToLongCache;
    size_t cacheCapacity = 100; // Adjust the cache size as needed

    void initializeDatabase();
    bool shortCodeExists(const std::string& shortCode);
};

#endif // SQLITEURLSHORTENER_H
