#ifndef SERVER_ACCOUNTS_H
#define SERVER_ACCOUNTS_H

#include <string.h>
#include "../debug.h"
#include "../sope.h"
#include "../constants.h"
#include "server_util.h"

extern pthread_mutex_t account_mutex[];

/**
 * @brief Initializes the account array and corresponding mutex array. All accounts are initialized with the error account;
 * 
 * @return int 0 upon sucess, non-zero otherwise
 */
int initAccounts();

/**
 * @brief Create a Bank Account object
 * 
 * @param id                id of the new bank account
 * @param password          password of the new bank account
 * @param balance           initial balance of the new bank account
 * @return bank_account_t   account with given parameters or errorAccount if an error occurs
 */
bank_account_t createBankAccount(uint32_t id, char* password, uint32_t balance);

/**
 * @brief Create a bank account for an admin (used the predefined admin ID)
 * 
 * @param password          password of the new bank account
 * @return bank_account_t   account with given parameters or errorAccount if an error occurs
 */
bank_account_t createAdminBankAccount(char* password);

/**
 * @brief Finds the bank account with the given ID;
 * 
 * @param id                id of the bank account
 * @return bank_account_t*  pointer to the bank account or null if the account id isn't valid
 */
static inline bank_account_t* findBankAccount(uint32_t id);

/**
 * @brief Inserts the bank account into the account array. (used account mutex)
 * 
 * @param newAccount        bank account to be inserted
 * @param delay             delay of the operation in ms
 * @param officeID          id of the bank office inserting the account
 * @return int              0 upon sucess, non-zero otherwise
 */
int insertBankAccount(bank_account_t newAccount, uint32_t delay,uint32_t officeID);

/**
 * @brief Checks if the bank account with the given ID exists
 * 
 * @param id                id of the account       
 * @return true             the account exists
 * @return false            the account doesn't exist
 */
bool existsBankAccount(uint32_t id);

/**
 * @brief Get the error account. An error account as an ID that is defined in the constants file as ERROR_ACCOUNT_ID.
 * 
 * @return bank_account_t   error account object
 */
bank_account_t errorAccount();

#endif
