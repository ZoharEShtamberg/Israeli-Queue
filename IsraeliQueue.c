//
// Created by Zohar Shtamberg on 17/04/2023.
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//=================================================================
/*type defs:*/
//=================================================================
typedef int (*FriendshipFunction)(void*,void*);
typedef int (*ComparisonFunction)(void*,void*);
typedef enum { ISRAELIQUEUE_SUCCESS, ISRAELIQUEUE_ALLOC_FAILED, ISRAELIQUEUE_BAD_PARAM } IsraeliQueueError;

typedef struct IsraeliNode_t{
	void *item;
	struct IsraeliNode_t *next;
	int friendsBypassed;
	int enemiesBlocked;
} *IsraeliNode;

/*head: pointer to the last obj in line. each obj points to the one in front of it.*/
typedef struct IsraeliQueue_t{
	const IsraeliNode head;
	FriendshipFunction *friendshipFunctionList; //will hold our copy of the func array
	ComparisonFunction comparisonFunction;
	int friendship_th;
	int rivalry_th;
} *IsraeliQueue;

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
IsraeliQueue IsraeliQueueCreate(FriendshipFunction * friendshipFunctionListIn, ComparisonFunction comparisonFunction,
								int friendship_th, int rivalry_th){
	//here we need to duplicate the function list so that we can realloc it.
	if(!comparisonFunction){
		return NULL; //bad parameter
	}

	IsraeliQueue returnQueue = malloc(sizeof(returnQueue)); //'destroy' function should free this memory

	if(!returnQueue){
		return NULL;
	}



}