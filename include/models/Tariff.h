#pragma once

#include <ctime>
#include <vector>
#include <optional>
#include <stdexcept>
#include <algorithm>

class Tariff {
public:
    enum class Period { Peak, OffPeak, Holiday };
    
    Tariff(int id, std::string name, double base_rate, 
          Period period = Period::Peak);
    
    double calculate_cost(time_t duration) const;
    double calculate_cost(time_t start, time_t end) const;

    void add_discount(double percent, time_t start, time_t end);
    bool is_discount_active() const;
    
    bool is_active() const;
    bool is_available_for(int seat_type) const;
    
    int id() const noexcept;
    const std::string& name() const noexcept;
    double current_rate() const;
    Period period() const noexcept;
    
    void set_base_rate(double new_rate);
    void set_period(Period new_period);

private:
    int id_;
    std::string name_;
    double base_rate_;
    Period period_;
    
    struct Discount {
        double percent;
        time_t start;
        time_t end;
    };
    std::vector<Discount> discounts_;
    
    double current_discount() const;
};