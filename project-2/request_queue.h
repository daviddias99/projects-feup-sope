
#include "stdlib.h"
#include "types.h"
#include "stdbool.h"


typedef struct RequestQueueNode{

  tlv_request_t element;                      
  struct RequestQueueNode* next;   

}RequestQueueNode;


typedef struct RequestQueue{

    RequestQueueNode* header;     
    size_t size;                    
    
}RequestQueue;


int queue_insert(RequestQueue* list,tlv_request_t newElement, int position);
int queue_delete(RequestQueue* list, int position);
int queue_push(RequestQueue* list, tlv_request_t newElement);
tlv_request_t queue_pop(RequestQueue* list);
tlv_request_t queue_retreive(RequestQueue* list, int position);
int queue_empty(RequestQueue *list);
bool queue_is_empty(RequestQueue* list);

