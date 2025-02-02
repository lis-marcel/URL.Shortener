// SQLiteException.h
#ifndef SQLITEEXCEPTION_H
#define SQLITEEXCEPTION_H

#include <stdexcept>
#include <string>

class SQLiteException : public std::runtime_error {
public:
    explicit SQLiteException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif // SQLITEEXCEPTION_H
