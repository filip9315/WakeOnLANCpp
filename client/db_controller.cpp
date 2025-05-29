#include "db_controller.h"

#include <string>
#include <iostream>
#include <bsoncxx/json.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/options/find.hpp>
#include "config.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

std::string db_controller::get_url() {
    try {
        mongocxx::instance instance;
        std::string mongo_url = MONGODB_URL;
        const mongocxx::uri uri(mongo_url);
        const mongocxx::client client(uri);
        const auto db = client["main"];
        auto collection = db["urls"];

        const auto filter = make_document(kvp("type", "url"));
        auto result = collection.find_one(filter.view());

        if (result) {
            auto view = result->view();
            auto elem = view["url"];
            if (elem) {
                return std::string(elem.get_string().value);
            }
        }

        std::cout << "Document or 'url' field not found" << std::endl;
        return "";
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return "";
    }
}