# Food Delivery System (C)

A comprehensive, console-based food delivery simulation coded in C—showcasing modular programming, file I/O, geolocation APIs, and system-level data management. This project mimics the core workflows of popular food delivery apps, enabling user registration, restaurant discovery, menu browsing, order placement, payment, tracking, and feedback, all within your terminal.

## Features

- **User Registration & Login:**  
  Authenticate and manage users. User information is saved persistently in `users.csv`.

- **Restaurant & Menu Management:**  
  Restaurants and their menus are managed via CSV files. Browse offerings, filter by name/location, and display by distance.

- **Geolocation & Maps:**  
  User-provided addresses are geocoded (via OpenCageData API) for location-based suggestions and tracking. Order tracking generates a Leaflet.js-powered map route between user and restaurant.

- **Food Ordering Flow:**  
  Add dishes to your cart from restaurant menus, calculate subtotals (with GST and delivery charges), and confirm orders.

- **Multiple Payment Methods:**  
  Simulated payments: Cash, Card, UPI, and Wallet—complete with prompts and input checks.

- **Order Tracking & Feedback:**  
  Track orders on a map and leave feedback or ratings per restaurant. Feedback is stored and can be viewed for each venue.

- **File-based Persistence:**  
  No database required—all data is read/written in CSVs, keeping things transparent and beginner-friendly.

## Getting Started

### Requirements

- GCC compiler for C
- Linux/Unix recommended (POSIX file ops used)
- `curl` and `json-c` libraries for API communications
- Internet access (for geocoding user addresses)

### Compilation

```bash
gcc Food_Delivery_System.c -o FoodDeliverySystem -lcurl -ljson-c
```

### Running

```bash
./FoodDeliverySystem
```

### Directory Setup

- Place restaurant CSV files in your `FOLDER_PATH` as defined in the code. Each CSV should hold restaurant details and menu items (example structure provided in the repo).
- Ensure an **empty** `users.csv` file exists—or let the system generate one on first registration.

## Usage Flow

1. **Sign up or log in** as a user.
2. Enter your address (lat/lon fetched via the API).
3. Browse/search for restaurants (location-aware options available).
4. Review menus and add items to your cart.
5. See order summary—view taxes and delivery costs.
6. Choose a payment option and confirm your order.
7. (Optional) Track your order on a map.
8. (Optional) Leave feedback for the restaurant.

## Example Restaurant CSV

```
28.61,77.23,Connaught Place,4.5
S No.,Cuisine,Price
1,Pizza,250
2,Burger,120
3,Pasta,200
```
- **First line:** latitude, longitude, address, rating.
- **Next line:** menu header.
- **Following lines:** menu items.

## Limitations

- All logic and storage is local (no real database or networked backend).
- Payment is simulated; not for real financial transactions!
- File-based, so be mindful of correct file placements and read/write permissions.
- Some error handling (input/file issues) can be improved for robustness.

## Educational Value

Great for:
- Learning organized C code and larger project structures.
- Hands-on practice with CSV parsing, file directory operations, and APIs in C.
- Demonstrating how real-world workflows (food delivery, user management, etc.) can be implemented from scratch.
