
#include "server_util.h"

bank_account_array_t accounts;

int main(int argc, char* argv[]){

    srand(time(NULL));
    accounts.next_account_index = 0;

    if(argc != 3){

        fprintf( stderr, "Usage: %s <bank_office_count> <admin_password>\n", argv[0]);
        exit(-1);
        
    }

    int bankOfficeCount = strtol(argv[1],NULL,10);
    char* adminPassword = argv[2];

    if(!passwordIsValid(adminPassword)){

        fprintf( stderr, "Invalid password\n");
        exit(-2);
    }

    bank_account_t createAdminBankAccount(adminPassword);


    return 0;
}