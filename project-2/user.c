#include "user_util.h"

int main(int argc, char* argv[]){

    if(argc != 6){
        fprintf(stderr, "Usage: %s <account_ID> <account_password><operation_delay><operation_number><arguments>\n", argv[0]);
        exit(-1);
    }

    if(checkArguments(argv[1], argv[2], argv[3], argv[4], argv[5]) != VALID_OPERATION)
        exit(1); // TODO: Show error message depending on return value?

    tlv_request_t request;
    tlv_reply_t reply;
    
    formatRequest(&request, argv[1], argv[2], argv[3], argv[4], argv[5]);

    if(setupResponseFIFO() == 1)
        exit(2);

    if(setupRequestFIFO() == 1)
        exit(3);

    waitResponse(&request, &reply);

    if(recordOperation(&request, &reply) == 1)
        exit(4);

    return 0;
}