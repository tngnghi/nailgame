#include <iostream>
#include <map>
#include <queue>

class Customer {
    std::string name;
    std::string service;
    int patience;
    bool requiresSpecificTech;
    bool appointment;
    std::map<std::string, int> specificTechs;
};
class NailCondition {
    int quality;
    int healthy;
    int treatment;
};
class Technician {
    std::string name;
    std::map<std::string, int> skills;
    bool isBusy;
};

std::priority_queue<Customer> queue;
void addCustomer(Customer c) {
    queue.push(c);

    for(auto& cust : queue) {
        cust.patience--;
        if(cust.patience == 0) queue.pop;
    }
}

int main() {

    return 0;
}
