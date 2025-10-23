#include <iostream>
#include "db_controller.h"
#include <curl/curl.h>
#include "address_list.h"


void send_req(string url, string mac) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        std::string json_body = "{\"mac\": \"" + mac + "\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/json"));

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
}

void wake(string url, vector<string> addresses) {
    int pc_num;
    cout << "Choose PC to wake:" << endl;
    for (int i = 0; i < addresses.size(); i++) {
        cout << i+1 << ". " << addresses[i] << endl;
    }
    cin >> pc_num;
    send_req(url, addresses[pc_num-1]);
}

int main(int argc, char *argv[]) {
    db_controller controller;
    address_list address_list;
    int option;
    int pc_num;
    string address;
    const string url = controller.get_url() + "/wake";
    vector<string> addresses;

    if (argc == 2) {
        address = argv[1];
        send_req(url, address);
        return 0;
    }

    cout << "Welcome to Wake on LAN (C++)!" << endl;
    cout << "What do you want to do?" << endl;
    cout << "   1. Wake a PC" << endl;
    cout << "   2. Add a PC to the list" << endl;
    cout << "   3. Remove a PC from the list" << endl;
    cout << "   4. Exit" << endl;
    cin >> option;

    while (option != 4) {

        switch (option) {
            case 1:
                addresses = address_list.get_addresses();
                wake(url, addresses);
                break;
            case 2:
                cout << "Enter MAC address of the PC:" << endl;
                cin >> address;
                address_list.save_address(address);
                break;
            case 3:
                addresses = address_list.get_addresses();
                cout << "Enter number of the PC:" << endl;
                for (int i = 0; i < addresses.size(); i++) {
                    cout << i+1 << ". " << addresses[i] << endl;
                }
                cin >> pc_num;
                address_list.delete_address(addresses[pc_num-1]);
                break;
            case 4:
                return 0;
            default:
                cout << "Invalid option!" << endl;
                break;
        }
        cout << endl << endl << "What do you want to do next?" << endl;
        cout << "   1. Wake a PC" << endl;
        cout << "   2. Add a PC to the list" << endl;
        cout << "   3. Remove a PC from the list" << endl;
        cout << "   4. Exit" << endl;
        cin >> option;
    }


    return 0;
}
