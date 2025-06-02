#include "../include/core/ClubSystem.h"
#include "../include/ui/UI.h"
#include <stdexcept>
#include <filesystem>  


int main() {
    ClubSystem system; 
    system.initialize("data/club.db");
    
    UI ui(system);
    ui.start();
    return 0;
}