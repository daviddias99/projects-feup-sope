#ifndef USER_UTIL_H
#define USER_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../sope.h"
#include "../constants.h"
#include "../types.h"
#include "../debug.h"
 
/**
 * @brief Used to handle SIGALRM thrown when server takes
 * more than 30 seconds to respond. Assures that the error is
 * recorded on the user logfile and that all FIFOs are closed
 * as well as unlinking the created reply FIFO.
 * 
 * @param signo     Thrown signal
 */
void alarm_handler(int signo);

/**
 * @brief Used to handle SIGINT in case the user presses CTRL+C
 * after the FIFOs are opened. It assures that they are closed
 * and unlinked in the case of the reply FIFO.
 * 
 * @param signo     Thrown signal
 */
void signal_handler(int signo);

/**
 * @brief Used to print an error message to STDOUT. The error message
 * will be the string of index 'error' on the array ERROR_MESSAGES.
 * 
 * @param error     Index of the error message on the array ERROR_MESSAGES
 */
void printErrorMessage(int error);

/**
 * @brief Copies the given user_command_t struct to the global struct
 * of the same type.
 * 
 * @param user_command  Command struct to be copied 
 */
int readCommand(user_command_t* user_command);

/**
 * @brief Changes the handler of both SIGALRM and SIGINT to be the 
 * function 'alarm_handler()' and signal_handler, respectively.
 * 
 * @return int      Returns 0 if both handlers were set successfuly,
 *                  1 if SIGALRM failed, 2 if SIGINT failed
 */
int setupHandlers();

/**
 * @brief Attempts to open the server request FIFO. If unsuccessful
 * records the error in the user Logfile.
 * 
 * @return int      Returns 0 if successful, 1 otherwise
 */
int setupRequestFIFO();

/**
 * @brief Creates the FIFO used to read the server response to the
 * sent request and opens it.
 * 
 * @return int      Returns 0 if successful, non 0 otherwise
 */
int setupResponseFIFO();

/**
 * @brief Closes FIFO used to send the request to the server.
 * 
 * @return int      Returns 0 if successful, 1 otherwise
 */
int closeRequestFIFO();

/**
 * @brief Closes FIFO used to read the response from the server.
 * 
 * @return int      Returns 0 if successful, 1 otherwise
 */
int closeResponseFIFO();

/**
 * @brief Calls both functions 'closeRequestFIFO()' and 'closeResponseFIFO()'.
 * Unlinks the name used by the previously created response FIFO.
 * 
 * @return int      Returns 0 if successfully unlinked, 1 otherwise
 */
int closeComunication();

/**
 * @brief Used to record both the request and reply on to the user
 * Logfile.
 * 
 * @param request   Request of the user operation
 * @param reply     Reply from the user operation
 * 
 * @return int      Returns 0 if successful, non 0 otherwise
 */ 
int recordOperation(tlv_request_t* request, tlv_reply_t* reply);

/**
 * @brief Fills the struct req_create_account_t with the appropriate
 * values and sets the create parameter of the given 'request_value'
 * to it.
 * 
 * @param request_value     Struct to be assigned the calculated parameter       
 */
int formatReqCreateAccount(req_value_t* request_value);

/**
 * @brief Fills the struct req_transfer_t with the appropriate
 * values and sets the transfer parameter of the given 'request_value'
 * to it.
 * 
 * @param request_value     Struct to be assigned the calculated parameter       
 */
int formatReqTransfer(req_value_t* request_value);

/**
 * @brief Fills the struct req_header_t passed as argument with the
 * correct values based on the user command.
 * 
 * @param request_value     Struct to be assigned the parameters       
 */
int formatReqHeader(req_header_t* header);

/**
 * @brief Fills the struct req_value_t passed as argument with the
 * correct values based on the user command.
 * 
 * @param request_value     Struct to be assigned the parameters
 */
int formatReqValue(req_value_t* request_value);

/**
 * @brief Fills the struct tlv_request_t passed as argument with the
 * correct values based on the user command. Calculates the appropriate
 * length to be sent according to the TLV message type.
 * 
 * @param request_value     Struct to be assigned the parameters
 */
int formatRequest(tlv_request_t* request);

/**
 * @brief Writes the request passed as argument to the request FIFO
 * used to communicate with the server.
 * 
 * @param request       Request to be writen
 * 
 * @return int          Returns 0 if successful, 1 otherwise
 */
int sendRequest(tlv_request_t* request);

/**
 * @brief Reads the reply sent by the user throught the response FIFO.
 * This is done by first read the reply type followed by the reply length
 * and then it's value, reading only the given length of bytes. Sets the
 * given tlv_reply_t struct to the read reply.
 * 
 * @param reply         Reply struct to be filled
 * 
 * @return int          Returns 0 if successful, 1 otherwise
 */
int readReply(tlv_reply_t *reply);

/**
 * @brief Sets the balance parameter of the given rep_value_t struct
 * to the filled rep_balance_t struct.
 * 
 * @param reply_value   Struct to be filled with the calculated parameter
 */
int formatRepBalanceAccount(rep_value_t* reply_value);

/**
 * @brief Sets the transfer parameter of the given rep_value_t struct
 * to the filled rep_transfer_t struct.
 * 
 * @param reply_value   Struct to be filled with the calculated parameter
 */
int formatRepTransfer(rep_value_t* reply_value);

/**
 * @brief Sets the shutdown parameter of the given rep_value_t struct
 * to the filled rep_shutdown_t struct.
 * 
 * @param reply_value   Struct to be filled with the calculated parameter
 */
int formatRepShutdown(rep_value_t* reply_value);

/**
 * @brief Fills the given rep_header_t struct with the appropriate
 * values.
 * 
 * @param header        Struct to be filled
 * @param ret_code      Return code given to the struct
 */
int formatRepHeader(rep_header_t* header, int ret_code);

/**
 * @brief Fills the given rep_value_t struct with the correct values
 * depending on the command operation.      
 * 
 * @param reply_value   Struct to be filled
 * @param ret_code      Return code of the reply
 */
int formatRepValue(rep_value_t* reply_value, int ret_code);

/**
 * @brief Fills the struct tlv_request_t passed as argument with the
 * correct values based on the user command and given return code.
 * Calculates the appropriate length to be sent according to the TLV
 * message type.
 * 
 * @param request_value     Struct to be assigned the parameters
 * @param ret_code          Reply's return code
 */
int formatReply(tlv_reply_t* reply, int ret_code);

/**
 * @brief Records the given error on the user Logfile creating the
 * necessary request and reply structs.
 * 
 * @param ret_code          Reply's return code
 */
int recordError(int ret_code);

/**
 * @brief Sends the request passed as argument to the server. Sets up
 * an alarm for FIFO_TIMEOUT_SECS and await's for a reply. If the reply
 * is read before the alarm thrown, it cancel's the alarm and closes
 * all FIFO's used for comunication with the server.
 * 
 * @param request       Request to be sent to the server
 * @param reply         Reply's struct to be read from the server
 * 
 * @return int          Returns 0 if successful, non 0 otherwise
 */
int waitResponse(tlv_request_t* request, tlv_reply_t* reply);

#endif
