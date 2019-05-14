#ifndef SERVER_ACC_MANAGING_H
#define SERVER_ACC_MANAGING_H

#include "sope.h"
#include "constants.h"
#include "types.h"
#include "server_util.h"

bank_account_t accounts[MAX_BANK_ACCOUNTS];
pthread_mutex_t account_mutex[MAX_BANK_ACCOUNTS];

bank_account_t createBankAccount(uint32_t id, char* password, uint32_t balance);
bank_account_t createAdminBankAccount(char* password);
bank_account_t findBankAccount(uint32_t id);
bank_account_t errorAccount();
int insertBankAccount(bank_account_t newAccount);
bool existsBankAccount(uint32_t id);
int initAccounts();

#endif