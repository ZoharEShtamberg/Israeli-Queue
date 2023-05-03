//
// Created by Zohar Shtamberg on 17/04/2023.
//
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "IsraeliQueue.h"


//=================================================================
/*typedefs & struct declaration*/
//=================================================================

typedef struct IsraeliItem_t{
	void* m_data;
	int m_friendsHelped;
	int m_enemiesBlocked;
	struct IsraeliItem_t* m_next;
} *IsraeliItem;

/**m_head: pointer to the first obj in line. each obj points to the one behind it.
 * 		 a ptr to a linked list of israeli items.
 * m_friendshipFunctionList: ptr to a linked list of ptrs to friendship functions.
 * */
 struct IsraeliQueue_t{
	IsraeliItem m_head;
	FriendshipFunction* m_friendshipFunctionList;
	ComparisonFunction m_comparisonFunction;
	int m_friendshipThreshold;
	int m_rivalryThreshold;
};// a ptr to an Israeli Queue is typedef-ed in header file as 'IsraeliQueue'



//=================================================================
/*function declarations*/
//=================================================================
static FriendshipFunction* duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In);
static void destroyIsraeliList(IsraeliItem head);
static bool areFriends(void* itemA, void* itemB,IsraeliQueue queue);
static bool areEnemies(void* itemA, void* itemB, IsraeliQueue queue);
static IsraeliItem findFriend(IsraeliQueue queue, void* item);
static void insertBehind(IsraeliItem behind, IsraeliItem front);
//=================================================================
/*internal functions:*/
//=================================================================
/**@param friendshipFunctionList_In: ptr to an array of FriendshipFunction ptrs.
 * @return: ptr to a linked list of the function ptrs.
 * note: memory should be freed when destroying the israeli queue
 * */
static FriendshipFunction* duplicateFuncArray(FriendshipFunction *friendshipFunctionList_In){
	assert(friendshipFunctionList_In);
	int size = 0;
	FriendshipFunction* temp =friendshipFunctionList_In;
	for(;*temp;temp++){
		size++;
	}
	FriendshipFunction* newList = malloc((size+1)* sizeof(FriendshipFunction));
	for(int i=0;i<size;i++){
		newList[i]=friendshipFunctionList_In[i];
	}
	newList[size]=NULL;
	return newList;
}

/**@param ItemA: ptr to first item (IsraeliItem->m_data)
 * @param ItemB: ptr to second Item
 * @param IsraeliQueue: the Israeli Queue containing the items.
 * @return: true if they are friends, false otherwise.
 * */
static bool areFriends(void* itemA, void* itemB,IsraeliQueue queue){
	FriendshipFunction* list = queue->m_friendshipFunctionList;
	for(;*list;list++){
		if((*list)(itemA, itemB) > queue->m_friendshipThreshold){
			return true;
		}
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
	FriendshipFunction *list = queue->m_friendshipFunctionList;
	while(list){
		curr = (*list)(itemA, itemB);
		if(curr > queue->m_friendshipThreshold){
			return false;
		}
		sum +=curr;
		counter++;
		list++;
		average = ((double)sum/counter);//inside the loop to avoid dividing by 0
	}
	if(ceil(average) >= queue->m_rivalryThreshold){//need to check if 'ceil()' is allowed
		return false;
	}
	return true;
}
/**@param queue: an Israeli queue.
 * @param item: item to find its place in queue.
 * @return IsraeliItem ptr to insert behind.
 * @note: increments stats for friend helped\enemy blocked.
 * should return NULL if and only if head==NULL.
 * */
static IsraeliItem findFriend(IsraeliQueue queue, void* item){
	IsraeliItem curr = queue->m_head, friend=NULL, enemy=NULL;
	if(curr==NULL){	//to avoid de-referencing NULL
		return NULL;
	}
	while(curr){	//could be while(1) for all that matters
		if(areFriends(curr->m_data,item,queue) && curr->m_friendsHelped<FRIEND_QUOTA){
			friend = curr;//found a friend
			for(enemy=curr->m_next ; enemy ; enemy=enemy->m_next){
				if(areEnemies(enemy->m_data, item, queue) &&
				   		enemy->m_enemiesBlocked < RIVAL_QUOTA){ //found an enemy
					enemy->m_enemiesBlocked++;
					curr=enemy;
					break; //enemy blocked
				}
			}
			if(!enemy){ //means no enemy blocked
				friend->m_friendsHelped++;
				return friend;
			}
		}
		if(!curr->m_next){//it's the last one and not a friend
			return curr;
		}
		curr=curr->m_next;
	}
	assert( 0 && "broke loop in find friend");
	/* this is a weird assert, but I want to prevent this memory leak from happening.
	 * this line should not be reached. it's ugly, but I did it that way
	 * because of the stupid requirement for friends who are last in line to count as a friend insertion.
	 * */
	return NULL;
}


/**@param: behind: node to insert to list
 * @param front: node to insert behind
 * @note: ...-> front -> behind ->...
 * */
static void insertBehind(IsraeliItem behind, IsraeliItem front){
	assert(front!=NULL);
	assert(behind!=NULL);
	IsraeliItem temp = front->m_next;
	front->m_next = behind;
	behind->m_next = temp;
}

/**@param head: head of israeli item linked list
 * frees allocated memory recursively.*/
static void destroyIsraeliList(IsraeliItem head){
	if(!head){
		return;
	}
	destroyIsraeliList(head->m_next);
	free(head);
}
/** copies all fields of 'source' into 'target', except 'm_next'.
 * both ptrs should be valid. */
static void copyIsraeliItem(IsraeliItem source, IsraeliItem target){
	assert(source && target);
	target->m_data=source->m_data;
	target->m_friendsHelped=source->m_friendsHelped;
	target->m_enemiesBlocked=source->m_enemiesBlocked;
}
/**@param source: head of linked israeli items.
 * @return: head to a new israeli item list. NULL for error.
 * calls it self recursively, allocating for each item.
 * @note: should not receive NULL!
 * */
static IsraeliItem duplicateItemList(IsraeliItem source){
	assert(source);
	if(source->m_next==NULL){	//means its the last one
		IsraeliItem last = malloc(sizeof(*last));
		if(!last){
			return NULL;
		}
		last->m_next = NULL;
		copyIsraeliItem(source, last);
		return last;
	}
	IsraeliItem curr = malloc(sizeof(*curr));
	if(!curr){
		return NULL;
	}
	curr->m_next = duplicateItemList(source->m_next);
	if(curr->m_next == NULL){ //alloc failed down the line
		free(curr);
		return NULL;
	}
	copyIsraeliItem(source, curr);
	return curr;
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

	FriendshipFunction *friendshipFunctionList = duplicateFuncArray(friendshipFunctionList_In);

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
	if(!queue||!data){
		return ISRAELIQUEUE_BAD_PARAM;
	}
	IsraeliItem friend = findFriend(queue, data);// takes care of thresholds!
	IsraeliItem newItem = malloc(sizeof(*newItem));
	if(!newItem){
		return ISRAELIQUEUE_ALLOC_FAILED;
	}
	newItem->m_friendsHelped = newItem->m_enemiesBlocked = 0;
	newItem->m_data = data;
	newItem->m_next=NULL;

	if(friend==NULL){
		queue->m_head=newItem;
		return ISRAELIQUEUE_SUCCESS;
	}
	insertBehind(newItem, friend);
	return ISRAELIQUEUE_SUCCESS;
}
//=================================================================
/*destroy function:*/
//=================================================================
void IsraeliQueueDestroy(IsraeliQueue queue){
	free(queue->m_friendshipFunctionList); //destroy func arr
	destroyIsraeliList(queue->m_head);		//destroy israeli items in queue
	free(queue);		//destroy queue struct
}
//=================================================================
/*dequeue function:*/
//=================================================================
/**@param queue: queue
 * @return: ptr to data or NULL for empty queue
 * frees items memory.*/
void* IsraeliQueueDequeue(IsraeliQueue queue){
	IsraeliItem head = queue->m_head;
	void* data = head->m_data;
	queue->m_head=head->m_next;
	free(head);
	return data;
}
//=================================================================
/*add friendship function:*/
//=================================================================
/**@param queue: queue
 * @param newFunction: ptr to a friendship function to add to the queue
 * */
IsraeliQueueError IsraeliQueueAddFriendshipMeasure(IsraeliQueue queue, FriendshipFunction newFunction){
	if(!queue||!newFunction){
		return ISRAELIQUEUE_BAD_PARAM;
	}
	int funcNum = 0;
	FriendshipFunction* list = queue->m_friendshipFunctionList;
	for(; *list; list++){
		funcNum++;
	}
	list=queue->m_friendshipFunctionList;
	FriendshipFunction *newList = malloc((funcNum+2) * sizeof(FriendshipFunction));
	if(!newList){
		return ISRAELIQUEUE_ALLOC_FAILED;
	}
	int i=0;
	for(;i<funcNum;i++){
		newList[i]=list[i];
	}
	newList[i++]=newFunction;
	newList[i]=NULL;
	queue->m_friendshipFunctionList = newList;
	free(list);
	return ISRAELIQUEUE_SUCCESS;
}
//=================================================================
/*threshold update functions:*/
//=================================================================

/**pretty straight forward these two*/
IsraeliQueueError IsraeliQueueUpdateFriendshipThreshold(IsraeliQueue queue, int threshold){
	if(!queue){
		return ISRAELIQUEUE_BAD_PARAM;
	}
	queue->m_friendshipThreshold=threshold;
	return ISRAELIQUEUE_SUCCESS;
}

IsraeliQueueError IsraeliQueueUpdateRivalryThreshold(IsraeliQueue queue, int threshold){
	if(!queue){
		return ISRAELIQUEUE_BAD_PARAM;
	}
	queue->m_rivalryThreshold=threshold;
	return ISRAELIQUEUE_SUCCESS;
}
//=================================================================
/*queue size function:*/
//=================================================================
/**@param queue:queue
 * @note: it counts them.*/
int IsraeliQueueSize(IsraeliQueue queue){
	int size = 0;
	for(IsraeliItem head = queue->m_head ; head ; head=head->m_next){
		size++;
	}
	return size;
}
//=================================================================
/*search function:*/
//=================================================================
/**@param queue: queue
 * @param data: ptr to data to search for in line*/
bool IsraeliQueueContains(IsraeliQueue queue, void *data){
	if(!queue){
		return false;
	}
	ComparisonFunction compare = queue->m_comparisonFunction;
	for(IsraeliItem head = queue->m_head ; head ; head=head->m_next){
		if(compare(data, head->m_data)){
			return true;
		}
	}
	return false;
}
//=================================================================
/*clone function:*/
//=================================================================
IsraeliQueue IsraeliQueueClone(IsraeliQueue q){
	IsraeliQueue clone = IsraeliQueueCreate(q->m_friendshipFunctionList,
											q->m_comparisonFunction,
											q->m_friendshipThreshold, q->m_rivalryThreshold);
	if(!clone){
		return NULL;	//'create' failed
	}
	if(q->m_head==NULL){
		return clone;	//'copyitemlist' must not receive NULL!
	}
	IsraeliItem newHead = duplicateItemList(q->m_head);
	if(!newHead){	//alloc error in copy item list
		IsraeliQueueDestroy(clone);
		return NULL;
	}
	clone->m_head = newHead;
	return clone;
}
/**@param queue: israeli queue
 * i really hope this is what they meant, but at this point who knows?*/
//=================================================================
/*improve positions function:*/
//=================================================================
IsraeliQueueError IsraeliQueueImprovePositions(IsraeliQueue queue){
	if(!queue){
		return ISRAELIQUEUE_BAD_PARAM;
	}
	int size = IsraeliQueueSize(queue);
	if(size<2){
		return ISRAELIQUEUE_SUCCESS;
	}
	IsraeliItem *list = malloc(size*sizeof(IsraeliItem)); //note: sizeof the ptr itself.
	if(!list){
		return ISRAELIQUEUE_ALLOC_FAILED;
	}
	IsraeliItem head=queue->m_head;
	for(int i=0;i<size;i++){
		assert(head);
		list[i]=head;
		head=head->m_next;
	}
	for(int i=size;i>1;i--){	//not including the first one
		list[i-1]->m_next = list[i]->m_next;
		IsraeliItem friend = findFriend(queue, list[i]->m_data);
		insertBehind(friend, list[i]);
	}
	queue->m_head = list[0]->m_next;	//after trying to improve the 1st item in line, the one after it is the new head
	IsraeliItem friend = findFriend(queue, list[0]->m_data);
	insertBehind(friend, list[0]);
	free(list);
	return ISRAELIQUEUE_SUCCESS;
}
//=================================================================
/*merge function:*/
//=================================================================
IsraeliQueue IsraeliQueueMerge(IsraeliQueue* queueList_In ,ComparisonFunction comparefunction){
	if(!queueList_In||!comparefunction){
		return NULL;	//bad parameters
	}
	unsigned int size = 0;
	while(queueList_In[size]){
		size++;
	}
	IsraeliQueue *newList = malloc(size*sizeof(IsraeliQueue));	//array of ptrs
	if(!newList){
		return NULL;	//alloc fail
	}
	for(int i=0;i<size;i++){
		newList[i] = IsraeliQueueClone(queueList_In[i]);
	}

}