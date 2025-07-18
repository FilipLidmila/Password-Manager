#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <ctype.h>

USER user;
static PASS* array_of_passwords = NULL;
int number_of_pass = 0;

// Stvara novu praznu datoteku – npr. za lozinke pojedinog korisnika
void createfile(const char* const filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Greška pri kreiranju datoteke");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

// Stvara ili resetira datoteku s popisom korisnika
void create_users_file(const char* const file_users) {
    FILE* file = fopen(file_users, "w");
    if (file == NULL) {
        perror("Greška pri kreiranju datoteke korisnika");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

// Dodaje novog korisnika – provjera unosa, validacija, enkripcija lozinke i spremanje
void new_user(int* const num_users, USER* users, const char* const file_users) {
    if (*num_users >= MAX_USERS) {
        printf("Dosegnut je maksimalni broj korisnika.\n");
        return;
    }

    char new_username[MAX_NAME_LENGTH];
    int i;

    // Unos korisničkog imena bez razmaka i samo s alfabetnim znakovima
    do {
        printf("Unesite korisničko ime (bez razmaka): ");
        scanf(" %49[^\n]", new_username);
        while (getchar() != '\n');
        if (strchr(new_username, ' ') != NULL || !is_alpha_string(new_username)) {
            printf("Neispravno korisničko ime. Pokušajte ponovno.\n");
        }
        else {
            break;
        }
    } while (1);

    // Provjera postoji li korisnik
    for (i = 0; i < *num_users; i++) {
        if (strcmp(new_username, users[i].name) == 0) {
            printf("Korisnik već postoji.\n");
            return;
        }
    }

    // Alokacija memorije za korisnika
    users[*num_users].name = _strdup(new_username);
    users[*num_users].password = (char*)malloc(MAX_PASS_LENGTH);
    if (users[*num_users].password == NULL) {
        perror("Greška pri alokaciji memorije za lozinku");
        exit(EXIT_FAILURE);
    }
    memset(users[*num_users].password, 0, MAX_PASS_LENGTH);

    // Unos lozinke – bez razmaka, s validacijom znakova
    do {
        printf("Unesite lozinku (bez razmaka): ");
        scanf(" %29[^\n]", users[*num_users].password);
        while (getchar() != '\n');

        if (strchr(users[*num_users].password, ' ') != NULL || !validatePlaintext(users[*num_users].password)) {
            printf("Neispravna lozinka. Pokušajte ponovno.\n");
        }
        else {
            break;
        }
    } while (1);

    users[*num_users].num_passwords = 0;
    encryptXOR(users[*num_users].password); // Enkripcija lozinke

    // Spremanje korisnika u datoteku
    FILE* file = fopen(file_users, "a");
    if (file == NULL) {
        perror("Greška pri otvaranju datoteke korisnika");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s %s\n", users[*num_users].name, users[*num_users].password);
    fclose(file);

    (*num_users)++;
    printf("Korisnik uspješno dodan.\n");
}

// Učitava sve korisnike iz datoteke – koristi se pri pokretanju programa
USER* load_users(const char* file_users, int* num_users) {
    FILE* file = fopen(file_users, "r");
    if (!file) {
        perror("Greška pri otvaranju datoteke korisnika");
        exit(EXIT_FAILURE);
    }

    USER* users = malloc(MAX_USERS * sizeof(USER));
    *num_users = 0;
    char name[MAX_NAME_LENGTH], password[MAX_PASS_LENGTH];

    while (fscanf(file, "%s %s", name, password) == 2) {
        users[*num_users].name = _strdup(name);
        users[*num_users].password = _strdup(password);
        users[*num_users].num_passwords = 0;
        (*num_users)++;
    }

    fclose(file);
    return users;
}

// Omogućuje prijavu korisnika – provjera korisničkog imena i lozinke, pristup funkcijama lozinki
void login_user(int* const num_users, USER* users) {
    char username[MAX_NAME_LENGTH], password[MAX_PASS_LENGTH];

    printf("Unesite korisničko ime: ");
    scanf(" %49[^\n]", username);
    printf("Unesite lozinku: ");
    scanf(" %29[^\n]", password);

    for (int i = 0; i < *num_users; i++) {
        decryptXOR(users[i].password);
        if (strcmp(users[i].name, username) == 0 && strcmp(users[i].password, password) == 0) {
            printf("Uspješna prijava.\n");
            encryptXOR(users[i].password);

            char filename[MAX_FILENAME_LENGTH];
            snprintf(filename, sizeof(filename), "%s.txt", username);

            // Ako datoteka ne postoji, stvori novu
            if (_access(filename, 0) != 0) {
                createfile(filename);
                printf("Datoteka %s je kreirana.\n", filename);
            }

            // Izbornik funkcionalnosti lozinki
            while (1) {
                int izbor;
                printf("\nOdaberite opciju:\n1. Dodaj lozinku\n2. Ispiši lozinke\n3. Izbriši lozinku\n4. Promijeni lozinku\n5. Pretraži lozinku\n6. Ispiši abecedno\n7. Odjava\nOdabir: ");
                scanf("%d", &izbor);
                getchar();

                switch (izbor) {
                case 1: add_password(filename, &users[i]); break;
                case 2: writeout_passwords(filename, &users[i]); break;
                case 3: delete_password(filename, &users[i]); break;
                case 4: change_password(filename, &users[i]); break;
                case 5: search_password(filename); break;
                case 6: abc_print(filename, &users[i]); break;
                case 7: printf("Odjavljeni ste.\n"); return;
                default: printf("Neispravan odabir.\n");
                }
            }
        }
        encryptXOR(users[i].password);
    }

    printf("Pogrešno korisničko ime ili lozinka.\n");
}

// Briše korisnika – uklanja njegovu datoteku i oslobađa memoriju
void delete_user(int* const num_users, USER* users) {
    char username[MAX_NAME_LENGTH];
    printf("Unesite korisničko ime za brisanje: ");
    scanf("%s", username);

    for (int i = 0; i < *num_users; i++) {
        if (strcmp(users[i].name, username) == 0) {
            char filename[MAX_FILENAME_LENGTH];
            snprintf(filename, sizeof(filename), "%s.txt", username);
            remove(filename);
            free(users[i].name);
            free(users[i].password);

            // Pomicanje elemenata u nizu nakon brisanja
            for (int j = i; j < *num_users - 1; j++) {
                users[j] = users[j + 1];
            }

            (*num_users)--;
            printf("Korisnik %s je izbrisan.\n", username);
            return;
        }
    }

    printf("Korisnik nije pronađen.\n");
}

// Ispisuje sve korisnike iz memorije
void display_users(int* const num_users, USER* users) {
    printf("Popis korisnika:\n");
    for (int i = 0; i < *num_users; i++) {
        printf("%s\n", users[i].name);
    }
}

// Oslobađa memoriju svih korisnika i resetira broj
void free_users(int* const num_users, USER* users) {
    for (int i = 0; i < *num_users; i++) {
        free(users[i].name);
        free(users[i].password);
    }
    *num_users = 0;
}

// Pita korisnika za potvrdu i ako potvrdi, poziva free i izlazi iz programa
void exit_manager(int* const num_users, USER* users) {
    char odgovor;
    printf("Želite li izaći iz aplikacije? (D/N): ");
    scanf(" %c", &odgovor);
    if (odgovor == 'D' || odgovor == 'd') {
        free_users(num_users, users);
        exit(0);
    }
}

// Pomoćna funkcija – provjerava da je string sastavljen samo od slova
inline bool is_alpha_string(const char* str) {
    while (*str) {
        if (!isalpha(*str)) return false;
        str++;
    }
    return true;
}
