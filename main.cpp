// main.cpp
#include "crow.h"
#include "URLShortener.h"
#include "SQLiteURLShortener.h"
#include <string>

int main()
{
    crow::SimpleApp app;
    const std::string baseURL = "http://127.0.0.1:18080/";
    SQLiteURLShortener urlShortener(baseURL);

    // Endpoint to shorten a given URL
    CROW_ROUTE(app, "/shorten").methods(crow::HTTPMethod::Post)([&urlShortener](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("url")) {
            return crow::response(400, "Invalid or missing 'url' field");
        }

        std::string longURL = body["url"].s();
        std::string shortURL = urlShortener.shortenURL(longURL);

        if (shortURL.empty()) {
            return crow::response(500, "Internal Server Error");
        }

        crow::json::wvalue res;
        res["shortUrl"] = shortURL;

        return crow::response(res);
    });

    // Endpoint to redirect to the original URL
    CROW_ROUTE(app, "/<string>")([&urlShortener](const crow::request& req, crow::response& res, std::string shortCode) {
        std::string originalURL = urlShortener.getOriginalURL(shortCode);
        if (originalURL.empty()) {
            res.code = 404;
            res.end("URL not found");
        }
        else {
            res.redirect(originalURL);
			res.end();
        }
    });

    app.bindaddr("127.0.0.1").port(18080).run();
}
