// SQLiteURLShortener.h
#ifndef SQLITEURLSHORTENER_H
#define SQLITEURLSHORTENER_H

#include "IURLShortener.h"
#include <sqlite3.h>
#include <mutex>

class SQLiteURLShortener : public IURLShortener {
public:
    SQLiteURLShortener(const std::string& baseURL);
    virtual ~SQLiteURLShortener();

    std::string shortenURL(const std::string& longURL) override;
    std::string getOriginalURL(const std::string& shortCode) override;

private:
    sqlite3* db;
    std::mutex dbMutex;

    void initializeDatabase();
    bool shortCodeExists(const std::string& shortCode);
};

#endif // SQLITEURLSHORTENER_H
