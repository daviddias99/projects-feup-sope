#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

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


typedef struct bank_account_array{

    bank_account_t bank_accounts_array[MAX_BANK_ACCOUNTS];
    size_t next_account_index;

} bank_account_array_t;

bank_account_t createBankAccount(uint32_t id, char* password, uint32_t balance);
bank_account_t createAdminBankAccount(char* password);

bool passwordIsValid(char* password);
int randomBetween(int a, int b);
char getHexChar(unsigned int a);
int generateSalt(char* saltStr);


#endif