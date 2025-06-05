#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <algorithm>

int day = 1, money = 100;
int currentMinute = 0;
const int DAY_END_MINUTE = 600; // 10 hours, e.g. 10*60 = 600 minutes

// Service types and base times (in minutes)
std::map<std::string, int> baseServiceTime = {
    {"Manicure", 25},
    {"Pedicure", 30},
    {"Gel Mani", 45},
    {"GelX", 50},
    {"Dip", 50},
    {"Acrylic", 60},
    {"Hard Gel", 60},
    {"Fill", 60},
    {"Design", 15}
};

int randBetween(int a, int b) {
    return a + rand() % (b - a + 1);
}

struct Technician {
    std::string name;
    int skill;
    int service;
    int speed;
    bool isBusy = false;
    int busyUntil = 0;
    std::string currentJob;
    Technician(std::string n) : name(n) {
        skill = randBetween(50, 100);
        service = randBetween(40, 100);
        speed = randBetween(30, 100);
    }
};

struct Customer {
    std::string name;
    std::string serviceRequested;
    int designLevel;
    int patience;
    int minSkill;
    int minService;
    int minSpeed;
    int satisfaction = 100;
    int waitTime = 0;
    int visit = 0;
    bool left = false;
    Customer(std::string n, std::string s, int lvl = 0)
        : name(n), serviceRequested(s), designLevel(lvl) {
        patience = randBetween(5, 60);
        minSkill = randBetween(40, 90);
        minService = randBetween(30, 80);
        minSpeed = randBetween(30, 80);
    }
};

std::vector<Customer> waitingCustomers;
std::vector<Customer> completedCustomers;
std::vector<Technician> technicians;

void printBoard() {
    std::cout << "\n=== Salon Board (Day " << day << ", Minute " << currentMinute << ") ===\n";
    std::cout << "\nTechnicians:\n";
    std::cout << "Name         Skill Service Speed  Status\n";
    for (const auto& t : technicians) {
        std::cout << std::left << std::setw(13) << t.name
                  << std::setw(6) << t.skill
                  << std::setw(8) << t.service
                  << std::setw(6) << t.speed;
        if (t.isBusy)
            std::cout << "Busy (" << t.currentJob << ", free at min " << t.busyUntil << ")\n";
        else
            std::cout << "Free\n";
    }
    std::cout << "\nCustomers Waiting:\n";
    std::cout << "Idx Name     ReqService Level MinSkill MinCustServ MinSpeed Patience Wait\n";
    for (size_t i = 0; i < waitingCustomers.size(); ++i) {
        const auto& c = waitingCustomers[i];
        std::cout << std::setw(3) << i
                  << std::left << std::setw(9) << c.name
                  << std::setw(11) << c.serviceRequested
                  << std::setw(6) << c.designLevel
                  << std::setw(9) << c.minSkill
                  << std::setw(12) << c.minService
                  << std::setw(9) << c.minSpeed
                  << std::setw(13) << c.patience
                  << c.waitTime << "\n";
    }
    std::cout << "============================\n";
}

// Random name generators
std::string randCustomerName() {
    std::string names[] = {"Anna", "Ben", "Cindy", "David", "Ella", "Hoa", "Jade", "Lee"};
    return names[randBetween(0, 7)];
}
std::string randTechName() {
    std::string names[] = {"Linh", "Tam", "Quynh", "Bao", "Kiet", "Kim"};
    return names[randBetween(0, 5)];
}
std::string randService() {
    std::string services[] = {"Manicure", "Pedicure", "Gel Mani", "GelX", "Dip", "Acrylic", "Hard Gel", "Fill", "Design"};
    return services[randBetween(0, 8)];
}

void addRandomCustomer() {
    std::string s = randService();
    int level = 0;
    if (s == "Design" || s == "Acrylic" || s == "Hard Gel" || s == "GelX") {
        level = randBetween(1, 3);
    }
    waitingCustomers.emplace_back(randCustomerName(), s, level);
    std::cout << "New customer arrived!\n";
}
void addRandomTechnician() {
    technicians.emplace_back(randTechName());
    std::cout << "New technician joined the salon!\n";
}

int estimateTime(const Customer& cust, const Technician& tech) {
    int qualified = 25;
    auto it = baseServiceTime.find(cust.serviceRequested);
    if (it != baseServiceTime.end()) qualified = it->second;
    int time = std::ceil(qualified * 100.0 / tech.speed);
    if (cust.designLevel > 0) time += 15 * cust.designLevel;
    return time;
}
int moneyEarned(const Customer& cust, int actualTime) {
    int baseMoney = actualTime / 2;
    if (cust.serviceRequested == "GelX") baseMoney += 10;
    return baseMoney;
}

// Find most suitable technician if no one fully qualifies
int technicianSuitability(const Technician& tech, const Customer& cust) {
    // Lower score is better (closer to requirements)
    int skillGap = std::max(0, cust.minSkill - tech.skill);
    int serviceGap = std::max(0, cust.minService - tech.service);
    int speedGap = std::max(0, cust.minSpeed - tech.speed);
    return skillGap + serviceGap + speedGap;
}

void assignJobs() {
    for (auto& tech : technicians) {
        if (tech.isBusy && tech.busyUntil > currentMinute) continue;
        if (tech.isBusy && tech.busyUntil <= currentMinute) {
            tech.isBusy = false;
            tech.currentJob = "";
        }
        if (tech.isBusy) continue;
        for (auto it = waitingCustomers.begin(); it != waitingCustomers.end();) {
            // Check if fully qualified
            if (tech.skill >= it->minSkill && tech.service >= it->minService && tech.speed >= it->minSpeed) {
                int tmin = estimateTime(*it, tech);
                tech.isBusy = true;
                tech.busyUntil = currentMinute + tmin;
                tech.currentJob = it->serviceRequested + "_" + it->name; // Unique
                int rating = 5;
                if (tech.skill < it->minSkill + 10) rating--;
                if (tech.service < it->minService + 10) rating--;
                if (tech.speed < it->minSpeed + 10) rating--;
                it->satisfaction = rating * 20;
                std::cout << "Assigned " << it->serviceRequested << " (level " << it->designLevel << ") for " << it->name
                          << " to " << tech.name << " (Rating: " << rating << "★, est. " << tmin << " min)\n";
                // After service, tech skill up, requirements down
                tech.skill = std::min(100, tech.skill + 2);
                tech.service = std::min(100, tech.service + 1);
                tech.speed = std::min(100, tech.speed + 1);
                for (auto& c : waitingCustomers)
                {
                    c.minSkill = std::max(20, c.minSkill - 1);
                    c.minService = std::max(10, c.minService - 1);
                    c.minSpeed = std::max(10, c.minSpeed - 1);
                }
                completedCustomers.push_back(*it);
                it = waitingCustomers.erase(it);
                break;
            }
            ++it;
        }
    }
    // If any customer remains with no qualified tech, offer best match or refusal
    for (size_t i = 0; i < waitingCustomers.size(); ++i) {
        Customer& cust = waitingCustomers[i];
        int bestIdx = -1, bestScore = 100000;
        for (size_t t = 0; t < technicians.size(); ++t) {
            Technician& tech = technicians[t];
            if (tech.isBusy && tech.busyUntil > currentMinute) continue;
            int score = technicianSuitability(tech, cust);
            if (score < bestScore) {
                bestScore = score;
                bestIdx = t;
            }
        }
        if (bestIdx != -1 && bestScore < 99999) {
            // Offer partial match
            Technician& tech = technicians[bestIdx];
            std::cout << "Customer " << cust.name << " cannot be fully satisfied, but "
                      << tech.name << " is the closest. Assign? (y/n): ";
            std::string ans;
            std::getline(std::cin, ans);
            if (ans == "y" || ans == "Y") {
                int tmin = estimateTime(cust, tech);
                tech.isBusy = true;
                tech.busyUntil = currentMinute + tmin;
                tech.currentJob = cust.serviceRequested + "_" + cust.name;
                // Lower rating due to gaps
                int rating = 5 - (bestScore / 10);
                if (rating < 1) rating = 1;
                cust.satisfaction = rating * 20;
                std::cout << "Assigned (not ideal) " << cust.serviceRequested << " (level " << cust.designLevel
                          << ") for " << cust.name << " to " << tech.name << " (Rating: " << rating
                          << "★, est. " << tmin << " min)\n";
                // Up skill and down requirements
                tech.skill = std::min(100, tech.skill + 2);
                tech.service = std::min(100, tech.service + 1);
                tech.speed = std::min(100, tech.speed + 1);
                for (auto& c : waitingCustomers)
                {
                    c.minSkill = std::max(20, c.minSkill - 1);
                    c.minService = std::max(10, c.minService - 1);
                    c.minSpeed = std::max(10, c.minSpeed - 1);
                }
                completedCustomers.push_back(cust);
                waitingCustomers.erase(waitingCustomers.begin() + i);
                --i;
            }
            // else leave customer in queue for possible refusal
        }
    }
}

void refuseService() {
    if (waitingCustomers.empty()) {
        std::cout << "No customers to refuse.\n";
        return;
    }
    printBoard();
    std::cout << "Enter index of customer to refuse: ";
    std::string input;
    std::getline(std::cin, input);
    int idx = std::stoi(input);
    if (idx >= 0 && idx < (int)waitingCustomers.size()) {
        std::cout << "Refused service to " << waitingCustomers[idx].name << ".\n";
        waitingCustomers.erase(waitingCustomers.begin() + idx);
    } else {
        std::cout << "Invalid index.\n";
    }
}

void advanceTime(int minutes) {
    for (int m = 0; m < minutes; ++m) {
        currentMinute++;
        // Free up any techs who finish at this minute, pay
        for (auto& tech : technicians) {
            if (tech.isBusy && tech.busyUntil == currentMinute) {
                tech.isBusy = false;
                // Find customer for money
                for (auto it = completedCustomers.begin(); it != completedCustomers.end(); ++it) {
                    std::string jobTag = tech.currentJob;
                    std::string expectedTag = it->serviceRequested + "_" + it->name;
                    if (jobTag == expectedTag) {
                        int actualTime = estimateTime(*it, tech);
                        int earned = moneyEarned(*it, actualTime);
                        money += earned;
                        std::cout << tech.name << " finished " << it->serviceRequested << " for " << it->name
                                  << ". Earned $" << earned << ".\n";
                        tech.currentJob = "";
                        break;
                    }
                }
            }
        }
        // Update waiting customers
        for (auto it = waitingCustomers.begin(); it != waitingCustomers.end();) {
            it->patience--;
            it->waitTime++;
            if (it->patience <= 0) {
                std::cout << it->name << " left due to impatience.\n";
                it = waitingCustomers.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void endOfDay() {
    // Find the latest busyUntil among all busy techs
    int maxBusy = currentMinute;
    for (const auto& tech : technicians) {
        if (tech.isBusy && tech.busyUntil > maxBusy) {
            maxBusy = tech.busyUntil;
        }
    }
    if (maxBusy > currentMinute) {
        std::cout << "[Day end] Waiting for all technicians to finish their work...\n";
        advanceTime(maxBusy - currentMinute); // Advance time so all jobs finish
    }

    std::cout << "\n--- End of Day " << day << " ---\n";
    std::cout << "Completed Customers:\n";
    for (const auto& c : completedCustomers)
        std::cout << c.name << ": " << c.satisfaction << "/100 satisfaction\n";
    completedCustomers.clear();
    day++;
    currentMinute = 0;
    // Chance for new staff
    if (randBetween(1, 10) > 8) addRandomTechnician();
}

int main() {
    srand(time(0));
    technicians.emplace_back("Linh");
    technicians.back().skill = 80; technicians.back().service = 85; technicians.back().speed = 70;
    technicians.emplace_back("Tam");
    std::string cmd;
    while (true) {
        printBoard();
        std::cout << "Money: $" << money << ". Options: [1] New Customer  [2] Assign Jobs  [3] Skip time  [4] End Day  [5] Refuse Customer [0] Quit\n";
        std::cout << "Choice: ";
        std::getline(std::cin, cmd);
        if (cmd == "1") addRandomCustomer();
        else if (cmd == "2") assignJobs();
        else if (cmd == "3") {
            std::cout << "Enter minutes to skip: ";
            std::string min;
            std::getline(std::cin, min);
            int skip = std::stoi(min);
            // Prevent skipping past closing time
            if (currentMinute + skip > DAY_END_MINUTE) skip = DAY_END_MINUTE - currentMinute;
            if (skip > 0) advanceTime(skip);
            else std::cout << "Cannot skip beyond closing time.\n";
        }
        else if (cmd == "4") {
            // Advance to end of day (but not past, so add only up to DAY_END_MINUTE)
            if (currentMinute < DAY_END_MINUTE) advanceTime(DAY_END_MINUTE - currentMinute);
            endOfDay();
        }
        else if (cmd == "5") refuseService();
        else if (cmd == "0") break;
        else std::cout << "Invalid input.\n";
        // Automatic day end if time passes closing
        if (currentMinute >= DAY_END_MINUTE) {
            endOfDay();
        }
    }
    return 0;
}
