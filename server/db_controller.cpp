#include <string>
#include "db_controller.h"
#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "config.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void db_controller::set_url(std::string url) {
    try
    {
        mongocxx::instance instance;
        std::string mongo_url = MONGODB_URL;
        const mongocxx::uri uri(mongo_url);
        const mongocxx::client client(uri);
        const auto db = client["main"];
        auto collection = db["urls"];

        const auto query_filter = make_document(kvp("type", "url"));
        const auto update_doc = make_document(kvp("$set", make_document(kvp("url", url))));
        auto result = collection.update_one(query_filter.view(), update_doc.view());

        if (result) {
            std::cout << "Updated URL in the database" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        // Handle errors
        std::cout<< "Exception: " << e.what() << std::endl;
    }
}

