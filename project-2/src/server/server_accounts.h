#include <string.h>
#include "../sope.h"
#include "../constants.h"
#include "server_util.h"

extern pthread_mutex_t account_mutex[];

int initAccounts();
bank_account_t createBankAccount(uint32_t id, char* password, uint32_t balance);
bank_account_t createAdminBankAccount(char* password);
bank_account_t* findBankAccount(uint32_t id);
int insertBankAccount(bank_account_t newAccount);
bool existsBankAccount(uint32_t id);
bank_account_t errorAccount();