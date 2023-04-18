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
typedef enum { ISRAELIQUEUE_SUCCESS, ISRAELIQUEUE_ALLOC_FAILED,
			   ISRAELIQUEUE_BAD_PARAM, ISRAELI_QUEUE_ERROR } IsraeliQueueError;

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
/**
 *
 * @param friendshipFunctionList_In: ptr to array of friendship-functions to duplicate
 * @return :ptr to a fresh copy of the array
 * note: the last item in the copy should also be a NULL ptr.
 * memory allocated for the copy will be freed in 'destroy' function.
 */
FriendshipFunction* duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In){

	if (!friendshipFunctionList_In){
		return NULL; // invalid pointer
	}

	int length=sizeof(sizeof(friendshipFunctionList_In)/(sizeof(FriendshipFunction*)));
	FriendshipFunction *NewFunctionsArray=malloc(sizeof(FriendshipFunction)*(length+1));

	if (NewFunctionsArray==NULL){
		return NULL; // memory allocation failed
	}
	for(int i=0;i<length;i++){
		NewFunctionsArray[i]=friendshipFunctionList_In[i];
	}	
	return NewFunctionsArray;

}

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