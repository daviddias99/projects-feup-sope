#pragma once

#define MAX_BANK_OFFICES 99
#define MAX_BANK_ACCOUNTS 4096
#define MIN_BALANCE 1UL
#define MAX_BALANCE 1000000000UL
#define ADMIN_ACCOUNT_ID 0
#define MAIN_THREAD_ID 0

#define MIN_PASSWORD_LEN 8
#define MAX_PASSWORD_LEN 20
#define HASH_LEN 64
#define SALT_LEN 64

#define MAX_OP_DELAY_MS 99999

#define WIDTH_ID 5
#define WIDTH_DELAY 5
#define WIDTH_ACCOUNT 4
#define WIDTH_BALANCE 10
#define WIDTH_OP 8
#define WIDTH_RC 12
#define WIDTH_HASH 5
#define WIDTH_TLV_LEN 3
#define WIDTH_STARTEND 5

#define SERVER_LOGFILE "slog.txt"
#define USER_LOGFILE "ulog.txt"

#define SERVER_FIFO_PATH "/tmp/secure_srv"
#define USER_FIFO_PATH_PREFIX "/tmp/secure_"
#define USER_FIFO_PATH_LEN (sizeof(USER_FIFO_PATH_PREFIX) + WIDTH_ID + 1)

#define FIFO_TIMEOUT_SECS 30

// -----------

#define READ 0
#define WRITE 1
#define SHA256_SIZE                   64
#define REQUEST_FIFO_PERM             0660
#define RESPONSE_FIFO_PERM            0660
#define ERROR_ACCOUNT_LIMIT_EXCEEDED  -2
#define ERROR_ACCOUNT_ID              (MAX_BANK_ACCOUNTS +1)

#define CANCEL_ALARM                  0
#define UNUSED(x) (void)(x)

#define MS_TO_US(x)         (x * 1000)

#define OK                  0
#define SRV_DOWN            1
#define SRV_TIMEOUT         2

#define NEG_ACCOUNT_ID      1
#define LRG_ACCOUNT_ID      2
#define SRT_PASSWORD        3
#define LNG_PASSWORD        4
#define BAD_PASSWORD        5
#define NEG_DELAY           6
#define LNG_DELAY           7
#define INV_OPERATION       8
#define INV_NEW_ACCOUNT_ID  9
#define SRT_BALANCE         10
#define LRG_BALANCE         11
#define INV_NEW_PASSWORD    12
#define INV_TRS_ACCOUNT     13
#define SML_TRSF_AMOUNT     14
#define LRG_TRSF_AMOUNT     15
#define INV_OP_ARGUMENTS    16
#define LOG_REQUEST_ERROR   17
#define LOG_REPLY_ERROR     18
#define OTHER_ERROR         19
