// URLShortener.cpp
#include "URLShortener.h"
#include <sstream>
#include <random>
#include <mutex>

const std::string URLShortener::Chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

URLShortener::URLShortener() : baseURL("http://127.0.0.1:18080/") {}

std::string URLShortener::shortenURL(const std::string& longURL) {
    static std::mutex mapMutex;
    std::lock_guard<std::mutex> lock(mapMutex);

    if (reverseUrlMap.find(longURL) != reverseUrlMap.end()) {
        return reverseUrlMap[longURL];
    }
    std::string shortURL = generateShortURL();
    urlMap[shortURL] = longURL;
    reverseUrlMap[longURL] = shortURL;
    return shortURL;
}

std::string URLShortener::getOriginalURL(const std::string& shortURL) {
    static std::mutex mapMutex;
    std::lock_guard<std::mutex> lock(mapMutex);

    auto it = urlMap.find(shortURL);
    if (it != urlMap.end()) {
        return it->second;
    }
    return "";
}

std::string URLShortener::generateShortURL() {
    std::string shortURL;
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, Chars.size() - 1);

    for (int i = 0; i < 7; ++i) {
        shortURL += Chars[distribution(generator)];
    }

    return shortURL;
}
