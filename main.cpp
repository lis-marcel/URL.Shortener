#include "crow.h"
#include "SQLiteURLShortener.h"
#include "SQLiteException.h"
#include <string>

struct CORSMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // Handle OPTIONS preflight
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "*");
            res.add_header("Access-Control-Allow-Headers", "content-type"); // or "*" for testing

            res.code = 200;
            res.end();
            return;
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        if (!res.headers.count("Access-Control-Allow-Origin")) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "*");
            res.add_header("Access-Control-Allow-Headers", "content-type"); // or "*" for testing
        }
    }
};

int main()
{
    crow::App<CORSMiddleware> app;
    const std::string baseURL = "http://127.0.0.1:18080/";
    SQLiteURLShortener urlShortener(baseURL);

    // Endpoint to shorten a given URL
    CROW_ROUTE(app, "/shorten")
        .methods(crow::HTTPMethod::POST)([&urlShortener](const crow::request& req) {
        try {
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
        }
        catch (const SQLiteException& ex) {
            // Log the error message and return a 500 response
            std::cerr << "Error: " << ex.what() << std::endl;
            return crow::response(500, "Internal Server Error");
        }
    });

    // Endpoint to redirect to the original URL
    CROW_ROUTE(app, "/<string>")
        .methods(crow::HTTPMethod::GET)([&urlShortener](const crow::request& req, crow::response& res, std::string shortCode) {
        try {
            std::string originalURL = urlShortener.getOriginalURL(shortCode);
            res.redirect(originalURL);
            res.end();
        }
        catch (const SQLiteException& ex) {
            // Log the error message and return a 404 response
            std::cerr << "Error: " << ex.what() << std::endl;
            res.code = 404;
            res.end("URL not found");
        }
    });

    app.bindaddr("127.0.0.1").port(18080).run();
}