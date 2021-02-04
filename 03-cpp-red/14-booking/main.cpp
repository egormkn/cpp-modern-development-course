#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Booking {
  int64_t time;
  int client_id;
  int room_count;
};

class RecentBookings {
 public:
  void Add(const Booking& booking) { // O(log Q)
    bookings.push(booking); // amortized O(1)
    client_bookings[booking.client_id]++; // O(log Q)
    room_bookings += booking.room_count; // O(1)
  }

  int GetClients(int64_t current_time) { // O(1)
    Clear(current_time); // amortized O(1) / O(log Q)
    return client_bookings.size(); // O(1)
  }

  int GetRooms(int64_t current_time) { // O(1)
    Clear(current_time); // amortized O(1) / O(log Q)
    return room_bookings; // O(1)
  }

 private:
  static const int TIME_WINDOW_SIZE = 86400;
  
  queue<Booking> bookings;
  map<int, int> client_bookings;
  int room_bookings;

  void Clear(int64_t current_time) {
    while (!bookings.empty() && bookings.front().time <= current_time - TIME_WINDOW_SIZE) {
      Booking& booking = bookings.front();
      if (--client_bookings[booking.client_id] == 0) {
        client_bookings.erase(booking.client_id);
      }
      room_bookings -= booking.room_count;
      bookings.pop();
    }
  }
};

class BookingManager {
 public:
  void Book(int64_t time, const string& hotel_name, int client_id,
            int room_count) {
    current_time = time;
    hotels[hotel_name].Add({time, client_id, room_count});
  }

  int GetClients(const string& hotel_name) {
    return hotels.count(hotel_name)
               ? hotels.at(hotel_name).GetClients(current_time)
               : 0;
  }

  int GetRooms(const string& hotel_name) {
    return hotels.count(hotel_name)
               ? hotels.at(hotel_name).GetRooms(current_time)
               : 0;
  }

 private:
  map<string, RecentBookings> hotels;
  int64_t current_time;
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  BookingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;

    if (query_type == "BOOK") {
      int64_t time;
      string hotel_name;
      int client_id, room_count;
      cin >> time >> hotel_name >> client_id >> room_count;
      manager.Book(time, hotel_name, client_id, room_count);
    } else if (query_type == "CLIENTS") {
      string hotel_name;
      cin >> hotel_name;
      cout << manager.GetClients(hotel_name) << "\n";
    } else if (query_type == "ROOMS") {
      string hotel_name;
      cin >> hotel_name;
      cout << manager.GetRooms(hotel_name) << "\n";
    }
  }

  return 0;
}