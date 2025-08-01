/*
Kindly refer to the important comments for necessary lines.
*/


//All necessary modules.
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <json-c/json.h>

//Defining some constants, which are about to be used latter in this program.
#define FILENAME "users.csv"
#define API_KEY "Your API key" // The api for maps
#define FOLDER_PATH "/mnt/c/Users/Akshit Kumar Tiwari/python files/Programs/Tech_Test/Restdata" // Upload data here
#define MAX_CUISINES 10
#define MAX_CART 20
#define MAX_NAME_LEN 50
#define MAX_ADDRESS_LEN 100
#define MAX_FILENAME_LEN 256
#define GST_RATE 0.18
#define DELIVERY_CHARGE 30.0
#define MAX_USERS 100
#define MAX_DISHES 200
#define MAX_RESTAURANTS 100
#define MAX_LINE_LENGTH 200
#define MAX_FEEDBACKS 200
#define WALLET_MONEY 2000

//Declaration of necessary global variables.
char selected_path[MAX_FILENAME_LEN];
int choice1;
float grand_total;

//Defining structures.
typedef struct {
    char name[MAX_NAME_LEN];
    float price;
} Cuisine;

typedef struct {
    char feedback[MAX_FEEDBACKS][256];
    int ratings[MAX_FEEDBACKS];
    int feedbackCount;
} Customer;

typedef struct {
    char address[MAX_ADDRESS_LEN];
    float rating;
    float latitude;
    float longitude;
} RestaurantInfo;

typedef struct {
    char city[50];
    double latitude;
    double longitude;
    float rating;
    int num_dishes;
    char name[MAX_NAME_LEN];
    Cuisine cuisines[MAX_CUISINES];
    int cuisine_count;
    struct {
        char dish[50];
        int price;
    } menu[MAX_DISHES];
    RestaurantInfo info;
    float distance;
    char feedback[MAX_FEEDBACKS][256];
    int ratings[MAX_FEEDBACKS];
    int feedbackCount;
} Restaurant;

typedef struct {
    Cuisine cart[MAX_CUISINES];
    int count;
} Cart;

struct User {
    char name[50];
    char address[100];
    char city[50];
    char state[50];
    char pincode[7]; // Updated pincode size to accommodate 6 digits + null terminator
    char diet[10];
    char password[20];
    char email[50];
    double latitude;
    double longitude;
};

struct buffer {
    char *data;
    size_t size;
};

//Declaration of important functions.
float calculate_distance(float lat1, float lon1, float lat2, float lon2);
void list_restaurants_distance(const char *folder_path, float user_lat, float user_lon, Restaurant *restaurants, int *num_restaurants);
void display_menu_distance(Restaurant *restaurant, Cart *cart);
void openMapInBrowser(float startLatitude, float startLongitude, float endLatitude, float endLongitude);
void show_cart(Cart *cart);
void payWithCash();
void payWithCard();
void payWithUPI();
int generateRandom4DigitNumber();
void openMapInBrowser(float startLatitude, float startLongitude, float endLatitude, float endLongitude);
bool login();
void createUser();
int run_restaurant_program(const char *folder_path, const char *search_name);
void get_user_coordinates_from_file(const char *filename, float *user_lat, float *user_lon);
void restaurant_Coordinates(const char *file_path, float *latitude, float *longitude);
void print_joined_paths(const char *folder_path);
char* get_selected_restaurant_csv_path(const char *folder_path, float user_lat, float user_lon, int choice, char *selected_path);
int select_restaurant_menu_rating(const char *folder_path, Cart *cart);
void read_restaurants_from_file1(char *filename, Restaurant restaurants[], int *num_restaurants);
void search_food1(char *food_name, Restaurant restaurants[], int num_restaurants, int cart[]);
void display_restaurant_menu1(Restaurant restaurant, int cart[]);
void show_cart1(int cart[], Restaurant restaurants[], int num_restaurants);
void proceed_to_pay1(int cart[], Restaurant restaurants[], int num_restaurants);
void delete_item_from_cart1(int cart[], int *num_items);
void clear_input_buffer1();
void runRestaurantOrderingSystem1(Restaurant restaurants[], int num_restaurants);
void runFeedbackSystem(Restaurant* restaurant, Customer* customer);
void paybyWallet();
int load_restaurant_data_rating(const char *folder_path, const char *restaurant_name, Restaurant *restaurant);
void list_restaurants_rating(const char *folder_path, Restaurant *restaurants, int *num_restaurants);
void display_menu_rating(Restaurant *restaurant, Cart *cart);
int select_restaurant_menu_rating(const char *folder_path, Cart *cart);
void add_to_cart(Cart *cart, Restaurant *restaurant, int index);
void delete_from_cart(Cart *cart, int index);
float calculate_total_price(Cart *cart);

void writeUserToCSV(const struct User *user) {
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%lf,%lf\n",
            user->name, user->address, user->city, user->state, user->pincode,
            user->diet, user->password, user->email, user->latitude, user->longitude);
    fclose(file);
}

bool readUserFromCSV(const char *name, const char *password, struct User *user) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return false;
    }

    char line[256]; // Assuming each line in users.csv is less than 256 characters

    while (fgets(line, sizeof(line), file)) {
        // Parse each line to extract fields
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%lf\n",
               user->name, user->address, user->city, user->state, user->pincode,
               user->diet, user->password, user->email, &user->latitude, &user->longitude);

        // Remove newline character from password
        user->password[strcspn(user->password, "\n")] = '\0';

        // Trim extra spaces from user->name (optional, if needed)
        char trimmed_name[50];
        sscanf(user->name, "%s", trimmed_name);

        // Debug output
        // printf("Read from file: name=%s, password=%s\n", trimmed_name, user->password);

        // Compare trimmed_name and password
        if (strcmp(trimmed_name, name) == 0 && strcmp(user->password, password) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}


bool isValidEmail(const char *email) {
    int atCount = 0;
    int dotCount = 0;
    int length = strlen(email);
    for (int i = 0; i < length; i++) {
        if (email[i] == '@') {
            atCount++;
        } else if (email[i] == '.') {
            dotCount++;
        }
    }
    return (atCount == 1 && dotCount >= 1);
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, struct buffer *buf) {
    size_t new_size = buf->size + size * nmemb;
    buf->data = realloc(buf->data, new_size + 1);
    if (buf->data == NULL) {
        fprintf(stderr, "Realloc failed\n");
        return 0;
    }
    memcpy(buf->data + buf->size, ptr, size * nmemb);
    buf->size = new_size;
    buf->data[new_size] = '\0';
    return size * nmemb;
}

void url_encode(const char *src, char *dest) {
    const char *hex = "0123456789ABCDEF";
    while (*src) {
        if (('a' <= *src && *src <= 'z') ||
            ('A' <= *src && *src <= 'Z') ||
            ('0' <= *src && *src <= '9')) {
            *dest++ = *src;
        } else {
            *dest++ = '%';
            *dest++ = hex[(unsigned char)*src >> 4];
            *dest++ = hex[(unsigned char)*src & 15];
        }
        src++;
    }
    *dest = '\0';
}

void getCoordinates(const char *full_address, double *latitude, double *longitude) {
    CURL *curl;
    CURLcode res;
    struct buffer response;
    char url[2048];
    char encoded_address[1024];

    response.data = malloc(1);
    response.size = 0;

    url_encode(full_address, encoded_address);
    snprintf(url, sizeof(url), "https://api.opencagedata.com/geocode/v1/json?q=%s&key=%s", encoded_address, API_KEY);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            struct json_object *parsed_json;
            struct json_object *status;
            struct json_object *status_code;
            struct json_object *status_message;
            struct json_object *results;
            struct json_object *first_result;
            struct json_object *geometry;
            struct json_object *lat;
            struct json_object *lng;

            parsed_json = json_tokener_parse(response.data);
            json_object_object_get_ex(parsed_json, "status", &status);
            json_object_object_get_ex(status, "code", &status_code);
            json_object_object_get_ex(status, "message", &status_message);

            int code = json_object_get_int(status_code);
            const char* message = json_object_get_string(status_message);

            printf("API Status: %d - %s\n", code, message); // Debug print

            if (code == 200) { // Check if the response is OK
                json_object_object_get_ex(parsed_json, "results", &results);
                if (json_object_array_length(results) > 0) {
                    first_result = json_object_array_get_idx(results, 0);
                    json_object_object_get_ex(first_result, "geometry", &geometry);
                    json_object_object_get_ex(geometry, "lat", &lat);
                    json_object_object_get_ex(geometry, "lng", &lng);

                    *latitude = json_object_get_double(lat);
                    *longitude = json_object_get_double(lng);
                } else {
                    printf("No results found.\n");
                    *latitude = 0.0;
                    *longitude = 0.0;
                }
            } else {
                printf("API Error: %s\n", message);
                *latitude = 0.0;
                *longitude = 0.0;
            }

            json_object_put(parsed_json);
        }

        curl_easy_cleanup(curl);
    }

    free(response.data);
}

bool login() {
    char name[50];
    char password[20];
    printf("Enter your name: ");
    scanf("%s", name);
    printf("Enter your password: ");
    scanf("%s", password);

    struct User tempUser;
    if (readUserFromCSV(name, password, &tempUser)) {
        printf("Login successful!\n");
        return true;
    } else {
        printf("Invalid username or password.\n");
        return false;
    }
}

void createUser() {
    static int userCount = 0;  // Ensuring userCount is maintained correctly
    if (userCount == MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return;
    }

    struct User newUser;

    // Clear newline from previous input
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Enter your name: ");
    fflush(stdout);
    fgets(newUser.name, sizeof(newUser.name), stdin);
    newUser.name[strcspn(newUser.name, "\n")] = '\0'; // Remove newline character

    do {
        printf("Enter your email: ");
        scanf("%s", newUser.email);
        if (!isValidEmail(newUser.email)) {
            printf("Invalid email format. Please enter a valid email.\n");
        }
    } while (!isValidEmail(newUser.email));

    printf("Enter your password: ");
    scanf("%s", newUser.password);

    while ((c = getchar()) != '\n' && c != EOF);

    printf("Enter your address: ");
    fflush(stdout);
    fgets(newUser.address, sizeof(newUser.address), stdin);
    newUser.address[strcspn(newUser.address, "\n")] = '\0';

    printf("Enter your city: ");
    fgets(newUser.city, sizeof(newUser.city), stdin);
    newUser.city[strcspn(newUser.city, "\n")] = '\0';

    printf("Enter your state: ");
    fgets(newUser.state, sizeof(newUser.state), stdin);
    newUser.state[strcspn(newUser.state, "\n")] = '\0';

    printf("Enter your 6-digit pincode: ");
    scanf("%s", newUser.pincode);

    printf("Enter your diet (veg/non-veg): ");
    scanf("%s", newUser.diet);

    char full_address[256];
    snprintf(full_address, sizeof(full_address), "%s, %s, %s, %s", newUser.address, newUser.city, newUser.state, newUser.pincode);

    getCoordinates(full_address, &newUser.latitude, &newUser.longitude);

    writeUserToCSV(&newUser);

    userCount++;
    printf("User created successfully!\n");
}

void display_restaurant_header() {
    printf("\n");
    printf("---------------------------------------------------\n");
    printf(" S No. | %-15s | %-20s | %-7s\n", "Restaurant", "Location", "Rating");
    printf("---------------------------------------------------\n");
}

void display_restaurant_info(Restaurant *restaurant, int index) {
    printf("%3d.   | %-15s | %-20s | %-4.1f/5\n", index, restaurant->name, restaurant->info.address, restaurant->info.rating);
}

void display_menu_header(Restaurant *restaurant) {
    printf("\n%s\n", restaurant->name);
    printf("---------------\n");
    printf(" S No. | %-10s | %-7s\n", "Cuisines", "Price");
    printf("________________________\n");
}

void display_cuisines(Restaurant *restaurant) {
    for (int i = 0; i < restaurant->cuisine_count; i++) {
        printf("%3d.   | %-10s | Rs%.2f\n", i + 1, restaurant->cuisines[i].name, restaurant->cuisines[i].price);
    }
}

void add_to_cart(Cart *cart, Restaurant *restaurant, int index) {
    if (index < 1 || index > restaurant->cuisine_count) {
        printf("Invalid index.\n");
        return;
    }
    cart->cart[cart->count++] = restaurant->cuisines[index - 1];
    printf("Added %s to cart.\n", restaurant->cuisines[index - 1].name);
}

void delete_from_cart(Cart *cart, int index) {
    if (index < 1 || index > cart->count) {
        printf("Invalid index.\n");
        return;
    }
    for (int i = index - 1; i < cart->count - 1; i++) {
        cart->cart[i] = cart->cart[i + 1];
    }
    cart->count--;
    printf("Item removed from cart.\n");
}

float calculate_total_price(Cart *cart) {
    float total = 0.0;
    for (int i = 0; i < cart->count; i++) {
        total += cart->cart[i].price;
    }
    total += total * GST_RATE;
    total += DELIVERY_CHARGE;
    return total;
}

void show_cart(Cart *cart) {
    int choice;
    int after_payment_choice;

    do {
        printf("\nItems in cart:\n");
        printf("------------------------------\n");
        printf(" S No. | %-15s | %-7s\n", "Food Item", "Price");
        printf("------------------------------\n");

        for (int i = 0; i < cart->count; i++) {
            printf("%3d.   | %-15s | Rs%.2f\n", i + 1, cart->cart[i].name, cart->cart[i].price);
        }

        float total_price = calculate_total_price(cart);
        float gst = total_price * GST_RATE;
        grand_total = total_price + gst + DELIVERY_CHARGE;

        printf("\nTotal: Rs%.2f\n", total_price);
        printf("GST (18%%): Rs%.2f\n", gst);
        printf("Delivery Charge: Rs%.2f\n", DELIVERY_CHARGE);
        printf("Grand Total: Rs%.2f\n", grand_total);

        printf("\nOptions:\n");
        printf("1. Proceed to Pay\n");
        printf("2. Delete an item from the cart\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 2:
                int delete_index;
                printf("Enter the index of the item to delete: ");
                scanf("%d", &delete_index);
                delete_from_cart(cart, delete_index);
                break;
            case 1:
                printf("Proceeding to payment...\n");

                int payment_choice;

                printf("Select Payment Method:\n");
                printf("1. Cash\n");
                printf("2. Card\n");
                printf("3. UPI\n");
                printf("4. Wallet\n");
                printf("Enter your choice (1/2/3/4): ");
                scanf("%d", &payment_choice);

                switch (payment_choice) {
                    case 1:
                        printf("You have selected to pay with Cash.\n");
                        printf("Your order is confirmed.\n");
                        break;
                    case 2:
                        printf("You have selected to pay with Card.\n");
                        payWithCard();
                        break;
                    case 3:
                        printf("You have selected to pay with UPI.\n");
                        payWithUPI();
                        break;
                    case 4:
                        printf("You have selected to pay with Wallet.\n");
                        paybyWallet();
                        break;
                    default:
                        printf("Invalid choice!\n");
                        break;
                }

                printf("\nAdditional Options:\n");
                printf("1. Track your order\n");
                printf("2. Dear customer give you feedback out of 5: \n");
                printf("3. Return to previous menu\n");
                printf("4. Return to main menu\n");
                printf("Enter your choice: ");
                scanf("%d", &after_payment_choice);

                switch (after_payment_choice) {
                    case 1:
                    float startLatitude, startLongitude, endLatitude, endLongitude;
                        printf("Tracking your order...\n");
                        get_user_coordinates_from_file("users.csv", &startLatitude, &startLongitude);
                        get_selected_restaurant_csv_path(FOLDER_PATH, startLatitude, startLongitude, choice1, selected_path);
                        restaurant_Coordinates(selected_path, &endLatitude, &endLongitude);
                        openMapInBrowser(startLatitude, startLongitude, endLatitude, endLongitude);
                        break;
                    case 4:
                        printf("Returning to main menu...\n");
                        return;  // Exit the function, returning control to main()
                    case 2:
                        Restaurant restaurant = {.feedbackCount = 0};
                        Customer customer = {.feedbackCount = 0};
                        runFeedbackSystem(&restaurant, &customer);
                        break;
                    case 3:
                        ("Returning to previous menu...\n");
                        break;
                    default:
                        printf("Invalid choice! Returning to main menu...\n");
                        return;  // Exit the function, returning control to main()1

                }

                break;
            case 0:
                printf("Exiting the previous menu...\n");
                break;
            default:
                printf("Invalid choice! Please enter a valid option.\n");
                break;
        }

    } while (choice != 0);
}

// Function to extract the latitude and longitude from a line
void extract_coordinates(const char *line, float *latitude, float *longitude) {
    sscanf(line, "%f,%f", latitude, longitude);
}

// Function to process a specific CSV file and store the coordinates
void restaurant_Coordinates(const char *file_path, float *latitude, float *longitude) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    char line[1024];

    // Read the first line to get the coordinates
    if (fgets(line, sizeof(line), file)) {
        extract_coordinates(line, latitude, longitude);
    }

    fclose(file);
}

void payWithCash() {
    printf("You have selected to pay with Cash.\n");
    printf("Your order is confirmed ");
}

void payWithCard() {
    int num,code;
    char exp[6];
    char name[50];
    printf("You have selected to pay with Card.\n");
    printf("Enter the card details.\n");
    printf("Enter the name of the card beholder.\n");
    scanf("%s",name);
    printf("Enter the card number\n");
    scanf("%d",&num);
    printf("Enter the CVV code(xxx)\n");
    scanf("%d",&code);
    printf("Enter the expiry date of the card(MM/YY).\n");
    scanf("%s",exp);
    printf("Your order is confirmed.\n");

}

void payWithUPI() {
    int number;
    printf("You have selected to pay with UPI.\n");
    printf("Enter your phone number: ");
    scanf("%d", &number);
    int upiCode = generateRandom4DigitNumber();
    printf("Your UPI PIN is: %d\n", upiCode);
}

int generateRandom4DigitNumber() {
    return (rand() % 9000) + 1000; // Corrected the return statement
}

int load_restaurant_data(const char *folder_path, const char *restaurant_name, Restaurant *restaurant) {
    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s/%s.csv", folder_path, restaurant_name);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    // Read restaurant info (first line)
    char line[256];
    fgets(line, sizeof(line), file);
    sscanf(line, "%*f,%*f,%[^,],%*[^,],%*[^,],%f", restaurant->info.address, &restaurant->info.rating);

    // Read and skip the header line for cuisines
    fgets(line, sizeof(line), file);

    restaurant->cuisine_count = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%*d,%[^,],%f", restaurant->cuisines[restaurant->cuisine_count].name,
               &restaurant->cuisines[restaurant->cuisine_count].price);
        restaurant->cuisine_count++;
    }

    fclose(file);
    strcpy(restaurant->name, restaurant_name);

    return 1;
}

void list_restaurants(const char *folder_path, const char *search_name) {
    DIR *dir;
    struct dirent *ent;
    int index = 1;

    if ((dir = opendir(folder_path)) != NULL) {
        display_restaurant_header();

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Check if it's a regular file
                char *filename = ent->d_name;
                if (strstr(filename, ".csv") != NULL) { // Check if it's a CSV file
                    char name_without_extension[MAX_NAME_LEN];
                    strncpy(name_without_extension, filename, strlen(filename) - 4); // Remove .csv extension
                    name_without_extension[strlen(filename) - 4] = '\0';

                    if (strstr(name_without_extension, search_name) != NULL) { // Check if search_name is part of restaurant name
                        // Load restaurant data to get its info
                        Restaurant restaurant;
                        if (load_restaurant_data(folder_path, name_without_extension, &restaurant)) {
                            display_restaurant_info(&restaurant, index);
                            index++;
                        }
                    }
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
    }
}

int display_program_menu(const char *folder_path, const char *search_name, Cart *cart) {
    list_restaurants(folder_path, search_name);

    int restaurant_index;
    printf("Enter the index of the restaurant to view its menu (0 to exit): ");
    scanf("%d", &choice1);

    if (choice1 > 0) {
        int current_index = 1;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(folder_path)) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_REG) { // Check if it's a regular file
                    char *filename = ent->d_name;
                    if (strstr(filename, ".csv") != NULL) { // Check if it's a CSV file
                        char name_without_extension[MAX_NAME_LEN];
                        strncpy(name_without_extension, filename, strlen(filename) - 4); // Remove .csv extension
                        name_without_extension[strlen(filename) - 4] = '\0';

                        if (strstr(name_without_extension, search_name) != NULL) { // Check if search_name is part of restaurant name
                            if (current_index == choice1) {
                                Restaurant restaurant;
                                if (load_restaurant_data(folder_path, name_without_extension, &restaurant)) {
                                    display_menu_header(&restaurant);
                                    display_cuisines(&restaurant);
                                    
                                    int index;
                                    printf("\nEnter the number index of the cuisine to add to cart (0 to exit): ");
                                    scanf("%d", &index);

                                    while (index != 0) {
                                        if (index > 0 && index <= restaurant.cuisine_count) {
                                            add_to_cart(cart, &restaurant, index);
                                        } else {
                                            printf("Invalid index.\n");
                                        }
                                        printf("\nEnter the number index of the cuisine to add to cart (0 to exit): ");
                                        scanf("%d", &index);
                                    }
                                        
                                    show_cart(cart);
                                }
                                break;
                            }
                            current_index++;
                        }
                    }
                }
            }
            closedir(dir);
        } else {
            perror("Error opening directory");
        }

        if (choice1 != current_index) {
            printf("Invalid restaurant index.\n");
            return 1; // Return error code
        }
    } else {
        printf("Exiting program.\n");
    }

    return 0; // Return success code
}

void print_joined_paths(const char *folder_path) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(folder_path)) != NULL) {
        // Iterate through each file in the directory
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && strstr(ent->d_name, ".csv")) {
                // Construct and print the full path for CSV files
                char filename[MAX_FILENAME_LEN];
                int folder_len = strlen(folder_path);
                int name_len = strlen(ent->d_name);

                // Check if the combined length fits within MAX_FILENAME_LEN - 1
                if (folder_len + 1 + name_len < MAX_FILENAME_LEN) {
                    strcpy(filename, folder_path);
                    strcat(filename, "/");
                    strcat(filename, ent->d_name);
                    printf("%s\n", filename);
                } else {
                    fprintf(stderr, "Path too long: %s/%s\n", folder_path, ent->d_name);
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
    }
}

int run_restaurant_program(const char *folder_path, const char *search_name) {
    Cart cart = {.count = 0};

    return display_program_menu(folder_path, search_name, &cart);
}

// Utility function to calculate distance between two coordinates using Haversine formula
float calculate_distance(float lat1, float lon1, float lat2, float lon2) {
    const float R = 6371.0; // Radius of the Earth in kilometers

    float lat1_rad = lat1 * M_PI / 180.0;
    float lon1_rad = lon1 * M_PI / 180.0;
    float lat2_rad = lat2 * M_PI / 180.0;
    float lon2_rad = lon2 * M_PI / 180.0;

    float dlat = lat2_rad - lat1_rad;
    float dlon = lon2_rad - lon1_rad;

    float a = sin(dlat / 2) * sin(dlat / 2) +
              cos(lat1_rad) * cos(lat2_rad) *
              sin(dlon / 2) * sin(dlon / 2);

    float c = 2 * atan2(sqrt(a), sqrt(1 - a));

    float distance = R * c;
    return distance;
}

// Function to load restaurant data from a CSV file and calculate distance from user's coordinates
// Returns 1 if successful, 0 otherwise
int load_restaurant_data_distance(const char *folder_path, const char *restaurant_name, float user_lat, float user_lon, Restaurant *restaurant) {
    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s/%s.csv", folder_path, restaurant_name);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    // Read restaurant info (first line)
    char line[256];
    fgets(line, sizeof(line), file);
    sscanf(line, "%f,%f,%[^,],%*f", &restaurant->info.latitude, &restaurant->info.longitude, restaurant->info.address);

    // Read and skip the header line for cuisines
    fgets(line, sizeof(line), file);

    restaurant->cuisine_count = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%*d,%[^,],%f", restaurant->cuisines[restaurant->cuisine_count].name,
               &restaurant->cuisines[restaurant->cuisine_count].price);
        restaurant->cuisine_count++;
    }

    fclose(file);
    strcpy(restaurant->name, restaurant_name);

    // Calculate distance from user's coordinates
    restaurant->distance = calculate_distance(user_lat, user_lon, restaurant->info.latitude, restaurant->info.longitude);

    return 1;
}

// Function to list restaurants sorted by distance from user's location
void list_restaurants_distance(const char *folder_path, float user_lat, float user_lon, Restaurant *restaurants, int *num_restaurants) {
    DIR *dir;
    struct dirent *ent;
    *num_restaurants = 0;

    if ((dir = opendir(folder_path)) != NULL) {
        // Iterate through each CSV file in the directory
        while ((ent = readdir(dir)) != NULL && *num_restaurants < MAX_CUISINES) {
            if (ent->d_type == DT_REG && strstr(ent->d_name, ".csv")) {
                char restaurant_name[MAX_NAME_LEN];
                strncpy(restaurant_name, ent->d_name, strlen(ent->d_name) - 4); // Remove .csv extension
                restaurant_name[strlen(ent->d_name) - 4] = '\0';

                // Load restaurant data and calculate distance
                if (load_restaurant_data_distance(folder_path, restaurant_name, user_lat, user_lon, &restaurants[*num_restaurants])) {
                    (*num_restaurants)++;
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        return;
    }

    // Sort restaurants by distance (bubble sort for simplicity)
    for (int i = 0; i < *num_restaurants - 1; i++) {
        for (int j = 0; j < *num_restaurants - i - 1; j++) {
            if (restaurants[j].distance > restaurants[j + 1].distance) {
                Restaurant temp = restaurants[j];
                restaurants[j] = restaurants[j + 1];
                restaurants[j + 1] = temp;
            }
        }
    }

    // Display sorted restaurants
    printf("\n---------------------------------------------------\n");
    printf(" S No. | Restaurant      | Location             | Distance (km)\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < *num_restaurants; i++) {
        printf("  %-4d | %-15s | %-20s | %.2f\n", i + 1, restaurants[i].name, restaurants[i].info.address, restaurants[i].distance);
    }
    printf("---------------------------------------------------\n");
}

// Function to display the menu of a selected restaurant
void display_menu_distance(Restaurant *restaurant, Cart *cart) {
    printf("\n%s Menu:\n", restaurant->name);
    printf("---------------\n");
    printf(" S No. | %-10s | %-7s\n", "Cuisines", "Price");
    printf("________________________\n");
    for (int i = 0; i < restaurant->cuisine_count; i++) {
        printf("  %-4d | %-10s | Rs%.2f\n", i + 1, restaurant->cuisines[i].name, restaurant->cuisines[i].price);
    }
    printf("________________________\n");

    int index;
    printf("\nEnter the number index of the cuisine to add to cart (0 to exit): ");
    scanf("%d", &index);

    while (index != 0) {
        if (index > 0 && index <= restaurant->cuisine_count) {
            add_to_cart(cart, restaurant, index);
        } else {
            printf("Invalid index.\n");
        }
        printf("\nEnter the number index of the cuisine to add to cart (0 to exit): ");
        scanf("%d", &index);
    }
}

// Function to prompt the user to enter latitude and longitude coordinates
void get_user_coordinates_from_file(const char *filename, float *user_lat, float *user_lon) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s.\n", filename);
        return;
    }

    char line[256]; // Assuming maximum line length is 255 characters
    while (fgets(line, sizeof(line), file)) {
        // Parse the line to get latitude and longitude
        char *token;
        token = strtok(line, ",");
        // Assuming the format is fixed and the 9th and 10th fields are latitude and longitude
        int field_num = 0;
        while (token != NULL) {
            field_num++;
            if (field_num == 9) {
                *user_lat = atof(token); // Convert string to float
            }
            if (field_num == 10) {
                *user_lon = atof(token); // Convert string to float
                break; // No need to continue after getting the longitude
            }
            token = strtok(NULL, ",");
        }
    }

    fclose(file);
}

char* get_selected_restaurant_csv_path(const char *folder_path, float user_lat, float user_lon, int choice, char *selected_path) {
    Restaurant restaurants[MAX_CUISINES];
    int num_restaurants;

    list_restaurants_distance(folder_path, user_lat, user_lon, restaurants, &num_restaurants);

    if (choice < 1 || choice > num_restaurants) {
        printf("Invalid choice.\n");
        return NULL;
    }

    snprintf(selected_path, MAX_FILENAME_LEN, "%s/%s.csv", folder_path, restaurants[choice - 1].name);
    return selected_path;
}

// Function to select a restaurant and display its menu
int select_restaurant_menu_distance(const char *folder_path, float user_lat, float user_lon, Cart *cart) {
    Restaurant restaurants[MAX_CUISINES];
    int num_restaurants;
   

    list_restaurants_distance(folder_path, user_lat, user_lon, restaurants, &num_restaurants);

    printf("Enter the index of the restaurant to view its menu (0 to exit): ");
    scanf("%d", &choice1);

    if (choice1 < 1 || choice1 > num_restaurants) {
        printf("Invalid choice.\n");
        return 1;
    }
   
    get_selected_restaurant_csv_path(folder_path, user_lat, user_lon, choice1, selected_path);
    printf("Selected restaurant CSV path: %s\n", selected_path);

    display_menu_distance(&restaurants[choice1 - 1], cart);

    show_cart(cart);

    return 0;
}

void openMapInBrowser(float startLatitude, float startLongitude, float endLatitude, float endLongitude) {
    FILE *htmlFile = fopen("map.html", "w");
    if (htmlFile == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return;
    }

    fprintf(htmlFile, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(htmlFile, "    <title>Leaflet Map with Routing and Travel Time</title>\n");
    fprintf(htmlFile, "    <link rel=\"stylesheet\" href=\"https://unpkg.com/leaflet@1.7.1/dist/leaflet.css\" />\n");
    fprintf(htmlFile, "    <script src=\"https://unpkg.com/leaflet@1.7.1/dist/leaflet.js\"></script>\n");
    fprintf(htmlFile, "    <script src=\"https://unpkg.com/leaflet-routing-machine@latest/dist/leaflet-routing-machine.js\"></script>\n");
    fprintf(htmlFile, "    <style>\n");
    fprintf(htmlFile, "        #map { height: 600px; width: 100%%; }\n");
    fprintf(htmlFile, "    </style>\n");
    fprintf(htmlFile, "</head>\n<body>\n");
    fprintf(htmlFile, "    <div id=\"map\"></div>\n");
    fprintf(htmlFile, "    <div id=\"route-info\" style=\"padding: 10px;\"></div>\n");
    fprintf(htmlFile, "    <script>\n");
    fprintf(htmlFile, "        // Initialize the map\n");
    fprintf(htmlFile, "        var map = L.map('map').setView([%f, %f], 14);\n", (startLatitude + endLatitude) / 2, (startLongitude + endLongitude) / 2);
    fprintf(htmlFile, "        // Add OpenStreetMap tiles\n");
    fprintf(htmlFile, "        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {\n");
    fprintf(htmlFile, "            attribution: '&copy; <a href=\"https://www.openstreetmap.org/copyright\">OpenStreetMap</a> contributors'\n");
    fprintf(htmlFile, "        }).addTo(map);\n");
    fprintf(htmlFile, "        // Define the start (restaurant) and end (user) points\n");
    fprintf(htmlFile, "        var restaurant = [%f, %f];\n", startLatitude, startLongitude);
    fprintf(htmlFile, "        var user = [%f, %f];\n", endLatitude, endLongitude);
    fprintf(htmlFile, "        // Add markers for the restaurant and user points\n");
    fprintf(htmlFile, "        var restaurantMarker = L.marker(restaurant).addTo(map).bindPopup('Restaurant Location').openPopup();\n");
    fprintf(htmlFile, "        var userMarker = L.marker(user).addTo(map).bindPopup('Your Location').openPopup();\n");
    fprintf(htmlFile, "        // Create the routing control\n");
    fprintf(htmlFile, "        var control = L.Routing.control({\n");
    fprintf(htmlFile, "            waypoints: [\n");
    fprintf(htmlFile, "                L.latLng(restaurant[0], restaurant[1]),\n");
    fprintf(htmlFile, "                L.latLng(user[0], user[1])\n");
    fprintf(htmlFile, "            ],\n");
    fprintf(htmlFile, "            lineOptions: {\n");
    fprintf(htmlFile, "                styles: [{color: 'blue', opacity: 1, weight: 5}]\n");
    fprintf(htmlFile, "            },\n");
    fprintf(htmlFile, "            createMarker: function() { return null; }, // Disable default markers\n");
    fprintf(htmlFile, "            routeWhileDragging: true\n");
    fprintf(htmlFile, "        }).addTo(map);\n");
    fprintf(htmlFile, "        control.on('routesfound', function(e) {\n");
    fprintf(htmlFile, "            var routes = e.routes;\n");
    fprintf(htmlFile, "            var summary = routes[0].summary;\n");
    fprintf(htmlFile, "            var routeCoordinates = routes[0].coordinates;\n");
    fprintf(htmlFile, "            // Calculate duration based on constant speed (70 km/h)\n");
    fprintf(htmlFile, "            var speedKmph = 70;\n");
    fprintf(htmlFile, "            var distance = summary.totalDistance; // Distance in meters\n");
    fprintf(htmlFile, "            var durationInHours = (distance / 1000) / speedKmph; // distance converted to km\n");
    fprintf(htmlFile, "            var durationInMinutes = Math.round(durationInHours * 60);\n");
    fprintf(htmlFile, "            // Display the travel time and distance\n");
    fprintf(htmlFile, "            document.getElementById('route-info').innerHTML = `Travel Time: ${durationInMinutes} minutes<br>Distance: ${(distance / 1000).toFixed(2)} km`;\n");
    fprintf(htmlFile, "            // Simulate movement of delivery agent\n");
    fprintf(htmlFile, "            var deliveryAgentMarker = L.marker(restaurant).addTo(map);\n");
    fprintf(htmlFile, "            var index = 0;\n");
    fprintf(htmlFile, "            var delay = (durationInHours * 3600 * 1000) / routeCoordinates.length;\n");
    fprintf(htmlFile, "            function moveMarker() {\n");
    fprintf(htmlFile, "                if (index < routeCoordinates.length) {\n");
    fprintf(htmlFile, "                    deliveryAgentMarker.setLatLng([routeCoordinates[index].lat, routeCoordinates[index].lng]);\n");
    fprintf(htmlFile, "                    index++;\n");
    fprintf(htmlFile, "                    setTimeout(moveMarker, delay);\n");
    fprintf(htmlFile, "                }\n");
    fprintf(htmlFile, "            }\n");
    fprintf(htmlFile, "            moveMarker();\n");
    fprintf(htmlFile, "        });\n");
    fprintf(htmlFile, "    </script>\n");
    fprintf(htmlFile, "</body>\n</html>");

    fclose(htmlFile);

    // Stop any existing Python HTTP server running on port 8000
    system("kill $(lsof -t -i:8000)");

    // Start a Python HTTP server to serve the file
    int serverStatus = system("python3 -m http.server 8000 &");
    if (serverStatus != 0) {
        fprintf(stderr, "Error starting HTTP server.\n");
        return;
    }
    printf("HTTP server started.\n");

    // Wait for a short time to ensure server starts
    sleep(2);

    // Open the HTML file in Chrome from WSL
    int browserStatus = system("cmd.exe /C start chrome http://localhost:8000/map.html");
    if (browserStatus != 0) {
        fprintf(stderr, "Error opening browser.\n");
        return;
    }
    printf("Browser opened.\n");
}




int load_restaurant_data_rating(const char *folder_path, const char *restaurant_name, Restaurant *restaurant) {
    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s/%s.csv", folder_path, restaurant_name);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    // Read restaurant info (first line)
    char line[256];
    fgets(line, sizeof(line), file);
    sscanf(line, "%f,%f,%[^,],%*[^,],%*[^,],%f", &restaurant->info.latitude, &restaurant->info.longitude, restaurant->info.address, &restaurant->info.rating);

    // Read and skip the header line for cuisines
    fgets(line, sizeof(line), file);

    restaurant->cuisine_count = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%*d,%[^,],%f", restaurant->cuisines[restaurant->cuisine_count].name,
               &restaurant->cuisines[restaurant->cuisine_count].price);
        restaurant->cuisine_count++;
    }

    fclose(file);
    strcpy(restaurant->name, restaurant_name);

    return 1;
}


// Function to list restaurants sorted by rating
void list_restaurants_rating(const char *folder_path, Restaurant *restaurants, int *num_restaurants) {
    DIR *dir;
    struct dirent *ent;
    *num_restaurants = 0;

    if ((dir = opendir(folder_path)) != NULL) {
        // Iterate through each CSV file in the directory
        while ((ent = readdir(dir)) != NULL && *num_restaurants < MAX_CUISINES) {
            if (ent->d_type == DT_REG && strstr(ent->d_name, ".csv")) {
                char restaurant_name[MAX_NAME_LEN];
                strncpy(restaurant_name, ent->d_name, strlen(ent->d_name) - 4); // Remove .csv extension
                restaurant_name[strlen(ent->d_name) - 4] = '\0';

                // Load restaurant data
                if (load_restaurant_data_rating(folder_path, restaurant_name, &restaurants[*num_restaurants])) {
                    (*num_restaurants)++;
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        return;
    }

    // Sort restaurants by rating (bubble sort for simplicity)
    for (int i = 0; i < *num_restaurants - 1; i++) {
        for (int j = 0; j < *num_restaurants - i - 1; j++) {
            if (restaurants[j].info.rating < restaurants[j + 1].info.rating) {
                Restaurant temp = restaurants[j];
                restaurants[j] = restaurants[j + 1];
                restaurants[j + 1] = temp;
            }
        }
    }

    // Display sorted restaurants
    printf("\n---------------------------------------------------\n");
    printf(" S No. | Restaurant      | Rating\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < *num_restaurants; i++) {
        printf("  %-4d | %-15s | %.1f\n", i + 1, restaurants[i].name, restaurants[i].info.rating);
    }
    printf("---------------------------------------------------\n");
}

// Function to display the menu of a selected restaurant
void display_menu_rating(Restaurant *restaurant, Cart *cart) {
    printf("\n%s Menu:\n", restaurant->name);
    printf("---------------\n");
    printf(" S No. | %-10s | %-7s\n", "Cuisines", "Price");
    printf("________________________\n");
    for (int i = 0; i < restaurant->cuisine_count; i++) {
        printf("  %-4d | %-10s | Rs%.2f\n", i + 1, restaurant->cuisines[i].name, restaurant->cuisines[i].price);
    }
    printf("________________________\n");

    int index;
    printf("\nEnter the number index of the item to add to cart (0 to exit): ");
    scanf("%d", &index);

    while (index != 0) {
        if (index > 0 && index <= restaurant->cuisine_count) {
            add_to_cart(cart, restaurant, index);
        } else {
            printf("Invalid index.\n");
        }
        printf("\nEnter the number index of the cuisine to add to cart (0 to exit): ");
        scanf("%d", &index);
    }
}

// Function to select a restaurant and display its menu
int select_restaurant_menu_rating(const char *folder_path, Cart *cart) {
    Restaurant restaurants[MAX_CUISINES];
    int num_restaurants;

    list_restaurants_rating(folder_path, restaurants, &num_restaurants);

    printf("Enter the index of the restaurant to view its menu (0 to exit): ");
    scanf("%d", &choice1);

    if (choice1 < 1 || choice1 > num_restaurants) {
        printf("Invalid choice.\n");
        return 1;
    }

    display_menu_rating(&restaurants[choice1 - 1], cart);

    show_cart(cart);

    return 0;
}

// Function to encapsulate the restaurant ordering system
void runRestaurantOrderingSystem1(Restaurant restaurants[], int num_restaurants) {
    char food_name[50];
    int cart[MAX_DISHES] = {0}; // Cart to hold quantity of each item

    // Interactive search loop
    while (1) {
        printf("Enter food name to search (or 'exit' to quit): ");
        scanf("%49s", food_name);
        clear_input_buffer1();

        if (strcmp(food_name, "exit") == 0) {
            break; // Exit the loop if user types 'exit'
        }

        // Perform search
        search_food1(food_name, restaurants, num_restaurants, cart);
    }

    printf("Exiting restaurant ordering system. Thank you!\n");
}

// Function to read restaurant data from a CSV file
void read_restaurants_from_file1(char *filename, Restaurant restaurants[], int *num_restaurants) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    char *token;
    int index = 0;

    // Skip header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        // Parse the line into tokens
        token = strtok(line, ",");
        strcpy(restaurants[index].name, token);

        token = strtok(NULL, ",");
        strcpy(restaurants[index].city, token);

        token = strtok(NULL, ",");
        restaurants[index].latitude = atof(token);

        token = strtok(NULL, ",");
        restaurants[index].longitude = atof(token);

        token = strtok(NULL, ",");
        restaurants[index].rating = atof(token);

        restaurants[index].num_dishes = 0;
        int dish_index = 0;

        // Now parse dish and price pairs
        while ((token = strtok(NULL, ",")) != NULL) {
            if (dish_index < MAX_DISHES) {
                strcpy(restaurants[index].menu[dish_index].dish, token);

                token = strtok(NULL, ",");
                restaurants[index].menu[dish_index].price = atoi(token);

                restaurants[index].num_dishes++;
                dish_index++;
            }
        }

        index++;
    }

    *num_restaurants = index;
    fclose(file);
}

// Function to search for a food item across all restaurants
void search_food1(char *food_name, Restaurant restaurants[], int num_restaurants, int cart[]) {
    int i, j;
    int found = 0;

    printf("\nSearch results for '%s':\n", food_name);
    printf("Index | Restaurant | Price | Restaurant Rating\n");

    for (i = 0; i < num_restaurants; i++) {
        for (j = 0; j < restaurants[i].num_dishes; j++) {
            if (strcmp(food_name, restaurants[i].menu[j].dish) == 0) {
                printf("%d. %s, $%d, %.1f stars\n", j + 1, restaurants[i].name,
                       restaurants[i].menu[j].price, restaurants[i].rating);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No results found for '%s'\n", food_name);
        return;
    }

    int choice;
    printf("\nEnter index to add to cart (0 to exit): ");
    scanf("%d", &choice);
    clear_input_buffer1();

    if (choice < 1 || choice > num_restaurants) {
        printf("Invalid choice.\n");
        return;
    }

    // Prompt user for action: Proceed to Pay or Explore Menu
    printf("\nOptions:\n");
    printf("1. Proceed to Pay\n");
    printf("2. Explore Menu of Restaurant\n");
    printf("Enter your choice: ");

    int option;
    scanf("%d", &option);
    clear_input_buffer1();

    switch (option) {
        case 1:
            cart[choice - 1]++;
            show_cart1(cart, restaurants, num_restaurants);
            break;
        case 2:
            display_restaurant_menu1(restaurants[choice - 1], cart);
            break;
        default:
            printf("Invalid option.\n");
            break;
    }
}

// Function to display the restaurant menu for exploration
void display_restaurant_menu1(Restaurant restaurant, int cart[]) {
    printf("\nRestaurant: %s\n", restaurant.name);
    printf("Menu:\n");
    printf("Index | Item | Price\n");

    int choice;
    while (1) {
        for (int i = 0; i < restaurant.num_dishes; i++) {
            printf("%d. %s, $%d\n", i + 1, restaurant.menu[i].dish, restaurant.menu[i].price);
        }

        printf("\nEnter index to add to cart (0 to exit): ");
        scanf("%d", &choice);
        clear_input_buffer1();

        if (choice == 0) {
            break;
        }

        if (choice < 1 || choice > restaurant.num_dishes) {
            printf("Invalid choice.\n");
            continue;
        }

        cart[choice - 1]++;
    }

    show_cart1(cart, &restaurant, 1);
}

// Function to display the contents of the cart
void show_cart1(int cart[], Restaurant restaurants[], int num_restaurants) {
    printf("\nCurrent Cart Contents:\n");
    printf("Index | Restaurant | Item | Quantity | Price\n");

    int grand_total = 0;
    for (int i = 0; i < num_restaurants; i++) {
        for (int j = 0; j < restaurants[i].num_dishes; j++) {
            if (cart[j] > 0) {
                printf("%d. %s, %s, %d, $%d\n", j + 1, restaurants[i].name,
                       restaurants[i].menu[j].dish, cart[j], restaurants[i].menu[j].price * cart[j]);
                grand_total += restaurants[i].menu[j].price * cart[j];
            }
        }
    }

    printf("\nGrand Total: $%d\n", grand_total);

    // Options
    printf("\nOptions:\n");
    printf("1. Proceed to Pay\n");
    printf("2. Delete an Item from Cart\n");
    printf("3. Exit\n");

    int option;
    printf("Enter your choice: ");
    scanf("%d", &option);
    clear_input_buffer1();

    switch (option) {
        case 1:
            proceed_to_pay1(cart, restaurants, num_restaurants);
            break;
        case 2:
            delete_item_from_cart1(cart, &num_restaurants);
            break;
        case 3:
            break; // Exit
        default:
            printf("Invalid option.\n");
            break;
    }
}

// Function to proceed to pay (dummy function)
void proceed_to_pay1(int cart[], Restaurant restaurants[], int num_restaurants) {
    printf("\nProceeding to payment...\n");
    // Implement payment logic here (not implemented in this basic example)
    int payment_choice;
    int after_payment_choice;

    printf("Select Payment Method:\n");
    printf("1. Cash\n");
    printf("2. Card\n");
    printf("3. UPI\n");
    printf("4. Wallet");
    printf("Enter your choice (1/2/3): ");
    scanf("%d", &payment_choice);

    switch (payment_choice) {
        case 1:
            printf("You have selected to pay with Cash.\n");
            printf("Your order is confirmed.\n");
            break;
        case 2:
            printf("You have selected to pay with Card.\n");
            payWithCard();
            break;
        case 3:
            printf("You have selected to pay with UPI.\n");
            payWithUPI();
            break;
        case 4:
            printf("You have selected to pay with UPI.\n");
            paybyWallet();
            default:
            printf("Invalid choice! Please select 1, 2, 3 or 4.\n");
            break;
    }

    printf("\nAdditional Options:\n");
    printf("1. Give your feedback\n");
    printf("2. Return to main menu\n");
    printf("Enter your choice: ");
    scanf("%d", &after_payment_choice);

    switch (after_payment_choice) {
        case 1:
            //feedback
            Restaurant restaurant = {.feedbackCount = 0};
            Customer customer = {.feedbackCount = 0};
            runFeedbackSystem(&restaurant, &customer);
            break;
        case 2:
            printf("Returning to previous menu...\n");
            return;  // Exit the function, returning control to main()
        default:
            printf("Invalid choice! Returning to previous menu...\n");
            return;  // Exit the function, returning control to main()1
    }
}

// Function to delete an item from the cart
void delete_item_from_cart1(int cart[], int *num_items) {
    int index;

    printf("\nEnter index of item to delete: ");
    scanf("%d", &index);
    clear_input_buffer1();

    if (index < 1 || index > *num_items) {
        printf("Invalid index.\n");
        return;
    }

    // Remove item from cart
    cart[index - 1] = 0;
    printf("Item deleted from cart.\n");
}

// Function to clear input buffer (flush remaining characters)
void clear_input_buffer1() {
    while (getchar() != '\n');
}

void addFeedbackToFile(FILE* file, int rating, char* feedback) {
    fprintf(file, "Rating: %d\n", rating);
    fprintf(file, "Feedback: %s\n", feedback);
    fprintf(file, "\n");
}

void addFeedback(Restaurant* restaurant, Customer* customer, int rating, char* feedback) {
    if (restaurant->feedbackCount < MAX_FEEDBACKS && customer->feedbackCount < MAX_FEEDBACKS) {
        restaurant->ratings[restaurant->feedbackCount] = rating;
        snprintf(restaurant->feedback[restaurant->feedbackCount], 256, "%s", feedback);
        restaurant->feedbackCount++;

        customer->ratings[customer->feedbackCount] = rating;
        snprintf(customer->feedback[customer->feedbackCount], 256, "%s", feedback);
        customer->feedbackCount++;

        // Open file for appending feedback
        FILE* file = fopen("feedback.txt", "a");
        if (file != NULL) {
            addFeedbackToFile(file, rating, feedback);
            fclose(file);
        } else {
            printf("Error opening file for writing feedback.\n");
        }
    } else {
        printf("Maximum feedback limit reached.\n");
    }
}

float calculateAverageRating(Restaurant* restaurant) {
    int sum = 0;
    for (int i = 0; i < restaurant->feedbackCount; i++) {
        sum += restaurant->ratings[i];
    }
    return (float)sum / restaurant->feedbackCount;
}

void promptFeedback(Restaurant* restaurant, Customer* customer) {
    int rating;
    char feedback[256];

    printf("Enter your rating (1-5): ");
    scanf("%d", &rating);
    getchar();  // Clear newline character from buffer

    if (rating < 1 || rating > 5) {
        printf("Invalid rating. Please enter a rating between 1 and 5.\n");
        return;
    }

    printf("Enter your feedback: ");
    fgets(feedback, 256, stdin);

    addFeedback(restaurant, customer, rating, feedback);
    float averageRating = calculateAverageRating(restaurant);
    printf("Thank you for your feedback!\n");
    printf("Current average rating for the restaurant: %.2f\n", averageRating);
}

void runFeedbackSystem(Restaurant* restaurant, Customer* customer) {
    int continueFeedback = 1;

    while (continueFeedback) {
        promptFeedback(restaurant, customer);

        printf("Do you want to give more feedback? (1 for Yes, 0 for No): ");
        scanf("%d", &continueFeedback);
        getchar();  // Clear newline character from buffer
    }

    printf("Thank you for using our service, Order again! We are here to fill your apetite!!!!!!\n");
}

void paybyWallet() {
    int remaining = WALLET_MONEY;
    printf("Available Balance: %d\n", remaining);
    
    if (grand_total>WALLET_MONEY) {
        printf("Not sufficient balance.");
    } else {
        remaining=WALLET_MONEY-grand_total;
        printf("Successfully paid!!\n");
        printf("Remaining balance in wallet: %d", remaining);
    }
}

int main() {
    srand(time(0)); // Seed the random number generator
    Cart cart1 = {.count = 0};

    int choice;
    bool isLoggedIn = false;

    // User authentication loop
    do {
        printf("\n1. Login\n");
        printf("2. Create new user\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                isLoggedIn = login();
                if (!isLoggedIn) {
                    printf("Login failed. Please try again.\n");
                }
                break;
            case 2:
                createUser();
                break;
            case 3:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (!isLoggedIn);

    // Main menu loop
    do {
        printf("\nWelcome to the Food Ordering System\n");
        printf("1. Nearest Restaurants to you\n");
        printf("2. Top Rated Restaurants\n");
        printf("3. Search by Food item name\n");
        printf("4. Search by Restaurant name\n");
        printf("5. Cart\n");
        printf("6. Log Out\n");
        printf("Enter your choice (1-6): ");
        scanf("%d", &choice);

        switch (choice) {
            float user_lat, user_lon;
            
            case 1:
                get_user_coordinates_from_file("users.csv", &user_lat, &user_lon);
                select_restaurant_menu_distance(FOLDER_PATH, user_lat, user_lon, &cart1);
                break;
            case 2:
                select_restaurant_menu_rating(FOLDER_PATH, &cart1);
                break;
            case 3:
                Restaurant restaurants[MAX_RESTAURANTS];
                int num_restaurants = 0;
                {
                    read_restaurants_from_file1("food.csv", restaurants, &num_restaurants);
                    runRestaurantOrderingSystem1(restaurants, num_restaurants);
                }
                break;
            case 4:
            {
                char rest_name[256];
                printf("Enter part of the Restaurant name to search: ");
                scanf(" %[^\n]", rest_name);
                run_restaurant_program(FOLDER_PATH, rest_name);
                break;
            }
            case 5:
                
                show_cart(&cart1);
                break;
            case 6:
                printf("Logged out.\n");
                isLoggedIn = false; // Reset login status
                break;
            default:
                printf("Invalid choice! Please enter a number between 1 and 6.\n");
                break;
        }
    } while (choice != 6);

    return 0;
}
