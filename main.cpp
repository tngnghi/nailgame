#include <iostream>
#include <map>
#include <queue>

class Customer {
    std::string name;
    std::string service;
    int patience;
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

void assignJobs() {
  while (!queue.empty()) {
    for (auto& tech : technicians) {
      if (!tech.isBusy && tech.skills[queue.top().service] > 50) {
        tech.isBusy = true;
        queue.pop(); 
        break;
      }
    }
  }
}

int calculateSatisfaction(Customer c, Technician t) {
  return (t.skills[c.service] * 10) - c.waitTime; 
}
int money = 0;
map<string, bool> upgrades; // e.g., {"UV Lamp": false}

void buyUpgrade(string item) {
  if (money >= item.cost) {
    upgrades[item] = true;
    money -= item.cost;
  }
}

void exportToCSV() {
  ofstream file("results.csv");
  file << "Customer,Service,Satisfaction\n";
  for (auto& c : completedCustomers) {
    file << c.name << "," << c.service << "," << c.satisfaction << "\n";
  }
}/* customer is automatically booking or walk-in
when customer is not satisfy, their score are lower, at 0 they leave
similar to nail tech, (ex: no upgrade for too long, not enough customer
when aquaired certain amount of customer, ability to expand & upgrade)
*/

int main() {

    return 0;
}
