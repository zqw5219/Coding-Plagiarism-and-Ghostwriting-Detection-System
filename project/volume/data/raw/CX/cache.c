#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"
#include "cache.h"


/* cache_init initializes the input cache linked list. */
void cache_init(CacheList *list) {
  list->size=0;
  list->first=NULL;
  list->last=NULL; 
}

/* cache_URL adds a new cached item to the linked list. It takes the
 * URL being cached, a link to the content, the size of the content, and
 * the linked list being used. It creates a struct holding the metadata
 * and adds it at the beginning of the linked list.
 */
void cache_URL(const char *URL, const char *headers, void *item, size_t size, CacheList *list) {
  
  struct CachedItem* newNode=malloc(sizeof(struct CachedItem ));
  newNode->url=strdup(URL);
  newNode->headers=strdup(headers);
  newNode->item_p=item;
  newNode->size=size;
  newNode->prev= NULL;
  newNode->next=NULL;

  //check if the linked list have enough size

  if (size > MAX_OBJECT_SIZE)
    {
      free(newNode->url);
      free(newNode->headers);
      free(newNode->item_p);
      free(newNode);
      return;
    }
  
  //check if the list is 0
  if((list-> size) == 0)
  {
    list->first=newNode;
    list->last=newNode;
    (list->size) += (newNode->size);
    return;
  }

  //the linked list is runing out of space
  list -> size += size;
  while ((list -> size) > MAX_CACHE_SIZE)
  {
    list -> size -= list->last->size;
    if (list->last->prev)
    {
      list->last->prev->next=NULL;
    }
    CachedItem *temNode=list->last->prev;
    free(list->last->url);
    free(list->last->headers);
    free(list->last->item_p);
    free(list->last);  
    list->last=temNode;    
  }
  
  //add new node to the front
  newNode -> next = list -> first;
  list -> first -> prev = newNode;
  list -> first = newNode;
  list->size += size;
  return;
}


/* find iterates through the linked list and returns a pointer to the
 * struct associated with the requested URL. If the requested URL is
 * not cached, it returns null.
 */
CachedItem *find(const char *URL, CacheList *list) {
  CachedItem* firstNode;
  CachedItem* curNode;
  firstNode= list->first;
  curNode=NULL;

  //check if it can find the match
  while (list->first!=NULL)
    {
      if (strcmp(list->first->url,URL)==0)
	{
	  curNode=list->first;
	  break;
	}
      list->first=list->first->next;
    }
  

  // it did not find the node
  if (list->first==NULL)
    {
      list->first=firstNode;
      return NULL;
    }


  //it did find the match
  //the matched node is in the head  
  if (curNode->prev==NULL)
    {
      return curNode;
    }


  //the matched node is in the tail
  if ((curNode->next)==NULL)
    {
      list->last=curNode->prev;
      curNode->prev->next=NULL;
      curNode->prev=NULL;
      curNode->next = firstNode;
      list->first=curNode;
      return curNode;
    }
  

  //if the matched node is in the middle
  curNode->prev->next=curNode->next;
  curNode->next->prev=curNode->prev;
  curNode->prev=NULL;
  curNode->next=firstNode;
  list->first=curNode;
  return curNode;
}


/* frees the memory used to store each cached object, and frees the struct
 * used to store its metadata. */
void cache_destruct(CacheList *list) {
  CachedItem *curNode;
  
  // curNode is the temp node for record
  // the  next node of list->first
  while ((list->first)!=NULL)
    {
      curNode=list->first->next;
      free(list->first->url);
      free(list->first->headers);
      free(list->first->item_p);
      free(list->first);
      list->first=curNode;
      
    }
 return;
}
