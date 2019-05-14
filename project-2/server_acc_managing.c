
#include "server_acc_managing.h"



bank_account_t createAdminBankAccount(char *password)
{

    return createBankAccount(ADMIN_ACCOUNT_ID, password, 0);
}

bank_account_t createBankAccount(uint32_t id, char *password, uint32_t balance)
{

    bank_account_t newBankAccount;

    newBankAccount.account_id = id;
    newBankAccount.balance = balance;
    generateSalt(newBankAccount.salt);

    char temp[MAX_PASSWORD_LEN + SALT_LEN];

    strcpy(temp, password);
    strcat(temp, newBankAccount.salt);

    generateSHA256sum(temp, newBankAccount.hash);

    return newBankAccount;
}

bank_account_t findBankAccount(uint32_t id)
{
    return accounts[id];
}

bank_account_t errorAccount(){

    bank_account_t error;

    error.account_id = ERROR_ACCOUNT_ID;

    return error;
}

int initAccounts(){

    for(size_t i = 0; i < MAX_BANK_ACCOUNTS;i++){

        pthread_mutex_init(&account_mutex[i],NULL);
        accounts[i] = errorAccount();

    }

    return 0;
}

int insertBankAccount(bank_account_t newAccount)
{
    pthread_mutex_lock(&account_mutex[newAccount.account_id]);

    if (newAccount.account_id >= MAX_BANK_ACCOUNTS){

        return -1;
    }
    if (existsBankAccount(newAccount.account_id)){

        return -2;
    }
        
    accounts[newAccount.account_id] = newAccount;

    pthread_mutex_unlock(&account_mutex[newAccount.account_id]);

    logAccountCreation(getLogfileFD(),pthread_self(),&accounts[newAccount.account_id]);

    return 0;
}

bool existsBankAccount(uint32_t id)
{

    if(id >= MAX_BANK_ACCOUNTS)
        return false;

    if (accounts[id].account_id != ERROR_ACCOUNT_ID)
        return true;
        
    return false;
}

