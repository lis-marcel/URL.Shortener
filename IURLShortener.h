#ifndef IURLSHORTENER_H
#define IURLSHORTENER_H

#include <string>
#include "CodeGenerator.h"

class IURLShortener {
public:
    virtual ~IURLShortener() {}

    // Shortens the given long URL and returns the short URL
    virtual std::string shortenURL(const std::string& longURL) = 0;

    // Retrieves the original long URL from the given short code
    virtual std::string getOriginalURL(const std::string& shortCode) = 0;

protected:
    CodeGenerator codeGenerator;
    std::string baseURL;

    IURLShortener(const std::string& baseURL);
};

#endif // IURLSHORTENER_H
