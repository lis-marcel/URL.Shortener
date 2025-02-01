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

    // Check if the long URL has already been shortened
    if (reverseUrlMap.find(longURL) != reverseUrlMap.end()) {
        return baseURL + reverseUrlMap[longURL];
    }

    // Generate a unique short code
    std::string shortCode;
    do {
        shortCode = generateShortCode();
    } while (urlMap.find(shortCode) != urlMap.end());

    // Store the mapping
    urlMap[shortCode] = longURL;
    reverseUrlMap[longURL] = shortCode;

    // Return the full short URL
    return baseURL + shortCode;
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

std::string URLShortener::generateShortCode() {
    std::string shortURL;
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, Chars.size() - 1);

    for (int i = 0; i < 7; ++i) {
        shortURL += Chars[distribution(generator)];
    }

    return shortURL;
}
