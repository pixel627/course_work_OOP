#include "../../include/models/Client.h"

Client::Client(int id, std::string name, std::string contact)
    : id_(id), 
      name_(std::move(name)),
      contact_(std::move(contact)),
      registration_date_(time(nullptr)) 
{
    validate_contact(contact_);
}

Client::Client(int id, std::string name, std::string contact, time_t registered)
    : id_(id), 
      name_(std::move(name)),
      contact_(std::move(contact)),
      registration_date_(registered) 
{
    validate_contact(contact_);
}

void Client::update_contact(const std::string& new_contact) {
    validate_contact(new_contact);
    contact_ = new_contact;
}

void Client::add_reservation(int reservation_id) {
    if(std::find(reservation_ids_.begin(), reservation_ids_.end(), reservation_id) == reservation_ids_.end()) {
        reservation_ids_.push_back(reservation_id);
    }
}

int Client::id() const noexcept {
    return id_;
}

const std::string& Client::name() const noexcept {
    return name_;
}

const std::string& Client::contact() const noexcept {
    return contact_;
}

time_t Client::registered() const noexcept {
    return registration_date_;
}

const std::vector<int>& Client::reservations() const noexcept {
    return reservation_ids_;
}

bool Client::has_active_bookings() const noexcept {
    return !reservation_ids_.empty();
}

void Client::validate_contact(const std::string& contact) const {
    static const std::regex phone_regex(R"(^\+7\d{10}$)");
    static const std::regex email_regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    
    if(contact.empty()) {
        throw std::invalid_argument("Контакт не может быть пустым");
    }
    
    if(!std::regex_match(contact, phone_regex) && 
       !std::regex_match(contact, email_regex)) {
        throw std::invalid_argument(
            "Неверный формат контакта. Используйте:\n"
            "- Телефон: +7XXXXXXXXXX (11 цифр)\n"
            "- Email: example@mail.com"
        );
    }
}