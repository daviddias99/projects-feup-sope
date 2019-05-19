#include "user_validation.h"

int validUserAccount(user_command_t* command, char* accountID){

    int ID = atoi(accountID);

    command->accountID = ID;

    if(ID < 0)
        return NEG_ACCOUNT_ID;

    if(ID > MAX_BANK_ACCOUNTS)
        return LRG_ACCOUNT_ID;

    return OK;
}

int validAccount(char* accountID){
    int ID = atoi(accountID);

    if(ID < 0)
        return INV_TRS_ACCOUNT;

    if(ID > MAX_BANK_ACCOUNTS)
        return INV_TRS_ACCOUNT;

    return OK;
}

int validPassword(user_command_t* command, char* password){
    size_t password_length = strlen(password);

    if(password_length < MIN_PASSWORD_LEN)
        return SRT_PASSWORD;

    if(password_length > MAX_PASSWORD_LEN)
        return LNG_PASSWORD;

    char* token = strtok(password, " ");

    if(strlen(token) != password_length) // Checks if there are no blanck spaces (not sure it works yet)
        return BAD_PASSWORD;

    memcpy(command->password, password, MAX_PASSWORD_LEN + 1);

    return OK;
}

int validNewPassword(char* password){
    size_t password_length = strlen(password);

    if(password_length < MIN_PASSWORD_LEN)
        return SRT_PASSWORD;

    if(password_length > MAX_PASSWORD_LEN)
        return LNG_PASSWORD;

    return OK;
}

int validDelay(user_command_t* command, char* delay){
    int op_delay = atoi(delay);

    if(op_delay < 0)
        return NEG_DELAY;

    if(op_delay > MAX_OP_DELAY_MS)
        return LNG_DELAY;

    command->delay = op_delay;

    return OK;
}

int validOperation(user_command_t* command, char* operation){
    int op_number = atoi(operation);

    if(op_number < 0)
        return INV_OPERATION;

    if(op_number > 3)
        return INV_OPERATION;

    command->operation = op_number;

    return OK;
}

int validBalance(char* value){
    long unsigned int balance = atoi(value);

    if(balance < 1)
        return SRT_BALANCE;

    if(balance > MAX_BALANCE)
        return LRG_BALANCE;

    return OK;
}

int validAmount(char* value){
    long unsigned int amount = atoi(value);

    if(amount < 1)
        return SML_TRSF_AMOUNT;

    if(amount > MAX_BALANCE)
        return LRG_TRSF_AMOUNT;

    return OK;
}

int validArguments(user_command_t* command, char* arguments){

    command->arguments = arguments;
    
    switch(command->operation){
        case 0:
            return validCreationOperation(arguments);
        case 1: 
            return validBalanceOperation(arguments);
        case 2:
            return validTransferOperation(arguments);
        case 3:
            return validShutdownOperation(arguments);
        default:
            return OTHER_ERROR;
    }
}

int validCreationOperation(char* arguments){
    
    int code;
    char* temp_Args = (char*) malloc(sizeof(char) * strlen(arguments) + 1);

    memcpy(temp_Args,arguments,strlen(arguments)+1);

    if(strlen(temp_Args) == 0)
        return INV_OP_ARGUMENTS;

    char* newAccountID = strtok(temp_Args, " ");

    if(validAccount(newAccountID) != OK)
        return INV_NEW_ACCOUNT_ID;

    char* token = strtok(NULL, " ");

    if(token == NULL)
        return INV_OP_ARGUMENTS;

    if((code = validBalance(token)) != OK)
        return code;

    char* password = strtok(NULL, " ");

    if(password == NULL)
        return INV_OP_ARGUMENTS;

    if(validNewPassword(password) != OK)
        return INV_NEW_PASSWORD;

    token = strtok(NULL, " ");

    if(token != NULL)
        return INV_OP_ARGUMENTS;

    return OK;
}

int validBalanceOperation(char* arguments){
    if(strlen(arguments) != 0)
        return INV_OP_ARGUMENTS;
    
    return OK;
}

int validTransferOperation(char* arguments){
    int code;
    char* temp_Args = (char*) malloc(sizeof(char) * strlen(arguments) + 1);

    memcpy(temp_Args,arguments,strlen(arguments)+1);

    if(strlen(temp_Args) == 0)
        return INV_OP_ARGUMENTS;

    char* recipientID = strtok(temp_Args, " ");

    if(validAccount(recipientID) != OK)
        return INV_TRS_ACCOUNT;

    char* token = strtok(NULL, " ");

    if(token == NULL)
        return INV_OP_ARGUMENTS;

    if((code = validAmount(token)) != OK)
        return code;

    token = strtok(NULL, " ");

    if(token != NULL)
        return INV_OP_ARGUMENTS;
    
    return OK;
}

int validShutdownOperation(char* arguments){
    if(strlen(arguments) != 0)
            return INV_OP_ARGUMENTS;

    return OK;
}

int checkArguments(user_command_t* command, char* accountID, char* password, char* delay, char* operation, char* arguments){

    int code;

    if((code = validUserAccount(command, accountID)) != OK)
        return code;

    if((code = validPassword(command, password)) != OK)
        return code;

    if((code = validDelay(command, delay)) != OK)
        return code;

    if((code = validOperation(command, operation)) != OK)
        return code;

    if((code = validArguments(command, arguments)) != OK)
        return code;

    return OK;
}

