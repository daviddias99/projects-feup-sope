#include "user_util.h"

int request_fifo_fd;
int response_fifo_fd;
char response_filename[USER_FIFO_PATH_LEN];

void alarm_handler(int signo){

    UNUSED(signo);
    printf("Response took too long!\n");
    closeComunication();

    raise(SIGKILL); // Ends the program
}

int setupRequestFIFO(){

    

    if((request_fifo_fd= open(SERVER_FIFO_PATH, O_WRONLY)) == -1)
        return 1;

    

    return 0;
}

int setupResponseFIFO(){
    char fifo_name[USER_FIFO_PATH_LEN];
    pid_t pid = getpid();
    
    sprintf(fifo_name, "%s%05d", USER_FIFO_PATH_PREFIX, pid);
    memcpy(response_filename, fifo_name, USER_FIFO_PATH_LEN);

    mkfifo(fifo_name, RESPONSE_FIFO_PERM);

    if((response_fifo_fd = open(fifo_name, O_RDONLY | O_NONBLOCK)) == -1)
        return 1;

    return 0;
}

int closeComunication(){ // change name?
    if(close(request_fifo_fd) == -1){
        perror("Couldn't close Request FIFO");
        return 1;
    }

    if(close(response_fifo_fd) == -1){
        perror("Couldn't close Response FIFO");
        return 1;
    }

    // remove(response_fifo_fd);

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

    if(ID < 1)
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

    return true;
}

bool validDelay(char* delay){
    int op_delay = atoi(delay);

    if(op_delay < 0)
        return false;

    if(op_delay > MAX_OP_DELAY_MS)
        return false;

    return true;
}

bool validOperation(char* operation){
    int op_number = atoi(operation);

    if(op_number < 0)
        return false;

    if(op_number > 3)
        return false;

    return true;
}

bool validArguments(char* accountID, char* operation, char* arguments){
    int op_number = atoi(operation);

    switch(op_number){
        case 0:
            return validCreationOperation(accountID, arguments);
        case 1: 
            return validBalanceOperation(accountID, arguments);
        case 2:
            return validTransferOperation(accountID, arguments);
        case 3:
            return validShutdownOperation(accountID, arguments);
        default:
            return false;
    }
}

bool validCreationOperation(char* accountID, char* arguments){

    int ID = atoi(accountID);
    
    char* temp_Args = (char*) malloc(sizeof(char) * strlen(arguments) + 1);

    memcpy(temp_Args,arguments,strlen(arguments)+1);

    if(ID != 0)
        return false;

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

bool validBalanceOperation(char* accountID, char* arguments){
    int ID = atoi(accountID);

    if(ID == 0)
        return false;

    if(strlen(arguments) != 0)
        return false;
    
    return true;
}

bool validTransferOperation(char* accountID, char* arguments){
    int ID = atoi(accountID);

    char* temp_Args = (char*) malloc(sizeof(char) * strlen(arguments) + 1);

    memcpy(temp_Args,arguments,strlen(arguments)+1);

    if(ID == 0)
        return false;

    char* recipientID = strtok(temp_Args, " ");

    if(!validAccount(recipientID) || !strcmp(accountID, recipientID))
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

bool validShutdownOperation(char* accountID, char* arguments){
    int ID = atoi(accountID);

    if(ID != 0)
        return false;

    if(strlen(arguments) != 0)
            return false;
        
    return true;
}

int checkArguments(char* accountID, char* password, char* delay, char* operation, char* arguments){

    if(!validAccount(accountID) && atoi(accountID) != 0)
        return ERROR_INVALID_ACCOUNT;

    if(!validPassword(password))
        return ERROR_INVALID_PASSWORD;

    if(!validDelay(delay))
        return ERROR_INVALID_DELAY;

    if(!validOperation(operation))
        return ERROR_INVALID_OPERATION;

    if(!validArguments(accountID, operation, arguments))
        return ERROR_INVALID_ARGUMENTS;

    return VALID_OPERATION;
}

int formatCreateAccount(req_value_t* request_value, char* arguments){
    req_create_account_t create_account;

    

    

    char* accountID = strtok(arguments, " ");
    char* balance = strtok(NULL, " ");
    char* password = strtok(NULL, " ");

    

    print_dbg("--%s | %s | %s | %s \n",arguments,accountID,balance,password);

    create_account.account_id = atoi(accountID);
    create_account.balance = atoi(balance);

       
    memcpy(create_account.password, password, MAX_PASSWORD_LEN);

    
    
    request_value->create = create_account;

    

    return 0;
}

int formatTranfer(req_value_t* request_value, char* arguments){
    req_transfer_t tranfer;

    char* recipientID = strtok(arguments, " ");

    tranfer.account_id = atoi(recipientID);  

    char* amount = strtok(NULL, " ");

    tranfer.amount = atoi(amount);

    request_value->transfer = tranfer;

    return 0;
}

int formatHeader(req_header_t* header, char* accountID, char* password, char* op_delay){
    header->pid = getpid();
    header->account_id = atoi(accountID);
    memcpy(header->password, password, MAX_PASSWORD_LEN);
    header->op_delay_ms = atoi(op_delay);

    return 0;
}

int formatValue(req_value_t* request_value, char* accountID, char* password, char* delay, char* operation, char* arguments){
    req_header_t request_header;

    formatHeader(&request_header, accountID, password, delay);

    request_value->header = request_header;

    int op_number = atoi(operation);

    switch(op_number){
        case 0:
            
            formatCreateAccount(request_value, arguments);
            
            break;
        case 1:
            break;
        case 2:
            formatTranfer(request_value, arguments);
            break;
        case 3:
            break;
    }

    return 0;
}

int formatRequest(tlv_request_t* request, char* accountID, char* password, char* delay, char* operation, char* arguments){
    
    req_value_t request_value;

    
    
    formatValue(&request_value, accountID, password, delay, operation, arguments);

    

    int op_type = atoi(operation);

    request->type = op_type;
    request->length = sizeof(tlv_request_t);
    request->value = request_value;

    return 0;
}

int sendRequest(tlv_request_t* request){
    write(request_fifo_fd, request, request->length);

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

    read(response_fifo_fd, reply, sizeof(reply)); // Not sure if size is in the correct way to do it
    printf("gotcha %d\n", reply->value.header.ret_code);
    

    alarm(CANCEL_ALARM);

    closeComunication();

    return 0;
}