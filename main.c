#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include"header.h"
#include <ctype.h>

int main(void) {

    static enum Choice1 choice1;
    int broj_korisnika = 0;
    USER* korisnici = NULL;

    char unos[100];

    if (_access(file_users, 0) == 0) {
        printf("Datoteka %s već postoji.\n", file_users);
        korisnici = load_users(file_users, &broj_korisnika);
    }
    else {
        create_users_file(file_users);
        printf("Datoteka %s uspješno kreirana.\n", file_users);
        korisnici = malloc(sizeof(USER) * MAX_USERS);
        if (korisnici == NULL) {
            perror("Neuspjela alokacija memorije");
            exit(EXIT_FAILURE);
        }
    }
    system("pause");

    while (1) {
        system("cls");
        printf("Odaberite broj izbornika:\n");
        printf("1. Novi korisnik\n");
        printf("2. Prijava postojećeg korisnika\n");
        printf("3. Brisanje korisnika\n");
        printf("4. Prikaz svih korisnika\n");
        printf("5. Izlaz iz programa\n");

        if (fgets(unos, sizeof(unos), stdin)) {
            char* endptr;
            choice1 = strtol(unos, &endptr, 10);

            if (endptr != unos) {
                while (isspace((unsigned char)*endptr)) endptr++;
                if (*endptr == '\0' || *endptr == '\n') {
                    switch (choice1) {
                    case NEW_USER: new_user(&broj_korisnika, korisnici, file_users); break;
                    case LOGIN_USER: login_user(&broj_korisnika, korisnici); break;
                    case DELETE_USER: delete_user(&broj_korisnika, korisnici); break;
                    case DISPLAY_USER: display_users(&broj_korisnika, korisnici); break;
                    case EXIT_MANAGER: exit_manager(&broj_korisnika, korisnici); break;
                    default:
                        printf("Neispravan izbor. Unesite broj od 1 do 5.\n");
                        break;
                    }
                    system("pause");
                }
                else {
                    printf("Neispravan unos. Pokušajte ponovo.\n");
                    system("pause");
                }
            }
        }
        else {
            printf("Greška u unosu. Pokušajte ponovno.\n");
            system("pause");
        }
    }
    return 0;
}
