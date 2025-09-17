#include <bits/stdc++.h>
using namespace std;

class Vehicle {
protected:
    string route;
    int capacity;
    bool status;  // true = on-time, false = delayed
    int bookedSeats;  // số ghế đã được đặt

public:
    Vehicle(const string& r, int c)
        : route(r), capacity(c), status(true), bookedSeats(0) {}

    virtual ~Vehicle() {}

    // Phương thức tính thời gian di chuyển, virtual để có thể override
    virtual double calculateTravelTime(double distance) const {
        double DEFAULT_SPEED = 50.0; // km/h giả định
        if (DEFAULT_SPEED <= 0) return 0;
        return distance / DEFAULT_SPEED;
    }

    // Đặt trạng thái chậm / đúng giờ
    void setStatus(bool st) {
        status = st;
    }

    bool getStatus() const {
        return status;
    }

    string getRoute() const {
        return route;
    }

    int getCapacity() const {
        return capacity;
    }

    int getBookedSeats() const {
        return bookedSeats;
    }

    bool bookSeat() {
        if (bookedSeats < capacity) {
            bookedSeats++;
            return true;
        } else {
            return false;
        }
    }

    bool cancelSeat() {
        if (bookedSeats > 0) {
            bookedSeats--;
            return true;
        } else {
            return false;
        }
    }

    virtual void displayInfo() const {
        cout << "Vehicle - Route: " << route
             << ", Capacity: " << capacity
             << ", Booked: " << bookedSeats
             << ", Status: " << (status ? "On-time" : "Delayed")
             << endl;
    }
};

class ExpressBus : public Vehicle {
private:
    double speed;   // km/h cao hơn
    int fewerStops; // số lượng bớt trạm dừng

public:
    ExpressBus(const string& r, int c, double sp, int fewer)
        : Vehicle(r, c), speed(sp), fewerStops(fewer) {}

    double calculateTravelTime(double distance) const override {
        if (speed <= 0) return 0;
        // Giảm thời gian 20% so với nếu chạy với tốc độ bình thường (ví dụ speed ~ default * 1.2)
        // hoặc đơn giản tính distance / speed
        return distance / speed;
    }

    int getFewerStops() const {
        return fewerStops;
    }

    void displayInfo() const override {
        cout << "ExpressBus - Route: " << route
             << ", Capacity: " << capacity
             << ", Booked: " << bookedSeats
             << ", Speed: " << speed << " km/h"
             << ", FewerStops: " << fewerStops
             << ", Status: " << (status ? "On-time" : "Delayed")
             << endl;
    }
}

;

class Schedule {
private:
    shared_ptr<Vehicle> vehicle;  // sử dụng shared_ptr để chia sẻ Vehicle giữa các nơi
    string arrivalTime;
    string departureTime;

public:
    Schedule(shared_ptr<Vehicle> v, const string& at, const string& dt)
      : vehicle(v), arrivalTime(at), departureTime(dt) {}

    void displaySchedule() const {
        cout << "Schedule: ";
        vehicle->displayInfo();
        cout << "   Arrival: " << arrivalTime
             << ", Departure: " << departureTime << endl;
    }

    shared_ptr<Vehicle> getVehicle() const {
        return vehicle;
    }
};

class Station {
protected:
    string name;
    string location;
    string type; // "bus" hoặc "train"
    vector<shared_ptr<Schedule>> schedules;

public:
    Station(const string& n, const string& loc, const string& t)
        : name(n), location(loc), type(t) {}

    void addSchedule(shared_ptr<Schedule> sched) {
        if (schedules.size() >= 10) {
            cout << "Cannot add schedule: maximum number of schedules (10) reached for station \"" 
                 << name << "\"." << endl;
        } else {
            schedules.push_back(sched);
            cout << "Schedule added to station \"" << name << "\"." << endl;
        }
    }

    void removeSchedule(int index) {
        if (index < 0 || index >= (int) schedules.size()) {
            cout << "Invalid schedule index for removal." << endl;
        } else {
            schedules.erase(schedules.begin() + index);
            cout << "Schedule removed from station \"" << name << "\"." << endl;
        }
    }

    void displaySchedules() const {
        cout << "Schedules at Station \"" << name << "\" (Type: " << type << ", Location: " << location << "):" << endl;
        for (size_t i = 0; i < schedules.size(); ++i) {
            cout << "  [" << i << "] ";
            schedules[i]->displaySchedule();
        }
    }

    string getName() const {
        return name;
    }
};

class Passenger {
private:
    string name;
    int id;
    vector< shared_ptr<Vehicle> > bookedVehicles;

public:
    Passenger(const string& n, int i)
      : name(n), id(i) {}

    void bookTicket(shared_ptr<Vehicle> v) {
        if (!v) {
            cout << "Invalid vehicle." << endl;
            return;
        }
        if (v->bookSeat()) {
            bookedVehicles.push_back(v);
            cout << "Passenger " << name << " (ID " << id << ") booked ticket on route " << v->getRoute() << "." << endl;
        } else {
            cout << "Booking failed: vehicle route " << v->getRoute() << " is full." << endl;
        }
    }

    void cancelTicket(shared_ptr<Vehicle> v) {
        auto it = find(bookedVehicles.begin(), bookedVehicles.end(), v);
        if (it != bookedVehicles.end()) {
            if (v->cancelSeat()) {
                bookedVehicles.erase(it);
                cout << "Passenger " << name << " (ID " << id << ") canceled ticket on route " << v->getRoute() << "." << endl;
            } else {
                cout << "Cancellation failed: no booked seats in vehicle " << v->getRoute() << "." << endl;
            }
        } else {
            cout << "Cancellation failed: no such booking found." << endl;
        }
    }

    void displayBookings() const {
        cout << "Passenger " << name << " (ID " << id << ") bookings:" << endl;
        if (bookedVehicles.empty()) {
            cout << "  No bookings." << endl;
        } else {
            for (auto& v : bookedVehicles) {
                cout << "  -> ";
                v->displayInfo();
            }
        }
    }
};

int main() {
    // Tạo vài vehicle
    auto v1 = make_shared<Vehicle>("Route A", 30);
    auto v2 = make_shared<ExpressBus>("Route B (Express)", 20, 75.0, 5);

    // Tạo station
    Station st1("Central Station", "Downtown", "bus");

    // Tạo schedule
    auto sched1 = make_shared<Schedule>(v1, "08:00", "09:15");
    auto sched2 = make_shared<Schedule>(v2, "09:30", "10:10");

    // Thêm schedule vào station
    st1.addSchedule(sched1);
    st1.addSchedule(sched2);

    // Hiển thị theo station
    st1.displaySchedules();

    // Tạo hành khách
    Passenger p1("Alice", 1001);
    Passenger p2("Bob", 1002);

    // Hành khách đặt vé
    p1.bookTicket(v1);
    p1.bookTicket(v2);
    // Thử đặt khi đầy
    for (int i = 0; i < 30; ++i) {
        p2.bookTicket(v1);
    }

    // Hiển thị đặt vé của hành khách
    p1.displayBookings();
    p2.displayBookings();

    // Tính toán thời gian di chuyển
    double dist = 120.0; // km
    cout << "Travel time for Vehicle on route " << v1->getRoute() << " for distance " << dist << " km: "
         << v1->calculateTravelTime(dist) << " hours." << endl;
    cout << "Travel time for ExpressBus on route " << v2->getRoute() << " for distance " << dist << " km: "
         << v2->calculateTravelTime(dist) << " hours." << endl;

    // Thử xóa schedule
    st1.removeSchedule(0);
    st1.displaySchedules();

    return 0;
}
