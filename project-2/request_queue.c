#include "request_queue.h"

int queue_insert(RequestQueue_t *list, tlv_request_t newElement, int position)
{

    // alocate memory for a new node with the given tlv_request_t
    RequestQueueNode_t *newNode = malloc(sizeof(RequestQueueNode_t));
    newNode->element = newElement;
    newNode->next = NULL;
    RequestQueueNode_t *currentNode = list->header->next;

    int i = 0;

    if (position == 0)
    {
        newNode->next = list->header->next;
        list->header->next = newNode;
        list->size++;
        return 0;
    }

    while (currentNode != NULL)
    {

        if (i == position - 1)
        {

            RequestQueueNode_t *nextNode = currentNode->next;
            currentNode->next = newNode;
            newNode->next = nextNode;
            list->size++;
            return 0;
        }

        currentNode = currentNode->next;
        i++;
    }

    RequestQueueNode_t *nextNode = currentNode->next;
    currentNode->next = newNode;
    newNode->next = nextNode;
    list->size++;

    return 0;
}

int queue_delete(RequestQueue_t *list, int position)
{

    RequestQueueNode_t *currentNode = list->header->next;

    int i = 0;

    if (position == 0)
    {

        RequestQueueNode_t *deletedNode = currentNode;
        list->header->next = deletedNode->next;
        free(deletedNode);
        list->size--;
        return 0;
    }

    while (currentNode != NULL)
    {

        if (i == position - 1)
        {

            RequestQueueNode_t *deletedNode = currentNode->next;
            currentNode->next = deletedNode->next;
            free(deletedNode);
            list->size--;
            return 0;
        }

        currentNode = currentNode->next;
        i++;
    }

    return -1;
}

int queue_push(RequestQueue_t *list, tlv_request_t newElement)
{

    return queue_insert(list, newElement, list->size);
}

tlv_request_t queue_pop(RequestQueue_t *list)
{

    tlv_request_t result = queue_retreive(list, 0);
    queue_delete(list, 0);

    return result;
}

tlv_request_t queue_retreive(RequestQueue_t *list, int position)
{

    RequestQueueNode_t *currentNode = list->header->next;

    int i = 0;

    while (currentNode != NULL)
    {

        if (i == position)
        {

            return currentNode->element;
        }

        currentNode = currentNode->next;
        i++;
    }

    tlv_request_t dummyResult;

    return dummyResult;
}

int queue_empty(RequestQueue_t *list)
{

    while (list->size != 0)
    {

        queue_delete(list, 0);
    }
    return 0;
}

bool queue_is_empty(RequestQueue_t *list)
{

    if (list->header->next == NULL)
        return true;
    else
        return false;
}