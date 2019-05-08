#include "request_queue.h"

int queue_insert(RequestQueue *list, tlv_request_t newElement, int position)
{

    // alocate memory for a new node with the given tlv_request_t
    RequestQueueNode *newNode = malloc(sizeof(RequestQueueNode));
    newNode->element = newElement;
    newNode->next = NULL;
    RequestQueueNode *currentNode = list->header->next;

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

            RequestQueueNode *nextNode = currentNode->next;
            currentNode->next = newNode;
            newNode->next = nextNode;
            list->size++;
            return 0;
        }

        currentNode = currentNode->next;
        i++;
    }

    RequestQueueNode *nextNode = currentNode->next;
    currentNode->next = newNode;
    newNode->next = nextNode;
    list->size++;

    return 0;
}

int queue_delete(RequestQueue *list, int position)
{

    RequestQueueNode *currentNode = list->header->next;

    int i = 0;

    if (position == 0)
    {

        RequestQueueNode *deletedNode = currentNode;
        list->header->next = deletedNode->next;
        free(deletedNode);
        list->size--;
        return 0;
    }

    while (currentNode != NULL)
    {

        if (i == position - 1)
        {

            RequestQueueNode *deletedNode = currentNode->next;
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

int queue_push(RequestQueue *list, tlv_request_t newElement)
{

    return queue_insert(list, newElement, list->size);
}

tlv_request_t queue_pop(RequestQueue *list)
{

    tlv_request_t result = queue_retreive(list, 0);
    queue_delete(list, 0);

    return result;
}

tlv_request_t queue_retreive(RequestQueue *list, int position)
{

    RequestQueueNode *currentNode = list->header->next;

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

int queue_empty(RequestQueue *list)
{

    while (list->size != 0)
    {

        queue_delete(list, 0);
    }
    return 0;
}

bool queue_is_empty(RequestQueue *list)
{

    if (list->header->next == NULL)
        return true;
    else
        return false;
}