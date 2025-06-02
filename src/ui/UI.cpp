#include "../../include/ui/UI.h"


UI::UI(ClubSystem& system) : clubSystem(system) {}

void UI::start() {
    while(running) {
        mainMenu();
    }
}

void UI::mainMenu() {
    clearScreen();
    printHeader("Главное меню");
    std::cout << "1. Просмотр мест\n"
              << "2. Новое бронирование\n"
              << "3. Текущие бронирования\n"
              << "4. Управление клиентами\n"
              << "5. Управление продажами\n"
              << "6. Управление местами\n"
              << "7. Выход\n";
    
    switch(getChoice(1, 7)) {
        case 1: showSeats(); break;
        case 2: handleNewReservation(); break;
        case 3: showReservations(); break;
        case 4: clientManagementMenu(); break;
        case 5: salesMenu(); break;
        case 6: seatManagementMenu(); break;
        case 7: running = false; break;
    }
}

void UI::showSeats() {
    clearScreen();
    printHeader("Карта мест компьютерного клуба");
    
    const auto& seats = clubSystem.seats();
    if(seats.empty()) {
        std::cout << "Нет доступных мест!\n";
        waitForContinue();
        return;
    }

    for(size_t i = 0; i < seats.size(); ++i) {
        const auto& seat = seats[i];
        std::string color;
        std::string status;
        
        switch(seat.status()) { 
            case Seat::Status::Free: 
                color = "\033[32m"; 
                status = "Свободно";
                break;
            case Seat::Status::Reserved: 
                color = "\033[33m"; 
                status = "Забронировано";
                break;
            case Seat::Status::Occupied: 
                color = "\033[31m"; 
                status = "Занято";
                break;
            case Seat::Status::Maintenance: 
                color = "\033[34m"; 
                status = "Обслуживание";
                break;
        }
        
        std::cout << color 
                  << "╔════════════╗\n"
                  << "║ PC " << std::setw(4) << std::left << seat.id() << "    ║\n"
                  << "║ " << std::setw(10) << status << "   ║\n"
                  << "╚════════════╝\033[0m  ";
        
        if((i + 1) % 5 == 0) std::cout << "\n\n";
    }
    
    std::cout << "\n\nЛегенда:\n"
              << "\033[32mСвободно\033[0m  \033[33mЗабронировано\033[0m  "
              << "\033[31mЗанято\033[0m  \033[34mНа обслуживании\033[0m\n";
    waitForContinue();
}

void UI::reservationMenu() {
    clearScreen();
    printHeader("Управление бронированиями");
    std::cout << "1. Отменить бронирование\n"
              << "2. Детали бронирования\n"
              << "3. Назад\n";
    
    switch(getChoice(1, 3)) {
        case 1: {
            std::cout << "Введите ID бронирования: ";
            int id = getChoice(1, INT_MAX);
            try {
                clubSystem.reservations().cancel_reservation(id);
                std::cout << "\033[32mБронирование отменено!\033[0m\n";
            } catch(const std::exception& e) {
                std::cout << "\033[31mОшибка: " << e.what() << "\033[0m\n";
            }
            waitForContinue();
            break;
        }
        case 2: {
            std::cout << "Введите ID бронирования: ";
            int id = getChoice(1, INT_MAX);
            try {
                auto res = clubSystem.reservations().find_reservations(id)[0];
                printReservationDetails(res);
            } catch(...) {
                std::cout << "\033[31mБронирование не найдено!\033[0m\n";
            }
            waitForContinue();
            break;
        }
        case 3: return;
    }
}

void UI::printReservationDetails(const Reservation& res) {
    clearScreen();
    printHeader("Детали бронирования #" + std::to_string(res.id()));
    
    try {
        const Client& client = clubSystem.get_client(res.client_id());
        const Seat& seat = clubSystem.get_seat(res.seat_id());
        
        std::cout << "Клиент: " << client.name() << "\n"
                  << "Место: " << seat.id() << " (" << seatTypeToString(seat.type()) << ")\n"
                  << "Время: " << format_time(res.start_time()) 
                  << " - " << format_time(res.end_time()) << "\n"
                  << "Статус: " << status_to_string(res.status()) << "\n"
                  << "Стоимость: " << res.total_cost() << " руб.\n";
    } catch(const std::exception& e) {
        std::cout << "Ошибка загрузки данных: " << e.what() << "\n";
    }
}

std::string UI::seatTypeToString(Seat::Type type) {
    switch(type) {
        case Seat::Type::Standard: return "Стандарт";
        case Seat::Type::VIP: return "VIP";
        case Seat::Type::Gaming: return "Игровое";
        case Seat::Type::Conference: return "Конференция";
        default: return "Неизвестно";
    }
}

void UI::clientManagementMenu() {
    clearScreen();
    printHeader("Управление клиентами");
    std::cout << "1. Поиск клиента\n"
              << "2. Добавить клиента\n"
              << "3. Редактировать клиента\n"
              << "4. Назад\n";
    
    switch(getChoice(1, 4)) {
        case 1: clientLookup(); break;
        case 2: addNewClient(); break;
        case 3: editClient(); break;
        case 4: return;
    }
}

void UI::salesMenu() {
    clearScreen();
    printHeader("Управление продажами");
    std::cout << "1. Список продуктов\n"
              << "2. Добавить новый продукт\n"
              << "3. Назад\n";
    
    switch(getChoice(1, 3)) {
        case 1: showProducts(); break;
        case 2: addNewProduct(); break;
        case 3: return;
    }
}

void UI::showProducts() {
    clearScreen();
    printHeader("Доступные продукты");
    
    auto products = clubSystem.get_products();
    
    if(products.empty()) {
        std::cout << "Нет доступных продуктов\n";
    } else {
        std::cout << std::left 
                  << std::setw(5) << " ID "
                  << std::setw(20) << " Название "
                  << std::setw(15) << " Категория "
                  << std::setw(10) << " Цена "
                  << std::setw(10) << " Остаток\n";
        
        for(const auto& product : products) {
            std::cout << std::setw(5) << product.id()
                      << std::setw(20) << product.name()
                      << std::setw(15) << productCategoryToString(product.category())
                      << std::setw(10) << product.price()
                      << std::setw(10) << product.stock() << "\n";
        }
    }
    waitForContinue();
}

void UI::addNewProduct() {
    clearScreen();
    printHeader("Добавление нового продукта");
    
    std::string name;
    double price;
    int stock, category;

    std::cout << "Название продукта: ";
    std::getline(std::cin, name);
    
    std::cout << "Цена: ";
    while(!(std::cin >> price) || price <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Неверная цена. Введите снова: ";
    }
    std::cin.ignore();
    
    std::cout << "Количество: ";
    while(!(std::cin >> stock) || stock < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Неверное количество. Введите снова: ";
    }
    std::cin.ignore();
    
    std::cout << "Категории:\n";
    for(int i = 0; i <= static_cast<int>(Product::Category::Service); i++) {
        std::cout << i << ". " 
                  << productCategoryToString(static_cast<Product::Category>(i)) 
                  << "\n";
    }
    std::cout << "Выберите категорию: ";
    while(!(std::cin >> category) || category < 0 || 
          category > static_cast<int>(Product::Category::Service)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Неверная категория. Введите снова: ";
    }
    std::cin.ignore();

    try {
        Product new_product(
            0,  
            name,
            static_cast<Product::Category>(category),
            price,
            stock
        );
        
        clubSystem.add_product(new_product);
        std::cout << "\033[32mПродукт успешно добавлен!\033[0m\n";
    } catch(const std::exception& e) {
        std::cout << "\033[31mОшибка: " << e.what() << "\033[0m\n";
    }
    waitForContinue();
}

std::string UI::productCategoryToString(Product::Category category) {
    switch(category) {
        case Product::Category::Food: return "Еда";
        case Product::Category::Drink: return "Напиток";
        case Product::Category::Accessory: return "Аксессуар";
        case Product::Category::Service: return "Услуга";
        default: return "Неизвестно";
    }
}

void UI::handleNewReservation() {
    clearScreen();
    printHeader("Новое бронирование");
    
    try {
        std::cout << "Выберите место (1-" << clubSystem.seats().size() << "): ";
        int seat_id = getChoice(1, clubSystem.seats().size());
        
        std::cout << "Имя клиента: ";
        std::string name;
        std::getline(std::cin, name);
        
        std::cout << "Контакт: ";
        std::string contact;
        std::getline(std::cin, contact);
        
        Client client = clubSystem.create_client(name, contact);
        
        time_t now = time(nullptr);
        Reservation reservation = clubSystem.reservations().create_reservation(
            client.id(), seat_id, now, now + 3600); 
        
        std::cout << "\n\033[32mБронирование #" << reservation.id() << " создано!\033[0m\n";
    } catch(const std::exception& e) {
        std::cout << "\033[31mОшибка: " << e.what() << "\033[0m\n";
    }
    waitForContinue();
}

void UI::showReservations() {
    clearScreen();
    printHeader("Текущие бронирования");
    
    const auto reservations = clubSystem.reservations().find_reservations();
    std::cout << std::left << std::setw(10) << " ID " << std::setw(15) << " Клиент "
              << std::setw(10) << " Место " << std::setw(20) << " Начало"
              << std::setw(20) << " Окончание " << std::setw(15) << " Статус\n";
    
    for(const auto& res : reservations) {
        try {
            Client client = clubSystem.get_client(res.client_id());
            std::cout << std::setw(10) << res.id()
                      << std::setw(15) << client.name().substr(0, 14)
                      << std::setw(10) << res.seat_id()
                      << std::setw(20) << format_time(res.start_time())
                      << std::setw(20) << format_time(res.end_time())
                      << std::setw(15) << status_to_string(res.status()) << "\n";
        } catch(...) {}
    }
    waitForContinue();
}

void UI::addNewClient() {
    clearScreen();
    printHeader("Новый клиент");
    
    std::string name, contact;
    std::cout << "Имя: ";
    std::getline(std::cin, name);
    
    while(true) {
        std::cout << "Контакт (формат: +7XXXXXXXXXX или email@example.com): ";
        std::getline(std::cin, contact);
        
        try {
            Client temp(0, name, contact); 
            break;
        } catch(const std::exception& e) {
            std::cout << "\033[31mОшибка: " << e.what() << "\033[0m\n";
        }
    }
    
    try {
        Client client = clubSystem.create_client(name, contact);
        std::cout << "\n\033[32mКлиент #" << client.id() << " создан!\033[0m\n";
    } catch(const std::exception& e) {
        std::cout << "\033[31mОшибка: " << e.what() << "\033[0m\n";
    }
    waitForContinue();
}

void UI::editClient() {
    clearScreen();
    printHeader("Редактирование клиента");
    
    std::cout << "Введите ID клиента: ";
    int id = getChoice(1, INT_MAX);
    
    try {
        const Client& client = clubSystem.get_client(id);
        std::cout << "Текущий контакт: " << client.contact() << "\n";
        std::cout << "Новый контакт: ";
        std::string new_contact;
        std::getline(std::cin, new_contact);
        
        if(clubSystem.update_client(id, new_contact)) {
            std::cout << "\033[32mДанные обновлены!\033[0m\n";
        } else {
            std::cout << "\033[31mОшибка обновления\033[0m\n";
        }
    } catch(...) {
        std::cout << "\033[31mКлиент не найден!\033[0m\n";
    }
    waitForContinue();
}

void UI::clientLookup() {
    clearScreen();
    printHeader("Поиск клиента");
    
    std::cout << "Введите имя или контакт: ";
    std::string query;
    std::getline(std::cin, query);
    
    auto clients = clubSystem.find_clients(query);
    
    if(clients.empty()) {
        std::cout << "Клиенты не найдены\n";
    } else {
        for(const auto& client : clients) {
            std::cout << "ID: " << client.id() << " | Имя: " << client.name()
                      << " | Контакт: " << client.contact() << "\n";
        }
    }
    waitForContinue();
}

void UI::printHeader(const std::string& title) {
    std::cout << "\033[1;36m===== " << title << " =====\033[0m\n\n";
}

void UI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

int UI::getChoice(int min, int max) {
    int choice;
    while(true) {
        std::cout << "> ";
        if(!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Некорректный ввод. Попробуйте снова: ";
            continue;
        }
        std::cin.ignore();
        if(choice >= min && choice <= max) return choice;
        std::cout << "Допустимые значения: от " << min << " до " << max << ": ";
    }
}

void UI::waitForContinue() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

std::string UI::format_time(time_t time) {
    char buf[20];
    std::strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M", std::localtime(&time));
    return buf;
}

std::string UI::status_to_string(Reservation::Status status) {
    switch(status) {
        case Reservation::Status::Pending: return "Ожидание";
        case Reservation::Status::Active: return "Активно";
        case Reservation::Status::Completed: return "Завершено";
        case Reservation::Status::Cancelled: return "Отменено";
        default: return "Неизвестно";
    }
}

void UI::seatManagementMenu() {
    clearScreen();
    printHeader("Управление местами");
    std::cout << "1. Изменить статус места\n"
              << "2. Вернуться в главное меню\n";
    
    switch(getChoice(1, 2)) {
        case 1: changeSeatStatus(); break;
        case 2: return;
    }
}

void UI::changeSeatStatus() {
    clearScreen();
    printHeader("Изменение статуса места");
    
    std::cout << "Введите ID места (1-120): ";
    int seat_id = getChoice(1, 120);
    
    std::cout << "Выберите новый статус:\n"
              << "1. Занято\n"
              << "2. На обслуживание\n"
              << "3. Свободно\n";
    
    Seat::Status new_status;
    switch(getChoice(1, 3)) {
        case 1: new_status = Seat::Status::Occupied; break;
        case 2: new_status = Seat::Status::Maintenance; break;
        case 3: new_status = Seat::Status::Free; break;
    }
    
    try {
        clubSystem.update_seat_status(seat_id, new_status);
        std::cout << "\033[32mСтатус успешно изменен!\033[0m\n";
    } catch(const std::exception& e) {
        std::cout << "\033[31mОшибка: " << e.what() << "\033[0m\n";
    }
    waitForContinue();
}