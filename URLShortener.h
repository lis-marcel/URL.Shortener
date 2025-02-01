// URLShortener.h
#ifndef URLSHORTENER_H
#define URLSHORTENER_H

#include <string>
#include <unordered_map>
#include <sqlite3.h>

class URLShortener {
public:
    URLShortener();
    std::string shortenURL(const std::string& longURL);
    std::string getOriginalURL(const std::string& shortCode);

private:
    static const std::string Chars;
    std::string baseURL;
    std::unordered_map<std::string, std::string> urlMap;
    std::unordered_map<std::string, std::string> reverseUrlMap;

    std::string generateShortCode();
};

#endif // URLSHORTENER_H
