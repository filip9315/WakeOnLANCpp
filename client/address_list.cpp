//
// Created by Filip Wolski on 17/05/2025.
//

#include "address_list.h"
#include <fstream>
#include <iostream>

address_list::address_list() {
    std::ifstream infile("addresses.txt");
    if (!infile.is_open()) {
        std::ofstream outfile("addresses.txt");
        outfile.close();
    }
}

void address_list::save_address(string address) {
    std::ofstream outfile("addresses.txt", std::ios::app);
    if (outfile.is_open()) {
        outfile << address << std::endl;
        outfile.close();
    }
}

void address_list::delete_address(string address) {
    std::ifstream infile("addresses.txt");
    std::vector<std::string> addresses;
    std::string line;
    while (std::getline(infile, line)) {
        if (line != address) {
            addresses.push_back(line);
        }
    }
    infile.close();
    std::ofstream outfile("addresses.txt", std::ios::trunc);
    for (const auto& addr : addresses) {
        outfile << addr << std::endl;
    }
    outfile.close();
}

std::vector<std::string> address_list::get_addresses() {
    std::vector<std::string> addresses;
    std::ifstream infile("addresses.txt");
    if (!infile.is_open()) {
        std::cerr << "Failed to open addresses.txt" << std::endl;
    }
    std::string line;
    while (std::getline(infile, line)) {
        addresses.push_back(line);
    }
    infile.close();
    return addresses;
}
