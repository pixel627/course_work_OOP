#include "../../include/models/Reservation.h"

Reservation::Reservation(int id, 
                        int client_id, 
                        int seat_id,
                        time_t start_time,
                        time_t end_time,
                        Status status,
                        double total_cost)
    : id_(id),
      client_id_(client_id),
      seat_id_(seat_id),
      start_time_(start_time),
      end_time_(end_time),
      status_(status),
      total_cost_(total_cost) 
{
    if(start_time_ >= end_time_) {
        throw std::invalid_argument("End time must be after start time");
    }
    
    if(total_cost_ < 0) {
        throw std::invalid_argument("Total cost can't be negative");
    }
}

void Reservation::cancel() noexcept {
    status_ = Status::Cancelled;
}

void Reservation::activate() noexcept {
    status_ = Status::Active;
}

void Reservation::complete() noexcept {
    status_ = Status::Completed;
}

int Reservation::id() const noexcept {
    return id_;
}

int Reservation::client_id() const noexcept {
    return client_id_;
}

int Reservation::seat_id() const noexcept {
    return seat_id_;
}

time_t Reservation::start_time() const noexcept {
    return start_time_;
}

time_t Reservation::end_time() const noexcept {
    return end_time_;
}

Reservation::Status Reservation::status() const noexcept {
    return status_;
}

double Reservation::total_cost() const noexcept {
    return total_cost_;
}

void Reservation::set_total_cost(double cost) noexcept {
    if(cost >= 0) {
        total_cost_ = cost;
    }
}
