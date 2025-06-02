#pragma once

#include "ReservationManager.h"
#include "DatabaseManager.h"
#include "../models/Client.h"
#include "../models/Seat.h"
#include "../models/Tariff.h"
#include "../models/Product.h"
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <ctime>

class ReservationManager;

class ClubSystem {
public:
    ClubSystem();
    ~ClubSystem();
    
    void initialize(const std::string& db_path);
    void shutdown();

    Client create_client(std::string name, std::string contact);
    bool update_client(int id, const std::string& new_contact);

    void add_product(const Product& product);
    bool sell_product(int product_id, int quantity = 1);
    const std::unordered_map<int, Product>& products() const;

    ReservationManager& reservations();
    const std::vector<Seat>& seats() const noexcept;
    const Client& get_client(int client_id) const;
    const Seat& get_seat(int seat_id) const;
    std::vector<Product> get_products() const;
    std::vector<Client> find_clients(const std::string& query) const;
    void update_seat_status(int seat_id, Seat::Status new_status);
    void initialize_default_seats();
    void addSeat(const Seat& seat);

private:
    ReservationManager* reservation_manager_;
    std::vector<Seat> seats_;
    std::unordered_map<int, Client> clients_;
    std::unordered_map<int, Product> products_;

    void setup_database();
    void load_data();
    void save_data();
    void load_seats();
    void load_clients();
    void load_products();
};