
#include "server_util.h"


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

    bank_account_t adminAccount = createAdminBankAccount(adminPassword);
    insertBankAccount(adminAccount);
    createBankOffices(bankOfficeCount);

    initSyncMechanisms((size_t)bankOfficeCount);

    setupRequestFIFO();
    waitForRequests();

    return 0;
}