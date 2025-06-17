#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>

#define MAX_USERS 5
#define MAX_PASSWORDS 10
#define MAX_NAME_LENGTH 50
#define MAX_PASS_LENGTH 30
#define MAX_FILENAME_LENGTH (MAX_NAME_LENGTH + 5)

#define XOR_KEY 0xAA

static char file_users[MAX_FILENAME_LENGTH] = "users_all.txt";
extern int number_of_pass;

typedef struct {
    char name_of_use[MAX_NAME_LENGTH];
    char password_stored[MAX_PASS_LENGTH];
} PASS;

typedef struct {
    char* name;
    char* password;
    int num_passwords;
    PASS passwords[MAX_PASSWORDS];
} USER;

enum Choice1 {
    NEW_USER = 1,
    LOGIN_USER,
    DELETE_USER,
    DISPLAY_USER,
    EXIT_MANAGER
};

enum Choice2 {
    ADD_PASSWORD = 1, // snake case 
    WRITEOUT_PASSWORDS,
    DELETE_PASSWORD,
    CHANGE_PASSWORD,
    SEARCH_PASSWORD,
    ABC_PRINT,
    LOG_OUT
};

// Deklaracije funkcija:
void createfile(const char* const filename); // Stvara praznu datoteku ako ne postoji
void create_users_file(const char* const file_users); // Inicijalizira datoteku s korisnicima
void new_user(int* const num_users, USER* users, const char* const file_users); // Dodaje novog korisnika
void login_user(int* const num_users, USER* users); // Prijava korisnika
void delete_user(int* const num_users, USER* users); // Brisanje korisnika
void display_users(int* const num_users, USER* users); // Prikaz svih korisnika
void exit_manager(int* const num_users, USER* users); // Izlaz i čišćenje memorije
void add_password(const char* filename, USER* user); // Dodavanje nove lozinke korisniku
PASS* update_password(const char* const filename, int* p_number_of_pass); // Učitavanje lozinki iz datoteke
void writeout_passwords(const char* filename, const USER* user); // Ispis svih lozinki
void delete_password(const char* filename, USER* user); // Brisanje određene lozinke
void replace_spaces_with_underscores(char* str); // Zamjena razmaka s podvlakama u stringu
void replace_underscores_with_spaces(char* str); // Zamjena podvlaka s razmacima
void decryptXOR(char* password); // Dekriptiranje lozinke pomoću XOR
void encryptXOR(char* password); // Enkriptiranje lozinke pomoću XOR
void search_password(const char* filename); // Pretraživanje lozinke po imenu
bool validatePlaintext(const char* plaintext); // Provjera valjanosti običnog teksta
bool is_allowed(char c); // Provjera je li znak dozvoljen
void abc_print(const char* filename, USER* user); // Ispis lozinki abecednim redom
USER* load_users(const char* file_users, int* num_users); // Učitavanje korisnika iz datoteke
void free_users(int* const num_users, USER* users); // Oslobađanje zauzete memorije korisnika
int count_lines_in_file(const char* filename); // Broji redove u datoteci
void change_password(const char* filename, USER* user); // Promjena lozinke
static inline bool is_alpha_string(const char* str); // Provjerava je li string sastavljen samo od slova
