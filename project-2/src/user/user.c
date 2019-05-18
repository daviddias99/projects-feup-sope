#include "user_util.h"
#include "user_validation.h"

int main(int argc, char* argv[]){

    if(argc != 6){
        fprintf(stderr, "Usage: %s <account_ID><account_password><operation_delay><operation_number><arguments>\n", argv[0]);
        exit(1);
    }

    int code;
    user_command_t command;

    if((code = checkArguments(&command, argv[1], argv[2], argv[3], argv[4], argv[5])) != OK){
        printErrorMessage(code);
        exit(2);
    }

    tlv_request_t request;
    tlv_reply_t reply;

    readCommand(&command);

    formatRequest(&request);

    if(setupResponseFIFO() != 0)
        exit(3);

    if(setupRequestFIFO() != 0)
        exit(4);

    if(setupHandlers() != 0)
        exit(5);

    if(waitResponse(&request, &reply) != 0)
        exit(6);

    if(recordOperation(&request, &reply) != 0)
        exit(7);

    return 0;
}