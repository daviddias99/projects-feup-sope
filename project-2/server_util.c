#include "server_util.h"

bool passwordIsValid(char* password){

    int passwordLength = strlen(password);

    if(passwordLength > MAX_PASSWORD_LEN)
        return false;
    
    if(passwordLength < MIN_PASSWORD_LEN)
        return false;

    return true;
}


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

    for(int i = 0; i < SALT_LEN;i++)
        saltStr[i] = getHexChar(randomBetween(0,15));

    saltStr[SALT_LEN] = '\0';
}

bank_account_t createAdminBankAccount(char* password){

    return createBankAccount(0,password,0);

}

bank_account_t createBankAccount(uint32_t id, char* password, uint32_t balance) {

    bank_account_t newBankAccount;

    newBankAccount.account_id = id;
    newBankAccount.balance = balance;
    /**TODO: criar utilitario para efetuar o SHA256SUM da password **/
    generateSalt(newBankAccount.salt);

    return newBankAccount;
}
