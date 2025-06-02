#include "../../include/models/Tariff.h"

Tariff::Tariff(int id, std::string name, double base_rate, Period period)
    : id_(id),
      name_(std::move(name)),
      base_rate_(base_rate),
      period_(period)
{
    if(base_rate_ < 0) {
        throw std::invalid_argument("Base rate cannot be negative");
    }
}

double Tariff::calculate_cost(time_t duration) const {
    const double hours = duration / 3600.0;
    return current_rate() * hours;
}

double Tariff::calculate_cost(time_t start, time_t end) const {
    if(start >= end) {
        throw std::invalid_argument("Start time must be before end time");
    }
    return calculate_cost(end - start);
}

void Tariff::add_discount(double percent, time_t start, time_t end) {
    if(percent < 0 || percent > 100) {
        throw std::invalid_argument("Discount must be between 0 and 100 percent");
    }
    if(start >= end) {
        throw std::invalid_argument("Discount period start must be before end");
    }
    discounts_.push_back({percent, start, end});
}

bool Tariff::is_discount_active() const {
    const time_t now = time(nullptr);
    return std::any_of(discounts_.begin(), discounts_.end(),
        [now](const Discount& d) { return now >= d.start && now <= d.end; });
}

bool Tariff::is_active() const {
    // Заглушка - требует реализации логики периода
    // Пример: проверка текущего времени против периода тарифа
    return true;
}

bool Tariff::is_available_for(int seat_type) const {
    // Заглушка - требует реализации связи с типами мест
    return true;
}

double Tariff::current_rate() const {
    const double discount = current_discount();
    return base_rate_ * (1.0 - discount / 100.0);
}

void Tariff::set_base_rate(double new_rate) {
    if(new_rate < 0) {
        throw std::invalid_argument("Base rate cannot be negative");
    }
    base_rate_ = new_rate;
}

double Tariff::current_discount() const {
    const time_t now = time(nullptr);
    double max_discount = 0.0;
    
    for(const auto& discount : discounts_) {
        if(now >= discount.start && now <= discount.end) {
            max_discount = std::max(max_discount, discount.percent);
        }
    }
    return max_discount;
}

int Tariff::id() const noexcept { 
    return id_; 
}

const std::string& Tariff::name() const noexcept { 
    return name_; 
}

Tariff::Period Tariff::period() const noexcept { 
    return period_; 
}

void Tariff::set_period(Period new_period) { 
    period_ = new_period; 
}