#include <stdlib.h>

#include "server_bank_office.h"
#include "server_util.h"
#include "server_sync.h"
#include "server_accounts.h"
#include "server_operations.h"

void printSizes(){
/*
    printf("tlv_request: %ld \n",sizeof(tlv_request_t));
    printf("req_value: %ld \n",sizeof(req_value_t));
    printf("req_transfer: %ld \n",sizeof(req_transfer_t));
    printf("req_create_account: %ld \n",sizeof(req_create_account_t));
    printf("req_header: %ld \n",sizeof(req_header_t));

    printf("------------\n");

    printf("tlv_reply: %ld \n",sizeof(tlv_reply_t));
    printf("rep_value: %ld \n",sizeof(rep_value_t));
    printf("rep_transfer: %ld \n",sizeof(rep_transfer_t));
    printf("req_shutdown: %ld \n",sizeof(rep_shutdown_t));
    printf("rep_balance: %ld \n",sizeof(rep_balance_t));
    printf("rep_header: %ld \n",sizeof(rep_header_t));

    printf("------------\n");

    printf("ret_code: %ld \n",sizeof(ret_code_t));
    printf("op_type: %ld \n",sizeof(op_type_t));
*/
}

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

    printSizes();

    openLogFile();

    initSyncMechanisms((size_t)bankOfficeCount);

    createBankOffices(bankOfficeCount);
    initAccounts();

    bank_account_t adminAccount = createAdminBankAccount(adminPassword);
    memset(argv[2],0,strlen(argv[2])); // don't store the admins password in plain text
    insertBankAccount(adminAccount,0,MAIN_THREAD_ID);

    requests = *queue_create();

    setupRequestFIFO();

    waitForRequests();

    closeOffices();

    return 0;
}