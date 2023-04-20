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

/**basic linked list node.
 * */
typedef struct node_t{
	void *m_item;
	struct Node_t *m_next;
} *node;

/**m_head: pointer to the first obj in line. each obj points to the one behind it.
 * 		 a ptr to a linked list of israeli items.
 * m_friendshipFunctionList: ptr to a linked list of ptrs to friendship functions.
 * */
 struct IsraeliQueue_t{
	node m_head;
	node m_friendshipFunctionList;
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
static node duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In);
static void destroyFunctionList(node functionList);
//=================================================================
/*internal functions:*/
//=================================================================
/**@param friendshipFunctionList_In: ptr to an array of FriendshipFunction ptrs.
 * @return: ptr to a linked list of the function ptrs.
 * note: memory should be freed when destroying the israeli queue
 * */
static node duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In){
	node last=NULL, curr=NULL;

	while(friendshipFunctionList_In++){
		curr = malloc(sizeof(*curr));
		if(!curr){
			destroyFunctionList(last);
			return NULL;
		}
		curr->m_next = last; //ironic...
		curr->m_item = *friendshipFunctionList_In;
		last=curr;
	}
	return curr;
}
/**@param functionList:ptr to a linked list of functions.
 * frees allocated memory recursively.*/
static void destroyFunctionList(node functionList){
	if(!functionList){
		return;
	}
	destroyFunctionList(functionList->m_next);
	free(functionList);
	return;
}
/**@param ItemA: ptr to first item (IsraeliItem->m_data)
 * @param ItemB: ptr to second Item
 * @param friendshipFunctions: a head of a linked list, containing FriendshipFunction ptrs.
 * @param friendshipThreshold: usually from IsraeliQueue->m_friendshipThreshold
 * @return: true if they are friends, false otherwise.
 * */
static bool areFriends(void* itemA, void* itemB, node friendshipFunctions, int friendshipThreshold){
	while(friendshipFunctions->m_next){
		if(friendshipThreshold < ( ((FriendshipFunction)(friendshipFunctions->m_item) )(itemA, itemB))){
			return true;
		}
	}
	return false;
}
static bool areEnemies(void* itemA, void* itemB, node friendshipFunctions, int friendshipThreshold, int rivalryThreshold){
	int sum = 0, counter=0,curr;
	while(friendshipFunctions->m_next){
		curr = ( (FriendshipFunction)(friendshipFunctions->m_item) )(itemA, itemB);
		if(curr>friendshipThreshold){
			return false;
		}
		sum +=curr;
		counter++;
	}
	double average = sum/counter;
	if(ceil(average)<rivalryThreshold){//need to check if 'ceil()' is allowed
		return true;
	}
	return false;
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

	if(!comparisonFunction||!friendshipFunctionList_In){
		return NULL; //bad parameter
	}

	node friendshipFunctionList = duplicateFuncArray(friendshipFunctionList_In);

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