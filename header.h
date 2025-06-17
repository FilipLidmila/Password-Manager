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

void createfile(const char* const filename);
void create_users_file(const char* const file_users);
void new_user(int* const num_users, USER* users, const char* const file_users);
void login_user(int* const num_users, USER* users);
void delete_user(int* const num_users, USER* users);
void display_users(int* const num_users, USER* users);
void exit_manager(int* const num_users, USER* users);
void add_password(const char* filename, USER* user);
PASS* update_password(const char* const filename, int* p_number_of_pass);
void writeout_passwords(const char* filename, const USER* user);
void delete_password(const char* filename, USER* user);
void replace_spaces_with_underscores(char* str);
void replace_underscores_with_spaces(char* str);
void decryptXOR(char* password);
void encryptXOR(char* password);
void search_password(const char* filename);
bool validatePlaintext(const char* plaintext);
bool is_allowed(char c);
void abc_print(const char* filename, USER* user);
USER* load_users(const char* file_users, int* num_users);
void free_users(int* const num_users, USER* users);
int count_lines_in_file(const char* filename);
void change_password(const char* filename, USER* user);
static inline bool is_alpha_string(const char* str);
