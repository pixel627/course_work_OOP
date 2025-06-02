#pragma once

#include <ctime>
#include <stdexcept>

class Reservation {
public:
    enum class Status { 
        Pending,    // Ожидает подтверждения
        Active,     // Активно используется
        Completed,  // Успешно завершено
        Cancelled,  // Отменено
        ANY         // Специальный статус для фильтрации
    };

    Reservation(int id, 
                int client_id, 
                int seat_id,
                time_t start_time,
                time_t end_time,
                Status status,
                double total_cost = 0.0);

    void cancel() noexcept;
    void activate() noexcept;
    void complete() noexcept;

    int id() const noexcept;
    int client_id() const noexcept;
    int seat_id() const noexcept;
    time_t start_time() const noexcept;
    time_t end_time() const noexcept;
    Status status() const noexcept;
    double total_cost() const noexcept;

    void set_total_cost(double cost) noexcept;

private:
    int id_;            // Уникальный идентификатор брони
    int client_id_;     // ID клиента
    int seat_id_;       // ID места
    time_t start_time_; // Время начала бронирования (UNIX timestamp)
    time_t end_time_;   // Время окончания бронирования
    Status status_;     // Текущий статус
    double total_cost_; // Общая стоимость бронирования
};