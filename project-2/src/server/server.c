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

    openLogFile();
    initSyncMechanisms((size_t)bankOfficeCount);
    initAccounts();

    bank_account_t adminAccount = createAdminBankAccount(adminPassword);

    memset(argv[2],0,strlen(argv[2])); // don't store the admins password in plain text

    requests = *queue_create();
    insertBankAccount(adminAccount);

    createBankOffices(bankOfficeCount);

    setupRequestFIFO();

    waitForRequests();

    return 0;
}