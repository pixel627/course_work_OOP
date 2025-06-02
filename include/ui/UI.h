#pragma once

#pragma once
#include "../core/ClubSystem.h"
#include "../models/Client.h"
#include <functional>
#include <vector>
#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <climits>

class UI {
public:
    UI(ClubSystem& system);
    void start();
    
private:
    ClubSystem& clubSystem;
    bool running = true;

    void mainMenu();
    void showSeats();
    void reservationMenu();
    void clientManagementMenu();
    void salesMenu();

    void printHeader(const std::string& title);
    void clearScreen();
    int getChoice(int min, int max);
    void waitForContinue();
    
    void handleNewReservation();
    void handleCheckout();
    void showReservations();
    
    void addNewClient();
    void editClient();
    void clientLookup();
    void showProducts();
    void addNewProduct();
    std::string productCategoryToString(Product::Category category);
    void printReservationDetails(const Reservation& res);
    std::string seatTypeToString(Seat::Type type);
    void seatManagementMenu();
    void changeSeatStatus();
    
    std::string format_time(time_t time);
    std::string status_to_string(Reservation::Status status);
};