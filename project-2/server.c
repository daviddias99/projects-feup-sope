
#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "stdbool.h"
#include "constants.h"
#include "types.h"
#include "string.h"
#include "time.h"

#define READ 0
#define WRITE 1
#define SHA256_SIZE                   64

bool passwordIsValid(char* password){

    int passwordLength = strlen(password);

    if(passwordLength > MAX_PASSWORD_LEN)
        return false;
    
    if(passwordLength < MIN_PASSWORD_LEN)
        return false;

    return true;
}

bank_account_t bank_accounts_array[MAX_BANK_ACCOUNTS];

int randomBetween(int a, int b){

    return rand()%(b-a) + a;
}

char getHexChar(unsigned int a){

    if(a > 15)
        return '\0';
    
    switch (a)
    {
    case 10:
        return 'a';   
    case 11:
        return 'b';
    case 12:
        return 'c';
    case 13:
        return 'd';
    case 14:
        return 'e';
    case 15:
        return 'f';
    default:
        return a +'0';
    }

}

int generateSalt(char* saltStr){

    for(int i = 0; i < SALT_LEN;i++){

        saltStr[i] = getHexChar(randomBetween(0,15));
    }

    saltStr[SALT_LEN] = '\0';
}

int createAdminBankAccount(char* password){

    bank_account_t adminBankAccount;

    adminBankAccount.account_id = 0;
    adminBankAccount.balance = 0;
    generateSalt(adminBankAccount.salt);

}


int main(int argc, char* argv[]){

    srand(time(NULL));

    if(argc != 3){

        fprintf( stderr, "Usage: %s <bank_office_count> <admin_password>\n", argv[0]);
        exit(-1);
        
    }

    int bankOfficeCount = strtol(argv[1],NULL,10);
    char* adminPassword = argv[2];

    if(!passwordIsValid(adminPassword)){

        fprintf( stderr, "Invalid password\n");
        exit(-2);
    }

    createAdminBankAccount(adminPassword);


    return 0;
}