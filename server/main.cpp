#include <iostream>
#include "db_controller.h"
#include "ngrok_controller.h"
#include <crow/app.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>
#include <array>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


bool send_wol_packet(const std::vector<uint8_t>& mac) {
    if (mac.size() != 6) return false;

    std::array<uint8_t, 102> packet;
    std::fill(packet.begin(), packet.begin() + 6, 0xFF);
    for (int i = 0; i < 16; ++i) {
        std::copy(mac.begin(), mac.end(), packet.begin() + 6 + i * 6);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) return false;

    int broadcast = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9);
    inet_pton(AF_INET, "255.255.255.255", &addr.sin_addr);

    bool success = sendto(sock, packet.data(), packet.size(), 0, (sockaddr*)&addr, sizeof(addr)) >= 0;
    close(sock);
    return success;
}

std::vector<uint8_t> parse_mac(const std::string& mac_str) {
    std::vector<uint8_t> mac;
    std::istringstream ss(mac_str);
    std::string byte_str;
    while (std::getline(ss, byte_str, ':')) {
        mac.push_back(std::stoi(byte_str, nullptr, 16));
    }
    return mac;
}


int main() {

    ngrok_controller controller;
    const string url = controller.setup();

    db_controller db_controller;
    cout << "Public URL: " << url << endl;
    db_controller.set_url(url);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/wake").methods("POST"_method)([](const crow::request& req) {
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string mac_str = body.at("mac").get<std::string>();
            cout << "mac: " << mac_str << endl;
            auto mac = parse_mac(mac_str);

            if (send_wol_packet(mac)) {
                return crow::response(200, "Magic packet sent");
            } else {
                return crow::response(500, "Failed to send magic packet");
            }
        } catch (...) {
            return crow::response(400, "Invalid request");
        }
    });

    app.port(8080).run();


    return 0;
}
