#include "../../include/models/Seat.h"

Seat::Seat(int id, Type type, Status status)
    : id_(id), type_(type), status_(status) {}

bool Seat::set_status(Status new_status) noexcept {
    if(status_ != new_status) {
        status_ = new_status;
        return true;
    }
    return false;
}

Seat::Status Seat::status() const noexcept {
    return status_;
}

int Seat::id() const noexcept { 
    return id_; 
}

Seat::Type Seat::type() const noexcept { 
    return type_; 
}

const std::string& Seat::hardware_spec() const noexcept {
    return hardware_spec_;
}

void Seat::update_hardware(const std::string& new_spec) {
    if(!new_spec.empty()) {
        hardware_spec_ = new_spec;
    }
}
