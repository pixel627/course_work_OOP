#include "../../include/core/ClubSystem.h"

namespace fs = std::filesystem;

ClubSystem::ClubSystem() 
    : reservation_manager_(new ReservationManager(*this)) {}

ClubSystem::~ClubSystem() {
    delete reservation_manager_;
}

void ClubSystem::initialize(const std::string& db_path) {
    try {
        fs::create_directories(fs::path(db_path).parent_path());
        auto& db = DatabaseManager::instance();
        db.connect(db_path);
        setup_database();
        load_data();
    } catch(const std::exception& e) {
        throw std::runtime_error("Initialization failed: " + std::string(e.what()));
    }
}


void ClubSystem::setup_database() {
    auto& db = DatabaseManager::instance();
    const std::vector<std::string> tables = {
        R"(CREATE TABLE IF NOT EXISTS clients (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            contact TEXT UNIQUE NOT NULL,
            reg_date INTEGER NOT NULL))",
        
        R"(CREATE TABLE IF NOT EXISTS seats (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type INTEGER NOT NULL,
            status INTEGER NOT NULL,
            hardware_spec TEXT))",
        
        R"(CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            category INTEGER NOT NULL,
            price REAL NOT NULL,
            stock INTEGER DEFAULT 0))",
        
        R"(CREATE TABLE IF NOT EXISTS reservations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            client_id INTEGER NOT NULL,
            seat_id INTEGER NOT NULL,
            start_time INTEGER NOT NULL,
            end_time INTEGER NOT NULL,
            status INTEGER NOT NULL,
            total_cost REAL NOT NULL,
            FOREIGN KEY(client_id) REFERENCES clients(id),
            FOREIGN KEY(seat_id) REFERENCES seats(id)))"
    };

    for(const auto& sql : tables) {
        db.execute(sql);
    } 

    initialize_default_seats();
}

void ClubSystem::initialize_default_seats() {
    auto result = DatabaseManager::instance().fetch_all(
        "SELECT COUNT(*) FROM seats"
    );
    
    if(std::stoi(result[0].columns[0]) > 0) return;

    DatabaseManager::instance().begin_transaction();
    try {
        for(int i = 1; i <= 60; ++i) {
            DatabaseManager::instance().execute(
                "INSERT INTO seats (type, status, hardware_spec) "
                "VALUES (?, ?, ?)",
                {
                    std::to_string(static_cast<int>(Seat::Type::Standard)),
                    std::to_string(static_cast<int>(Seat::Status::Free)),
                    "CPU: Intel i5, RAM: 16GB, GPU: NVIDIA GTX 1660"
                }
            );
        }
        DatabaseManager::instance().commit_transaction();
    } catch(...) {
        DatabaseManager::instance().rollback_transaction();
        throw;
    }
    
    load_seats();
}

void ClubSystem::addSeat(const Seat& seat) {
    auto& db = DatabaseManager::instance();
    
    db.execute(
        "INSERT INTO seats (type, status, hardware_spec) VALUES (?, ?, ?)",
        {
            std::to_string(static_cast<int>(seat.type())),
            std::to_string(static_cast<int>(seat.status())),
            seat.hardware_spec()
        }
    );
    
    load_seats(); 
}

void ClubSystem::load_data() {
    try {
        load_seats();
        load_clients();
        load_products();
    } catch(const std::exception& e) {
        throw std::runtime_error("Data loading failed: " + std::string(e.what()));
    }
}

void ClubSystem::load_seats() {
    seats_.clear();
    auto rows = DatabaseManager::instance().fetch_all(
        "SELECT id, type, status, hardware_spec FROM seats"
    );
    
    for(const auto& row : rows) {
        Seat seat(
            std::stoi(row.columns[0]),
            static_cast<Seat::Type>(std::stoi(row.columns[1])),
            static_cast<Seat::Status>(std::stoi(row.columns[2]))
        );
        seat.update_hardware(row.columns[3]);
        seats_.push_back(std::move(seat));
    }
}

void ClubSystem::load_clients() {
    clients_.clear();
    auto rows = DatabaseManager::instance().fetch_all(
        "SELECT id, name, contact, reg_date FROM clients"
    );
    
    for(const auto& row : rows) {
        clients_.emplace(
            std::stoi(row.columns[0]),
            Client(
                std::stoi(row.columns[0]),
                row.columns[1],
                row.columns[2],
                std::stol(row.columns[3])
            )
        );
    }
}

void ClubSystem::load_products() {
    products_.clear();
    auto rows = DatabaseManager::instance().fetch_all(
        "SELECT id, name, category, price, stock FROM products"
    );
    
    for(const auto& row : rows) {
        products_.emplace(
            std::stoi(row.columns[0]),
            Product(
                std::stoi(row.columns[0]),
                row.columns[1],
                static_cast<Product::Category>(std::stoi(row.columns[2])),
                std::stod(row.columns[3]),
                std::stoi(row.columns[4])
            )
        );
    }
}

Client ClubSystem::create_client(std::string name, std::string contact) {
    auto& db = DatabaseManager::instance();
    const time_t reg_date = time(nullptr);
    
    db.execute(
        "INSERT INTO clients (name, contact, reg_date) VALUES (?, ?, ?)",
        {name, contact, std::to_string(reg_date)}
    );
    
    auto result = db.fetch_all("SELECT last_insert_rowid()");
    if(result.empty() || result[0].columns.empty()) {
        throw std::runtime_error("Failed to retrieve client ID");
    }
    
    const int id = std::stoi(result[0].columns[0]);
    Client client(id, std::move(name), std::move(contact), reg_date);
    clients_.emplace(id, client);
    return client;
}

bool ClubSystem::sell_product(int product_id, int quantity) {
    auto it = products_.find(product_id);
    if(it == products_.end() || !it->second.sell(quantity)) return false;

    try {
        DatabaseManager::instance().execute(
            "UPDATE products SET stock = ? WHERE id = ?",
            {std::to_string(it->second.stock()), std::to_string(product_id)}
        );
        return true;
    } catch(...) {
        it->second.restock(quantity);
        return false;
    }
}

bool ClubSystem::update_client(int client_id, const std::string& new_contact) {
    auto it = clients_.find(client_id);
    if(it == clients_.end()) return false;

    try {
        DatabaseManager::instance().execute(
            "UPDATE clients SET contact = ? WHERE id = ?",
            {new_contact, std::to_string(client_id)}
        );
        it->second.update_contact(new_contact);
        return true;
    } catch(...) {
        return false;
    }
}

void ClubSystem::save_data() {
    auto& db = DatabaseManager::instance();
    try {
        db.begin_transaction();
        
        for(const auto& [id, client] : clients_) {
            db.execute(
                "UPDATE clients SET name = ?, contact = ? WHERE id = ?",
                {client.name(), client.contact(), std::to_string(id)}
            );
        }
        
        for(const auto& [id, product] : products_) {
            db.execute(
                "UPDATE products SET stock = ? WHERE id = ?",
                {std::to_string(product.stock()), std::to_string(id)}
            );
        }
        
        db.commit_transaction();
    } catch(...) {
        db.rollback_transaction();
        throw;
    }
}

void ClubSystem::shutdown() {
    save_data();
    DatabaseManager::instance().disconnect();
}

ReservationManager& ClubSystem::reservations() { 
    return *reservation_manager_; 
}

const std::vector<Seat>& ClubSystem::seats() const noexcept {
    return seats_;
}

const Client& ClubSystem::get_client(int client_id) const {
    auto it = clients_.find(client_id);
    if(it == clients_.end()) throw std::runtime_error("Client not found");
    return it->second;
}

const Seat& ClubSystem::get_seat(int seat_id) const {
    auto it = std::find_if(seats_.begin(), seats_.end(),
        [seat_id](const Seat& s) { return s.id() == seat_id; });
    if(it == seats_.end()) throw std::runtime_error("Seat not found");
    return *it;
}

std::vector<Product> ClubSystem::get_products() const {
    std::vector<Product> result;
    for(const auto& [id, product] : products_) {
        result.push_back(product);
    }
    return result;
}

std::vector<Client> ClubSystem::find_clients(const std::string& query) const {
    std::string lower_query;
    std::transform(query.begin(), query.end(), std::back_inserter(lower_query),
        [](unsigned char c){ return std::tolower(c); });
    
    std::vector<Client> result;
    for(const auto& [id, client] : clients_) {
        std::string name_lower = client.name();
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        std::string contact_lower = client.contact();
        std::transform(contact_lower.begin(), contact_lower.end(), contact_lower.begin(), ::tolower);
        
        if(name_lower.find(lower_query) != std::string::npos || 
           contact_lower.find(lower_query) != std::string::npos) {
            result.push_back(client);
        }
    }
    return result;
}

void ClubSystem::add_product(const Product& product) {
    auto& db = DatabaseManager::instance();
    
    db.execute(
        "INSERT INTO products (name, category, price, stock) VALUES (?, ?, ?, ?)",
        {
            product.name(),
            std::to_string(static_cast<int>(product.category())),
            std::to_string(product.price()),
            std::to_string(product.stock())
        }
    );
    
    load_products();
}

void ClubSystem::update_seat_status(int seat_id, Seat::Status new_status) {
    auto it = std::find_if(seats_.begin(), seats_.end(),
        [seat_id](const Seat& s) { return s.id() == seat_id; });
    
    if(it == seats_.end()) {
        throw std::runtime_error("Место не найдено");
    }
    
    DatabaseManager::instance().execute(
        "UPDATE seats SET status = ? WHERE id = ?",
        {
            std::to_string(static_cast<int>(new_status)),
            std::to_string(seat_id)
        }
    );
    
    load_seats();
}