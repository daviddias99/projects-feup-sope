#include <stdlib.h>

#include "server_bank_office.h"
#include "server_util.h"
#include "server_sync.h"
#include "server_accounts.h"
#include "server_operations.h"

int main(int argc, char* argv[]){

    if(argc != 3){

        fprintf( stderr, "Usage: %s <bank_office_count> <admin_password>\n", argv[0]);
        exit(-1);
        
    }

    srand(time(NULL));
    
    int bankOfficeCount = strtol(argv[1],NULL,10);
    char* adminPassword = argv[2];

    if(!passwordIsValid(adminPassword)){

        fprintf( stderr, "Invalid password\n");
        exit(-2);
    }

    if(openLogFile() != 0)
        return -3;

    if(initSyncMechanisms((size_t)bankOfficeCount) != 0)
        return -4;

    if(createBankOffices(bankOfficeCount) != 0)
        return -5;

    if(initAccounts() != 0)
        return -6;

    bank_account_t adminAccount = createAdminBankAccount(adminPassword);
    memset(argv[2],0,strlen(argv[2])); // don't store the admins password in plain text

    if(insertBankAccount(adminAccount,0,MAIN_THREAD_ID) != 0)
        return -7;

    requests = *queue_create();

    if(setupRequestFIFO() != 0)
        return -8;

    if(waitForRequests() != 0)
        return -9;

    if(closeOffices() != 0)
        return -10;

    return 0;
}