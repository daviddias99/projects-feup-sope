#include "server_accounts.h"

static bank_account_t accounts[MAX_BANK_ACCOUNTS];
pthread_mutex_t account_mutex[MAX_BANK_ACCOUNTS];


bank_account_t createBankAccount(uint32_t id, char *password, uint32_t balance)
{
    bank_account_t newBankAccount;

    newBankAccount.account_id = id;
    newBankAccount.balance = balance;
    generateSalt(newBankAccount.salt);

    char temp[MAX_PASSWORD_LEN + SALT_LEN];

    strcpy(temp, password);
    strcat(temp, newBankAccount.salt);

    if(generateSHA256sum(temp, newBankAccount.hash) != 0)
        return errorAccount();

    return newBankAccount;
}

bank_account_t createAdminBankAccount(char *password)
{
    return createBankAccount(ADMIN_ACCOUNT_ID, password, 0);
}

int insertBankAccount(bank_account_t newAccount, uint32_t delay,uint32_t officeID)
{
    if (existsBankAccount(newAccount.account_id)) 
        return RC_ID_IN_USE;

    if(logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,0) < 0)
        return -1;

    if(pthread_mutex_lock(&account_mutex[newAccount.account_id]) != 0)
        return -2;

    if(logSyncDelay(getLogfile(), officeID, newAccount.account_id, delay) < 0)
        return -1;

    usleep(MS_TO_US(delay));

    accounts[newAccount.account_id] = newAccount;

    if(logAccountCreation(getLogfile(), officeID, &accounts[newAccount.account_id]) < 0)
        return -1;

    if(pthread_mutex_unlock(&account_mutex[newAccount.account_id]) != 0)
        return -2;

    if(logSyncMech(getLogfile(),officeID,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,0) < 0)
        return -1;

    return 0;
}

bool existsBankAccount(uint32_t id)
{
    if (id >= MAX_BANK_ACCOUNTS) {
        return false;
    }

    if (accounts[id].account_id != ERROR_ACCOUNT_ID) {
        return true;
    }

    return false;
}

bank_account_t* findBankAccount(uint32_t id)
{   
    return id >= MAX_BANK_ACCOUNTS ? NULL : &accounts[id];
}

int initAccounts()
{
    for (size_t i = 0; i < MAX_BANK_ACCOUNTS; i++)
    {
        if(pthread_mutex_init(&account_mutex[i], NULL) != 0)
            return -1;

        accounts[i] = errorAccount();
    }

    return 0;
}

bank_account_t errorAccount()
{

    bank_account_t error;

    error.account_id = ERROR_ACCOUNT_ID;

    return error;
}