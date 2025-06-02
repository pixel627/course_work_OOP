#pragma once

#include "../models/Reservation.h"
#include "ClubSystem.h"
#include "../models/Client.h"
#include "../models/Seat.h"
#include "DatabaseManager.h"
#include <algorithm>
#include <vector>
#include <stdexcept>

class ClubSystem; 

class ReservationManager {
public:
    explicit ReservationManager(ClubSystem& clubSystem);
    
    Reservation create_reservation(int client_id, int seat_id,
                                  time_t start, time_t end);
    void cancel_reservation(int reservation_id);
    
    std::vector<Reservation> find_reservations(
        int client_id = -1, 
        int seat_id = -1,
        Reservation::Status status = Reservation::Status::ANY) const;

    struct TimeSlot { time_t start; time_t end; };
    bool is_available(int seat_id, const TimeSlot& slot) const;
    double calculate_price(const Seat& seat, const TimeSlot& slot);
    int get_last_insert_id() const;

private:
    ClubSystem& clubSystem_;
    DatabaseManager& db_;
    
    Reservation load_reservation(int id) const;
    void save_reservation(const Reservation& r);
    void validate_time_slot(const TimeSlot& slot) const;
    
    static Reservation create_from_db_row(const std::vector<std::string>& row);
    static std::string build_where_clause(const std::vector<std::string>& conditions);
    std::string join_strings(const std::vector<std::string>& vec, 
        const std::string& delimiter) const;
};