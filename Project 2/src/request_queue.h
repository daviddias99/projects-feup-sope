#include <stdlib.h>
#include <stdbool.h>

#include "types.h"
#include "debug.h"

typedef struct RequestQueueNode
{
  tlv_request_t element;
  struct RequestQueueNode *next;

} RequestQueueNode_t;

typedef struct RequestQueue
{
  RequestQueueNode_t *header;
  size_t size;
  
} RequestQueue_t;

RequestQueue_t *queue_create();
int queue_insert(RequestQueue_t *list, tlv_request_t newElement, int position);
int queue_delete(RequestQueue_t *list, int position);
int queue_push(RequestQueue_t *list, tlv_request_t newElement);
tlv_request_t queue_pop(RequestQueue_t *list);
tlv_request_t queue_retreive(RequestQueue_t *list, int position);
int queue_empty(RequestQueue_t *list);
bool queue_is_empty(RequestQueue_t *list);
