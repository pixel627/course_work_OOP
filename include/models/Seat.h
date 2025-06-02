#pragma once

#include <string>
#include <stdexcept>

class Seat {
    public:
        enum class Status { Free, Reserved, Occupied, Maintenance };
        enum class Type { Standard, VIP, Gaming, Conference };
    
        Seat(int id, Type type, Status status);
    
        bool set_status(Status new_status) noexcept;
        Status status() const noexcept;
        
        int id() const noexcept;
        Type type() const noexcept;
        const std::string& hardware_spec() const noexcept;
        void update_hardware(const std::string& new_spec);
    
    private:
        int id_;
        Type type_;
        std::string hardware_spec_;
        Status status_ = Status::Free;
    };