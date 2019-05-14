#include "user_util.h"

int main(int argc, char* argv[]){

    if(argc != 6){
        fprintf(stderr, "Usage: %s <account_ID><account_password><operation_delay><operation_number><arguments>\n", argv[0]);
        exit(1);
    }

    int code;

    if((code = checkArguments(argv[1], argv[2], argv[3], argv[4], argv[5])) != OK){
        printf("Error: %s\n", ERROR_MESSAGES[code]);
        exit(2);
    }

    tlv_request_t request;
    tlv_reply_t reply;

    formatRequest(&request);

    if(setupResponseFIFO() != 0)
        exit(3);

    if(setupRequestFIFO() != 0)
        exit(4);

    if(waitResponse(&request, &reply) != 0)
        exit(5);

    if(recordOperation(&request, &reply) != 0)
        exit(6);

    return 0;
}