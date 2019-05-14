#include "user_util.h"

int request_fifo_fd;
int response_fifo_fd;
char response_filename[USER_FIFO_PATH_LEN];
user_command_t command = {0, "", 0, 0, ""};

void alarm_handler(int signo){

    UNUSED(signo);
    
    recordError(SRV_TIMEOUT);

    closeComunication();

    raise(SIGKILL); // Ends the program
}

int setupRequestFIFO(){
    if((request_fifo_fd= open(SERVER_FIFO_PATH, O_WRONLY)) == -1){
        recordError(SRV_DOWN);

        return -1;
    }

    return 0;
}

int setupResponseFIFO(){
    char fifo_name[USER_FIFO_PATH_LEN];
    pid_t pid = getpid();
    
    sprintf(fifo_name, "%s%05d", USER_FIFO_PATH_PREFIX, pid);
    memcpy(response_filename, fifo_name, USER_FIFO_PATH_LEN);
    mkfifo(fifo_name, RESPONSE_FIFO_PERM);

    if((response_fifo_fd = open(fifo_name, O_RDWR | O_NONBLOCK)) == -1){

        return 1;
    }
        

    int flags = fcntl(response_fifo_fd,F_GETFL,0);

    flags &= ~O_NONBLOCK;
    fcntl(response_fifo_fd,F_SETFL,flags);

    return 0;
}

int closeRequestFIFO(){
    if(close(request_fifo_fd) == -1){
        perror("Couldn't close Request FIFO");
        return 1;
    }

    return 0;
}

int closeResponseFIFO(){
    if(close(response_fifo_fd) == -1){
        perror("Couldn't close Response FIFO");
        return 1;
    }

    return 0;
}

int closeComunication(){ // change name?
    
    closeRequestFIFO();
    closeResponseFIFO();
    
    unlink(response_filename);

    return 0;
}

int recordOperation(tlv_request_t* request, tlv_reply_t* reply){
    int fd;
    int pid = getpid();

    if((fd = open(USER_LOGFILE, O_WRONLY | O_APPEND)) == -1){
        perror("Failed to open User Logfile.");
        return 1;
    }

    // TODO: Implement synchronization?
    logRequest(fd, pid, request); // Is the second argument the process PID??
    logReply(fd, pid, reply);


    return 0;
}

bool validAccount(char* accountID){
    int ID = atoi(accountID);

    command.accountID = ID;

    if(ID < 0)
        return false;

    if(ID > MAX_BANK_ACCOUNTS)
        return false;

    return true;
}

bool validPassword(char* password){
    size_t password_length = strlen(password);

    if(password_length < MIN_PASSWORD_LEN)
        return false;

    if(password_length > MAX_PASSWORD_LEN)
        return false;

    char* token = strtok(password, " ");

    if(strlen(token) != password_length) // Checks if there are no blanck spaces (not sure it works yet)
        return false;

    memcpy(command.password, password, MAX_PASSWORD_LEN + 1);

    return true;
}

bool validDelay(char* delay){
    int op_delay = atoi(delay);

    if(op_delay < 0)
        return false;

    if(op_delay > MAX_OP_DELAY_MS)
        return false;

    command.delay = op_delay;

    return true;
}

bool validOperation(char* operation){
    int op_number = atoi(operation);

    if(op_number < 0)
        return false;

    if(op_number > 3)
        return false;

    command.operation = op_number;

    return true;
}

bool validArguments(char* arguments){

    memcpy(command.arguments, arguments, strlen(arguments) + 1);

    switch(command.operation){
        case 0:
            return validCreationOperation(arguments);
        case 1: 
            return validBalanceOperation(arguments);
        case 2:
            return validTransferOperation(arguments);
        case 3:
            return validShutdownOperation(arguments);
        default:
            return false;
    }
}

bool validCreationOperation(char* arguments){
    
    char* temp_Args = (char*) malloc(sizeof(char) * strlen(arguments) + 1);

    memcpy(temp_Args,arguments,strlen(arguments)+1);

    char* newAccountID = strtok(temp_Args, " ");

    if(!validAccount(newAccountID))
        return false;

    char* token = strtok(NULL, " ");

    long unsigned int balance = atoi(token);

    if(balance < 1)
        return false;

    if(balance > MAX_BALANCE)
        return false;

    char* password = strtok(NULL, " ");

    if(!validPassword(password))
        return false;

    token = strtok(NULL, " ");

    if(token != NULL)
        return false;

    return true;
}

bool validBalanceOperation(char* arguments){
    if(strlen(arguments) != 0)
        return false;
    
    return true;
}

bool validTransferOperation(char* arguments){
    char* temp_Args = (char*) malloc(sizeof(char) * strlen(arguments) + 1);

    memcpy(temp_Args,arguments,strlen(arguments)+1);

    char* recipientID = strtok(temp_Args, " ");

    if(!validAccount(recipientID))
        return false;

    char* token = strtok(NULL, " ");

    if(strlen(token) == 0)
        return false;

    long unsigned int amount = atoi(token);

    if(amount < 1)
        return false;

    if(amount > MAX_BALANCE)
        return false;

    token = strtok(NULL, " ");

    if(token != NULL)
        return false;
  
    
    return true;
}

bool validShutdownOperation(char* arguments){
    if(strlen(arguments) != 0)
            return false;
        
    return true;
}

int checkArguments(char* accountID, char* password, char* delay, char* operation, char* arguments){

    if(!validAccount(accountID))
        return -1;

    if(!validPassword(password))
        return -1;

    if(!validDelay(delay))
        return -1;

    if(!validOperation(operation))
        return -1;

    if(!validArguments(arguments))
        return -1;

    return 0;
}

int formatCreateAccount(req_value_t* request_value){
    req_create_account_t create_account;   

    char* arguments = (char*) malloc(sizeof(char) * strlen(command.arguments) + 1);

    memcpy(arguments, command.arguments, strlen(command.arguments) + 1);    

    char* accountID = strtok(arguments, " ");
    char* balance = strtok(NULL, " ");
    char* password = strtok(NULL, " ");

    create_account.account_id = atoi(accountID);
    create_account.balance = atoi(balance);
       
    memcpy(create_account.password, password, MAX_PASSWORD_LEN);

    request_value->create = create_account;

    return 0;
}

int formatTranfer(req_value_t* request_value){
    req_transfer_t tranfer;

    char* arguments = (char*) malloc(sizeof(char) * strlen(command.arguments) + 1);

    memcpy(arguments, command.arguments, strlen(command.arguments) + 1);    

    char* recipientID = strtok(arguments, " ");

    tranfer.account_id = atoi(recipientID);  

    char* amount = strtok(NULL, " ");

    tranfer.amount = atoi(amount);

    request_value->transfer = tranfer;

    return 0;
}

int formatReqHeader(req_header_t* header){
    
    header->pid = getpid();
    header->account_id = command.accountID;
    memcpy(header->password, command.password, MAX_PASSWORD_LEN);
    header->op_delay_ms = command.delay;

    return 0;
}

int formatReqValue(req_value_t* request_value){
    req_header_t request_header;

    formatReqHeader(&request_header);

    request_value->header = request_header;

    switch(command.operation){
        case 0:
            formatCreateAccount(request_value);
            break;
        case 1:
            break;
        case 2:
            formatTranfer(request_value);
            break;
        case 3:
            break;
        default:
            break;
    }

    return 0;
}

int formatRequest(tlv_request_t* request){
    
    req_value_t request_value;

    formatReqValue(&request_value);

    int op_type = command.operation;

    request->type = op_type;
    request->length = sizeof(tlv_request_t);
    request->value = request_value;

    return 0;
}

int sendRequest(tlv_request_t* request){
    write(request_fifo_fd, request, request->length);

    return 0;
}

int formatRepHeader(rep_header_t* header, int ret_code){
    header->account_id = command.accountID;
    header->ret_code = ret_code;

    return 0;
}

int formatRepValue(rep_value_t* response_value, int ret_code){
    rep_header_t header;

    formatRepHeader(&header, ret_code);

    response_value->header = header;

    return 0;
}

int formatReply(tlv_reply_t* reply, int ret_code){
    rep_value_t value;

    formatRepValue(&value, ret_code);

    reply->value = value;

    return 0;
}

int recordError(int ret_code){
    tlv_request_t request;
    tlv_reply_t reply;

    formatRequest(&request);
    formatReply(&reply, ret_code);

    recordOperation(&request, &reply);

    return 0;
}

int waitResponse(tlv_request_t* request, tlv_reply_t* reply){
    struct sigaction action;

    action.sa_handler = alarm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM,&action,NULL) < 0){
        perror("Alarm handler setup failed!");
        return 1;
    }

    sendRequest(request);

    alarm(FIFO_TIMEOUT_SECS);

    read(response_fifo_fd, reply, sizeof(tlv_reply_t)); // Not sure if size is in the correct way to do it

    alarm(CANCEL_ALARM);

    closeComunication();

    return 0;
}