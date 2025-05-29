//
// Created by Filip Wolski on 17/05/2025.
//

#ifndef ADDRESS_LIST_H
#define ADDRESS_LIST_H
#include <string>
#include <vector>

using namespace std;

class address_list {
    public:
    address_list();
    void save_address(string address);
    void delete_address(string address);
    vector<string> get_addresses();
};

#endif //ADDRESS_LIST_H
