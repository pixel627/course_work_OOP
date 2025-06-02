#pragma once

#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <algorithm>

class Client {
public:
    Client(int id, std::string name, std::string contact);
    Client(int id, std::string name, std::string contact, time_t registered);
    
    void update_contact(const std::string& new_contact);
    void add_reservation(int reservation_id);
    
    int id() const noexcept;
    const std::string& name() const noexcept;
    const std::string& contact() const noexcept;
    time_t registered() const noexcept;
    const std::vector<int>& reservations() const noexcept;
    bool has_active_bookings() const noexcept;

private:
    int id_;
    std::string name_;
    std::string contact_;
    time_t registration_date_;
    std::vector<int> reservation_ids_;
    
    void validate_contact(const std::string& contact) const;
};