
#include "user_util.h"

int request_fifo_fd;
int response_fifo_fd;
char response_filename[USER_FIFO_PATH_LEN];
user_command_t command;

static const char *ERROR_MESSAGES[] = {
    [OK] = "Operation successful",
    [NEG_ACCOUNT_ID] = "Account ID can't be negative.",
    [LRG_ACCOUNT_ID] = "Account ID too large.",
    [SRT_PASSWORD] = "Password was too short.",
    [LNG_PASSWORD] = "Password was too long.",
    [BAD_PASSWORD] = "Password can't contain blank spaces.",
    [NEG_DELAY] = "Operation delay can't be negative or zero.",
    [LNG_DELAY] = "Operation delay was too long.",
    [INV_OPERATION] = "Choosen operation doesn't exist.",
    [INV_NEW_ACCOUNT_ID] = "New account ID is not accepted.",
    [SRT_BALANCE] = "New account balance is too small.",
    [LRG_BALANCE] = "New account balance is too large.",
    [INV_NEW_PASSWORD] = "New account password is invalid.",
    [INV_TRS_ACCOUNT] = "Recipient account ID is not accepted.",
    [SML_TRSF_AMOUNT] = "Transfering amount is too small.",
    [LRG_TRSF_AMOUNT] = "Transfering amount is too large.",
    [INV_OP_ARGUMENTS] = "Operation doesn't accept those arguments.",
    [LOG_REQUEST_ERROR] = "Failed to record request on user log file.",
    [LOG_REPLY_ERROR] = "Failed to record reply on user log file.",
    [OTHER_ERROR] = "Something went wrong.",
};

void alarm_handler(int signo)
{
    UNUSED(signo);

    recordError(SRV_TIMEOUT);

    closeComunication();

    exit(7);
}

void printErrorMessage(int error)
{
    printf("Error: %s\n", ERROR_MESSAGES[error]);
}

int readCommand(user_command_t *user_command)
{
    command = *user_command;

    return 0;
}

int setupRequestFIFO()
{
    if ((request_fifo_fd = open(SERVER_FIFO_PATH, O_WRONLY)) == -1)
    {
        recordError(SRV_DOWN);
        return 1;
    }

    return 0;
}

int setupResponseFIFO()
{
    char fifo_name[USER_FIFO_PATH_LEN];
    pid_t pid = getpid();

    sprintf(fifo_name, "%s%05d", USER_FIFO_PATH_PREFIX, pid);
    memcpy(response_filename, fifo_name, USER_FIFO_PATH_LEN);

    if (mkfifo(fifo_name, RESPONSE_FIFO_PERM) == -1)
    {
        perror("Response FIFO");
        return 1;
    }

    if ((response_fifo_fd = open(fifo_name, O_RDWR | O_NONBLOCK)) == -1)
    {
        perror("Response FIFO");
        return 2;
    }

    int flags;

    if ((flags = fcntl(response_fifo_fd, F_GETFL, 0)) == -1)
    {
        perror("Response FIFO");
        return 3;
    }

    flags &= ~O_NONBLOCK;

    if (fcntl(response_fifo_fd, F_SETFL, flags) == -1)
    {
        perror("Response FIFO");
        return 4;
    }

    return 0;
}

int closeRequestFIFO()
{
    if (close(request_fifo_fd) == -1)
    {
        perror("Request FIFO");
        return 1;
    }

    return 0;
}

int closeResponseFIFO()
{
    if (close(response_fifo_fd) == -1)
    {
        perror("Response FIFO");
        return 1;
    }

    return 0;
}

int closeComunication()
{ // change name?

    closeRequestFIFO();
    closeResponseFIFO();

    if (unlink(response_filename) == -1)
    {
        perror("Response FIFO.");
        return 1;
    }

    return 0;
}

int recordOperation(tlv_request_t *request, tlv_reply_t *reply)
{
    int fd;
    int pid = getpid();

    if ((fd = open(USER_LOGFILE, O_WRONLY | O_APPEND)) == -1)
    {
        perror("User Logfile");
        return 1;
    }

    if (logRequest(fd, pid, request) < 0)
    {
        printf("Error: %s\n", ERROR_MESSAGES[LOG_REQUEST_ERROR]);
        close(fd);
        return 2;
    }

    // TODO: this shouldn't be here

    if(reply->value.header.ret_code != RC_OK){

        if(reply->type == OP_SHUTDOWN){

            reply->value.shutdown.active_offices = 0;
        }
        else if(reply->type == OP_TRANSFER){

            reply->value.transfer.balance  = request->value.transfer.amount;
        }

    }

    if (logReply(fd, pid, reply) < 0)
    {
        printf("Error: %s\n", ERROR_MESSAGES[LOG_REPLY_ERROR]);
        close(fd);
        return 3;
    }
    
    if (close(fd) == -1)
    {
        perror("User Logfile");
        return 4;
    }

    return 0;
}

int formatReqCreateAccount(req_value_t *request_value)
{
    req_create_account_t create_account;

    char *arguments = (char *)malloc(sizeof(char) * strlen(command.arguments) + 1);

    memcpy(arguments, command.arguments, strlen(command.arguments) + 1);

    char *accountID = strtok(arguments, " ");
    char *balance = strtok(NULL, " ");
    char *password = strtok(NULL, " ");

    create_account.account_id = atoi(accountID);
    create_account.balance = atoi(balance);

    memcpy(create_account.password, password, MAX_PASSWORD_LEN);

    request_value->create = create_account;

    return 0;
}

int formatReqTransfer(req_value_t *request_value)
{
    req_transfer_t tranfer;

    char *arguments = (char *)malloc(sizeof(char) * strlen(command.arguments) + 1);

    memcpy(arguments, command.arguments, strlen(command.arguments) + 1);

    char *recipientID = strtok(arguments, " ");

    tranfer.account_id = atoi(recipientID);

    char *amount = strtok(NULL, " ");

    tranfer.amount = atoi(amount);

    request_value->transfer = tranfer;

    return 0;
}

int formatReqHeader(req_header_t *header)
{

    header->pid = getpid();
    header->account_id = command.accountID;
    memcpy(header->password, command.password, MAX_PASSWORD_LEN);
    header->op_delay_ms = command.delay;

    return 0;
}

int formatReqValue(req_value_t *request_value)
{
    req_header_t request_header;

    formatReqHeader(&request_header);

    request_value->header = request_header;

    switch (command.operation)
    {
    case 0:
        formatReqCreateAccount(request_value);
        break;
    case 1:
        break;
    case 2:
        formatReqTransfer(request_value);
        break;
    case 3:
        break;
    default:
        break;
    }

    return 0;
}

int formatRequest(tlv_request_t *request)
{

    req_value_t request_value;

    formatReqValue(&request_value);

    int op_type = command.operation;

    request->type = op_type;
    request->length = sizeof(req_header_t);

    switch (request->type){
        case OP_CREATE_ACCOUNT:
            request->length += sizeof(req_create_account_t);
            break;

        case OP_TRANSFER:
            request->length += sizeof(req_transfer_t);
            break;

        default:
            break;
    }
    
    request->value = request_value;

    return 0;
}

int sendRequest(tlv_request_t *request)
{
    write(request_fifo_fd, request, sizeof(op_type_t) + sizeof(uint32_t) + request->length);

    return 0;
}

int readReply(tlv_reply_t *reply){

    if(read(response_fifo_fd, &(reply->type), sizeof(op_type_t)) < 0){
        perror("Reply (operation type)");
        return 1;
    }

    if(read(response_fifo_fd, &(reply->length), sizeof(uint32_t)) < 0){
        perror("Reply (length)");
        return 2;
    }

    if(read(response_fifo_fd, &(reply->value), reply->length) < 0){
        perror("Reply (value)");
        return 3;
    }

    return 0;
}

int formatRepBalanceAccount(rep_value_t *reply_value)
{
    rep_balance_t reply_balance;

    reply_balance.balance = 0; // ATTENTION: Impossible to know if you are the user?

    reply_value->balance = reply_balance;

    return 0;
}

int formatRepTransfer(rep_value_t *reply_value)
{
    rep_transfer_t reply_transfer;

    reply_transfer.balance = 0; // ATTENTION: Impossible to know if you are the user?

    reply_value->transfer = reply_transfer;

    return 0;
}

int formatRepShutdownTransfer(rep_value_t *reply_value)
{
    rep_shutdown_t reply_shutdown;

    reply_shutdown.active_offices = 0; // ATTENTION: Impossible to know if you are the user?

    reply_value->shutdown = reply_shutdown;

    return 0;
}

int formatRepHeader(rep_header_t *header, int ret_code)
{
    header->account_id = command.accountID;
    header->ret_code = ret_code;

    return 0;
}

int formatRepValue(rep_value_t *reply_value, int ret_code)
{
    rep_header_t header;

    formatRepHeader(&header, ret_code);

    reply_value->header = header;

    switch (command.operation)
    {
    case 0:
        break;
    case 1:
        formatRepBalanceAccount(reply_value);
        break;
    case 2:
        formatRepTransfer(reply_value);
        break;
    case 3:
        formatRepShutdownTransfer(reply_value);
        break;
    default:
        break;
    }

    return 0;
}

int formatReply(tlv_reply_t *reply, int ret_code)
{
    rep_value_t reply_value;

    formatRepValue(&reply_value, ret_code);

    reply->type = command.operation;
    reply->length = sizeof(rep_header_t);

    switch(reply->type){
        case OP_BALANCE:
            reply->length += sizeof(rep_balance_t);
            break;
        case OP_TRANSFER:
            reply->length += sizeof(rep_transfer_t);
            break;
        case OP_SHUTDOWN:
            reply->length += sizeof(rep_shutdown_t);
            break;
        default:
            break;
    }

    reply->value = reply_value;

    return 0;
}

int recordError(int ret_code)
{
    tlv_request_t request;
    tlv_reply_t reply;

    formatRequest(&request);
    formatReply(&reply, ret_code);

    recordOperation(&request, &reply);

    return 0;
}

int waitResponse(tlv_request_t *request, tlv_reply_t *reply)
{
    struct sigaction action;

    action.sa_handler = alarm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM, &action, NULL) < 0)
    {
        perror("Alarm Handler");
        return 1;
    }

    sendRequest(request);

    alarm(FIFO_TIMEOUT_SECS);

    readReply(reply);


    alarm(CANCEL_ALARM);

    if (closeComunication() != 0)
        return 1;

    return 0;
}
