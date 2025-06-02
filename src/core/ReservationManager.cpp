#include "../../include/core/ReservationManager.h"

ReservationManager::ReservationManager(ClubSystem& clubSystem)
    : clubSystem_(clubSystem),   
      db_(DatabaseManager::instance()) {}

Reservation ReservationManager::create_reservation(int client_id, int seat_id, time_t start, time_t end) {
    TimeSlot slot{start, end};
    validate_time_slot(slot);
    
    if(!is_available(seat_id, slot)) {
        throw std::runtime_error("Место недоступно для бронирования");
    }

    const Seat& seat = clubSystem_.get_seat(seat_id);
    double price = calculate_price(seat, slot);

    db_.execute(
        "INSERT INTO reservations (client_id, seat_id, start_time, end_time, status, total_cost) "
        "VALUES (?, ?, ?, ?, ?, ?)",
        {
            std::to_string(client_id),
            std::to_string(seat_id),
            std::to_string(start),
            std::to_string(end),
            std::to_string(static_cast<int>(Reservation::Status::Pending)),
            std::to_string(price)
        }
    );

    clubSystem_.update_seat_status(seat_id, Seat::Status::Reserved);

    return load_reservation(get_last_insert_id());
}

double ReservationManager::calculate_price(const Seat& seat, const TimeSlot& slot) {
    double minutes = difftime(slot.end, slot.start) / 60;
    return minutes * 2.0; 
}

int ReservationManager::get_last_insert_id() const {
    auto result = db_.fetch_all("SELECT last_insert_rowid()");
    return std::stoi(result[0].columns[0]);
}

void ReservationManager::cancel_reservation(int reservation_id) {
    Reservation res = load_reservation(reservation_id);
    res.cancel();
    save_reservation(res);
    
    clubSystem_.update_seat_status(res.seat_id(), Seat::Status::Free);
}

std::vector<Reservation> ReservationManager::find_reservations(int client_id, int seat_id, 
                                                             Reservation::Status status) const {
    std::vector<std::string> conditions;
    std::vector<std::string> params;
    
    if(client_id != -1) {
        conditions.push_back("client_id = ?");
        params.push_back(std::to_string(client_id));
    }
    if(seat_id != -1) {
        conditions.push_back("seat_id = ?");
        params.push_back(std::to_string(seat_id));
    }
    if(status != Reservation::Status::ANY) {
        conditions.push_back("status = ?");
        params.push_back(std::to_string(static_cast<int>(status)));
    }

    std::string query = "SELECT id, client_id, seat_id, start_time, end_time, status, total_cost "
                       "FROM reservations";
    
    if(!conditions.empty()) {
        query += " WHERE " + join_strings(conditions, " AND ");
    }

    std::vector<Reservation> result;
    auto rows = db_.fetch_all(query, params);
    
    for(const auto& row : rows) {
        result.push_back(create_from_db_row(row.columns));
    }
    
    return result;
}

bool ReservationManager::is_available(int seat_id, const TimeSlot& slot) const {
    auto rows = db_.fetch_all(
        "SELECT COUNT(*) FROM reservations "
        "WHERE seat_id = ? "
        "AND ((start_time BETWEEN ? AND ?) OR (end_time BETWEEN ? AND ?)) "
        "AND status IN (0, 1)",
        {
            std::to_string(seat_id),
            std::to_string(slot.start),
            std::to_string(slot.end),
            std::to_string(slot.start),
            std::to_string(slot.end)
        }
    );

    return rows.empty() || std::stoi(rows[0].columns[0]) == 0;
}

Reservation ReservationManager::load_reservation(int id) const {
    auto rows = db_.fetch_all(
        "SELECT id, client_id, seat_id, start_time, end_time, status, total_cost "
        "FROM reservations WHERE id = ?",
        {std::to_string(id)}
    );

    if(rows.empty()) {
        throw std::runtime_error("Reservation not found");
    }
    
    return create_from_db_row(rows[0].columns);
}

void ReservationManager::save_reservation(const Reservation& r) {
    db_.execute(
        "UPDATE reservations SET "
        "client_id = ?, seat_id = ?, start_time = ?, end_time = ?, "
        "status = ?, total_cost = ? "
        "WHERE id = ?",
        {
            std::to_string(r.client_id()),
            std::to_string(r.seat_id()),
            std::to_string(r.start_time()),
            std::to_string(r.end_time()),
            std::to_string(static_cast<int>(r.status())),
            std::to_string(r.total_cost()),
            std::to_string(r.id())
        }
    );
}

Reservation ReservationManager::create_from_db_row(const std::vector<std::string>& row) {
    return Reservation(
        std::stoi(row[0]),
        std::stoi(row[1]),
        std::stoi(row[2]),
        std::stol(row[3]),
        std::stol(row[4]),
        static_cast<Reservation::Status>(std::stoi(row[5])),
        std::stod(row[6])
    );
}

std::string ReservationManager::join_strings(const std::vector<std::string>& vec, 
                                           const std::string& delimiter) const {
    std::string result;
    for(size_t i = 0; i < vec.size(); ++i) {
        if(i > 0) result += delimiter;
        result += vec[i];
    }
    return result;
}

void ReservationManager::validate_time_slot(const TimeSlot& slot) const {
    if(slot.start >= slot.end) {
        throw std::invalid_argument("End time must be after start time");
    }
    
    const time_t max_duration = 24 * 3600;
    if((slot.end - slot.start) > max_duration) {
        throw std::invalid_argument("Reservation duration exceeds 24 hours");
    }
}