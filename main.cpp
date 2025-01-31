// main.cpp
#include "crow.h"
#include "URLShortener.h"
#include <string>
#include <cstdlib> // For std::rand()

int main()
{
    crow::SimpleApp app;
    URLShortener urlShortener;

    // Endpoint to return a random shortened URL
    CROW_ROUTE(app, "/shorten")([&urlShortener]() {
        // Generate a random long URL
        std::string longURL = "https://example.com/random/" + std::to_string(std::rand());

        // Shorten the URL
        std::string shortURL = urlShortener.shortenURL(longURL);

        // Return the shortened URL as a response
        return shortURL;
    });

    app.bindaddr("127.0.0.1").port(18080).run();
}
