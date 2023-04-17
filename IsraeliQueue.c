//
// Created by Zohar Shtamberg on 17/04/2023.
//=================================================================
/*type defs:*/
//=================================================================
typedef int (*FriendshipFunction)(void*,void*);
typedef int (*ComparisonFunction)(void*,void*);

typedef struct IsraeliNode_t{
	void *item;
	struct IsraeliNode_t *next;
	int friendsBypassed;
	int enemiesBlocked;
} *IsraeliNode;

/*head: pointer to the last obj in line. each obj points to the one in front of it.*/
typedef struct IsraeliQueue_t{
	IsraeliNode head;
	FriendshipFunction *friendshipFunctionList;
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
/*TODO: figure out how to implement the friendshipfunctions list (probably linked list)
 *
 * */

IsraeliQueue IsraeliQueueCreate(FriendshipFunction * friendshipFunction, ComparisonFunction comparisonFunction,
								int friendship_th, int rivalry_th){


}