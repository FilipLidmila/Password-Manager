#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   // za strcpy i strcmp --> sortiranje po abecedi, kod funkcije abc_print i za kopiranje stringova
#include "header.h"   
#include <stdbool.h>  // za bool tip podataka jasno pokazuje da varijabla predstavlja logičku vrijednost – true ili false, a ne neki slučajni broj

// Zamjenjuje razmake u stringu podvlakama (za spremanje u datoteku)
void replace_spaces_with_underscores(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') str[i] = '_';
    }
}

// Zamjenjuje podvlake natrag u razmake (za ispis korisniku)
void replace_underscores_with_spaces(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '_') str[i] = ' ';
    }
}

// Dodaje novu lozinku korisniku – provjerava duplikate, validira unos, enkriptira i sprema
void add_password(const char* filename, USER* user) {
    if (user->num_passwords >= MAX_PASSWORDS) {
        printf("Dosegnut je maksimalan broj spremljenih lozinki.\n");
        return;
    }

    printf("Unesite naziv za što se lozinka koristi: ");
    scanf(" %49[^\n]", user->passwords[user->num_passwords].name_of_use);
    replace_spaces_with_underscores(user->passwords[user->num_passwords].name_of_use);
    getchar();

    for (int i = 0; i < user->num_passwords; i++) {
        if (strcmp(user->passwords[user->num_passwords].name_of_use, user->passwords[i].name_of_use) == 0) {
            printf("Naziv već postoji. Odaberite drugi.\n");
            return;
        }
    }

    do {
        printf("Unesite lozinku: ");
        scanf(" %29[^\n]", user->passwords[user->num_passwords].password_stored);
        getchar();

        if (strchr(user->passwords[user->num_passwords].password_stored, ' ') != NULL ||
            !validatePlaintext(user->passwords[user->num_passwords].password_stored)) {
            printf("Neispravna lozinka. Pokušajte ponovno.\n");
        }
        else {
            break;
        }
    } while (1);

    encryptXOR(user->passwords[user->num_passwords].password_stored);

    FILE* file = fopen(filename, "a");
    if (!file) {
        perror("Greška pri otvaranju datoteke");
        return;
    }

    fprintf(file, "%s %s\n",
        user->passwords[user->num_passwords].name_of_use,
        user->passwords[user->num_passwords].password_stored);

    fclose(file);
    user->num_passwords++;
}

// Ispisuje sve spremljene lozinke iz datoteke (dekriptirane za prikaz)
void writeout_passwords(const char* filename, const USER* user) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Greška pri otvaranju datoteke");
        return;
    }

    long pozicija = ftell(file);
    printf("[INFO] Trenutna pozicija u datoteci: %ld bajtova od početka.\n", pozicija);

    printf("Spremljene lozinke:\n");

    PASS pass;
    while (fscanf(file, "%s %s", pass.name_of_use, pass.password_stored) == 2) {
        replace_underscores_with_spaces(pass.name_of_use);
        decryptXOR(pass.password_stored);
        printf("Za: %s | Lozinka: %s\n", pass.name_of_use, pass.password_stored);
    }

    rewind(file); // vraćanje na početak datoteke (nije obavezno ali korisno za debug info)
    printf("[INFO] Nakon rewind(), vraćeni smo na početak datoteke.\n");

    fclose(file);
}

// Briše lozinku po nazivu – pita korisnika za potvrdu, koristi privremenu datoteku
void delete_password(const char* filename, USER* user) {
    char usage[MAX_NAME_LENGTH];
    printf("Unesite naziv lozinke koju želite izbrisati: ");
    scanf(" %49[^\n]", usage);
    getchar();

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Greška pri otvaranju datoteke");
        return;
    }

    FILE* temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("Greška pri kreiranju privremene datoteke");
        fclose(file);
        return;
    }

    PASS pass;
    int found = 0;
    while (fscanf(file, "%s %s", pass.name_of_use, pass.password_stored) == 2) {
        replace_underscores_with_spaces(pass.name_of_use);
        if (strcmp(pass.name_of_use, usage) == 0) {
            found = 1;
            decryptXOR(pass.password_stored);
            printf("Nađena lozinka: %s\n", pass.password_stored);
            printf("Potvrdite brisanje (D/N): ");
            char odg;
            scanf(" %c", &odg);
            getchar();
            if (odg == 'D' || odg == 'd') continue;
        }
        replace_spaces_with_underscores(pass.name_of_use);
        fprintf(temp, "%s %s\n", pass.name_of_use, pass.password_stored);
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);

    if (found) printf("Lozinka je izbrisana.\n");
    else printf("Lozinka nije pronađena.\n");
}

// Mijenja postojeću lozinku – traži unos nove i sprema enkriptiranu vrijednost
void change_password(const char* filename, USER* user) {
    char usage[MAX_NAME_LENGTH];
    printf("Unesite naziv lozinke za promjenu: ");
    scanf(" %49[^\n]", usage);
    getchar();

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Greška pri otvaranju datoteke");
        return;
    }

    FILE* temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("Greška pri kreiranju privremene datoteke");
        fclose(file);
        return;
    }

    PASS pass;
    int found = 0;

    while (fscanf(file, "%s %s", pass.name_of_use, pass.password_stored) == 2) {
        replace_underscores_with_spaces(pass.name_of_use);
        decryptXOR(pass.password_stored);

        if (strcmp(pass.name_of_use, usage) == 0) {
            printf("Nađena lozinka: %s\n", pass.password_stored);
            printf("Unesite novu lozinku: ");
            scanf(" %29[^\n]", pass.password_stored);
            getchar();
            encryptXOR(pass.password_stored);
            found = 1;
        }
        else {
            encryptXOR(pass.password_stored);
        }

        replace_spaces_with_underscores(pass.name_of_use);
        fprintf(temp, "%s %s\n", pass.name_of_use, pass.password_stored);
    }

    fclose(file);
    fclose(temp);
    remove(filename);
    rename("temp.txt", filename);

    if (found) printf("Lozinka je promijenjena.\n");
    else printf("Lozinka nije pronađena.\n");
}

// Traži lozinku po imenu i prikazuje dekriptiranu ako postoji
void search_password(const char* filename) {
    char name[MAX_NAME_LENGTH];
    printf("Unesite naziv lozinke za pretragu: ");
    scanf(" %49[^\n]", name);
    getchar();

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Greška pri otvaranju datoteke");
        return;
    }

    PASS pass;
    int found = 0;

    while (fscanf(file, "%s %s", pass.name_of_use, pass.password_stored) == 2) {
        replace_underscores_with_spaces(pass.name_of_use);
        if (strcmp(pass.name_of_use, name) == 0) {
            decryptXOR(pass.password_stored);
            printf("Lozinka za '%s' je: %s\n", pass.name_of_use, pass.password_stored);
            found = 1;
            break;
        }
    }

    if (!found) printf("Lozinka nije pronađena.\n");
    fclose(file);
}

// Enkriptira lozinku pomoću XOR algoritma
void encryptXOR(char* password) {
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] ^= XOR_KEY; // XOR enkripcija sa statičkim ključem
    }
}

// Dekriptira lozinku – zapravo isti algoritam kao i enkripcija
void decryptXOR(char* password) {
    encryptXOR(password); // XOR je reverzibilan – isto funkcija radi i obrnuto
}

// Provjerava je li znak dozvoljen u lozinki
bool is_allowed(char c) {
    const char* allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 !#$%&/()=?*'";
    return strchr(allowedChars, c) != NULL;
}

// Provjerava je li cijeli string ispravan (sadrži samo dozvoljene znakove)
bool validatePlaintext(const char* plaintext) {
    for (int i = 0; plaintext[i] != '\0'; i++) {
        if (!is_allowed(plaintext[i])) return false;
    }
    return true;
}

// Funkcija za usporedbu imena lozinki (za sortiranje)
int compare_names(const void* a, const void* b) {
    const PASS* pa = (const PASS*)a;
    const PASS* pb = (const PASS*)b;
    return strcmp(pa->name_of_use, pb->name_of_use);
}

// Sortira i ispisuje sve lozinke po abecednom redu
void abc_print(const char* filename, USER* user) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Greška pri otvaranju datoteke");
        return;
    }

    PASS temp[MAX_PASSWORDS];
    int count = 0;

    while (fscanf(file, "%s %s", temp[count].name_of_use, temp[count].password_stored) == 2) {
        decryptXOR(temp[count].password_stored);
        replace_underscores_with_spaces(temp[count].name_of_use);
        count++;
    }
    fclose(file);

    qsort(temp, count, sizeof(PASS), compare_names); // Sortira po imenu (abecedno)

    for (int i = 0; i < count; i++) {
        printf("Za: %s | Lozinka: %s\n", temp[i].name_of_use, temp[i].password_stored);
    }
}
