//
// Created by Zohar Shtamberg on 17/04/2023.
//
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "IsraeliQueue.h"


//=================================================================
/*typedefs & struct declaration*/
//=================================================================

typedef struct Node_t{
	void *m_item;
	struct node_t *m_next;
}*Node;

/**m_head: pointer to the first obj in line. each obj points to the one behind it.
 * 		 a ptr to a linked list of israeli items.
 * m_friendshipFunctionList: ptr to a linked list of ptrs to friendship functions.
 * */
 struct IsraeliQueue_t{
	Node m_head;
	Node m_friendshipFunctionList;
	ComparisonFunction m_comparisonFunction;
	int m_friendshipThreshold;
	int m_rivalryThreshold;
};// a ptr to an Israeli Queue is typedef-ed in header file as 'IsraeliQueue'

typedef struct IsraeliItem_t{
	 void* m_data;
	 int m_friendsHelped;
	 int m_enemiesBlocked;
 }*IsraeliItem;

//=================================================================
/*function declarations*/
//=================================================================
static Node duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In);
static void destroyLinkedList(Node list);
static bool areFriends(void* itemA, void* itemB,IsraeliQueue queue);
static bool areEnemies(void* itemA, void* itemB, IsraeliQueue queue);
static Node findFriend(IsraeliQueue queue, void* item);
static void insertBehind(Node behind, Node front);
//=================================================================
/*internal functions:*/
//=================================================================
/**@param friendshipFunctionList_In: ptr to an array of FriendshipFunction ptrs.
 * @return: ptr to a linked list of the function ptrs.
 * note: memory should be freed when destroying the israeli queue
 * */
static Node duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In){
	Node last=NULL, curr=NULL;

	while(friendshipFunctionList_In++){
		curr = malloc(sizeof(*curr));
		if(!curr){
			destroyLinkedList(last);
			return NULL;
		}
		curr->m_next = last; //ironic...
		curr->m_item = *friendshipFunctionList_In;
		last=curr;
	}
	return curr;
}

/**@param ItemA: ptr to first item (IsraeliItem->m_data)
 * @param ItemB: ptr to second Item
 * @param IsraeliQueue: the Israeli Queue containing the items.
 * @return: true if they are friends, false otherwise.
 * */
static bool areFriends(void* itemA, void* itemB,IsraeliQueue queue){
	Node list = queue->m_friendshipFunctionList;
	while(list){
		if(queue->m_friendshipThreshold < ( ((FriendshipFunction)list->m_item) )(itemA, itemB)){
			return true;
		}
		list = list->m_next;
	}
	return false;
}
/**@param ItemA: ptr to first item (IsraeliItem->m_data)
 * @param ItemB: ptr to second Item
 * @param IsraeliQueue: the Israeli Queue containing the items.
 * @return: true if they are enemies, false otherwise.
 * */
static bool areEnemies(void* itemA, void* itemB, IsraeliQueue queue){
	int sum = 0, counter=0,curr;
	double average;
	Node list = queue->m_friendshipFunctionList;
	while(list){
		curr = ((FriendshipFunction)list->m_item)(itemA, itemB);
		if(curr > queue->m_friendshipThreshold){
			return false;
		}
		sum +=curr;
		counter++;
		list = list->m_next;
		average = ((double)sum/counter);//inside the loop to avoid dividing by 0
	}
	if(ceil(average) >= queue->m_rivalryThreshold){//need to check if 'ceil()' is allowed
		return false;
	}
	return true;
}
/**@param queue: an Israeli queue.
 * @param item: item to find its place in queue.
 * @return Node ptr to insert behind.
 * @note: increments stats for friend helped\enemy blocked.
 * */
static Node findFriend(IsraeliQueue queue, void* item){
	Node curr = queue->m_head, friend=NULL;
	if(curr==NULL){	//to avoid de-referencing NULL
		return curr;
	}
	while(curr->m_next){
		if(areFriends(((IsraeliItem)curr->m_item)->m_data, item, queue)){
			friend = curr;//found a friend
			while(curr){
				if(areEnemies(((IsraeliItem)curr->m_item)->m_data, item, queue) &&
				   ((IsraeliItem)curr->m_item)->m_enemiesBlocked< RIVAL_QUOTA){
					((IsraeliItem)curr->m_item)->m_enemiesBlocked++;
					friend=NULL;
					break;
				}
				curr = curr->m_next;
			}
		}
		curr = curr->m_next;
	}
	if(friend && ((IsraeliItem)friend->m_item)->m_friendsHelped < FRIEND_QUOTA){
		((IsraeliItem)friend->m_item)->m_friendsHelped++;
		return friend;
	}
	return curr;
}
/**@param: behind: node to insert to list
 * @param front: node to insert behind
 * @note: ...-> front -> behind ->...
 * */
void insertBehind(Node behind, Node front){
	assert(front!=NULL);
	Node temp = front->m_next;
	front->m_next = behind;
	behind->m_next = temp;
}

/**@param list:ptr to a linked list.
 * frees allocated memory recursively.*/
void destroyLinkedList(Node list){
	if(!list){
		return;
	}
	destroyLinkedList(list->m_next);
	free(list);
}

//=================================================================
/*create function:*/
//=================================================================

/**@param: friendshipFunctionListIn: a pointer to an array of friendship function pointers.
 * 									 the array should end with a NULL ptr.
 * @param: comparisonFunction: a ptr to comparison function.
 * @param: friendship_th: friendship threshold.
 * @param: rivalry_th: rivalry threshold.
 * @return: ptr to an Israeli queue.
 * */
IsraeliQueue IsraeliQueueCreate(FriendshipFunction *friendshipFunctionList_In, ComparisonFunction comparisonFunction,
								int friendship_th, int rivalry_th){

	if(!comparisonFunction){
		return NULL; //bad parameter
	}

	Node friendshipFunctionList = duplicateFuncArray(friendshipFunctionList_In);

	IsraeliQueue returnQueue = malloc(sizeof(*returnQueue)); //'destroy' function should free this memory

	if(!returnQueue){
		return NULL; //malloc failed
	}
	returnQueue->m_comparisonFunction = comparisonFunction;
	returnQueue->m_friendshipFunctionList = friendshipFunctionList;
	returnQueue->m_head = NULL;
	returnQueue->m_friendshipThreshold = friendship_th;
	returnQueue->m_rivalryThreshold = rivalry_th;

	return returnQueue;
}
//=================================================================
/*enqueue function:*/
//=================================================================
IsraeliQueueError IsraeliQueueEnqueue(IsraeliQueue queue, void *data){
	Node friend = findFriend(queue, data);// takes care of thresholds!
	IsraeliItem newItem = malloc(sizeof(*newItem));
	if(!newItem){
		return ISRAELIQUEUE_ALLOC_FAILED;
	}
	Node newNode = malloc(sizeof(*newNode));
	if(!newNode){
		free(newItem);
		return ISRAELIQUEUE_ALLOC_FAILED;
	}
	newItem->m_friendsHelped = newItem->m_enemiesBlocked = 0;
	newItem->m_data = data;
	newNode->m_item = newItem;
	newNode->m_next=NULL;
	if(friend==NULL){
		queue->m_head=newNode;
		return ISRAELIQUEUE_SUCCESS;
	}
	insertBehind(newNode, friend);
	return ISRAELIQUEUE_SUCCESS;
}
//=================================================================
/*destroy function:*/
//=================================================================
void IsraeliQueueDestroy(IsraeliQueue queue){
	destroyLinkedList(queue->m_friendshipFunctionList);	//destroy function list
	Node copy = queue->m_head;
	while(copy){	//destroy 'israeli items' in the linked list
		free(copy->m_item);
		copy=copy->m_next;
	}
	destroyLinkedList(queue->m_head);	// destroy queue nodes
	free(queue);		//destroy queue struct
}


void* IsraeliQueueDequeue(IsraeliQueue queue){

}