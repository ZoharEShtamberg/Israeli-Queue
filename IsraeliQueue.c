//
// Created by Zohar Shtamberg on 17/04/2023.
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "IsraeliQueue.h"

//=================================================================
/*typedefs & struct decleration*/
//=================================================================


typedef struct Node_t{
	void *item;
	struct Node_t *next;
} *Node;

/**head: pointer to the first obj in line. each obj points to the one behind it.
 * */
 struct IsraeliQueue_t{
	Node head;
	Node *friendshipFunctionList; //will hold our copy of the func array
	ComparisonFunction comparisonFunction;
	int friendship_th;
	int rivalry_th;
}

//=================================================================
/*internal functions:*/
//=================================================================


//=================================================================
/*create function:*/
//=================================================================

/**@param: friendshipFunctionListIn: a pointer to an array of friendship function pointers.
 * 									 the array should end with a NULL ptr.
 * @param: comparisonFunction: a ptr to comparison function.
 * @param: friendship_th: friendship threshold.
 * @param: rivalry_th: rivalry threshold.
 * */
IsraeliQueue IsraeliQueueCreate(FriendshipFunction * friendshipFunctionList_In, ComparisonFunction comparisonFunction,
								int friendship_th, int rivalry_th){

	//here we need to duplicate the function list to memory we allocated, so that we can realloc it.
	FriendshipFunction *friendshipFunctionList = duplicatFuncArray(friendshipFunctionList_In);

	if(!comparisonFunction){
		return NULL; //bad parameter
	}

	IsraeliQueue returnQueue = malloc(sizeof(returnQueue)); //'destroy' function should free this memory

	if(!returnQueue){
		return NULL; //malloc failed
	}



}