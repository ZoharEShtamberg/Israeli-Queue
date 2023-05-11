//
// Created by Zohar Shtamberg on 17/04/2023.
//
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "IsraeliQueue.h"

#define ABS(X) (X) >= 0 ? (X): -1*(X)


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
static IsraeliItem* listQueue(IsraeliItem head, int size);
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
	if(!newList){
		return NULL;	//alloc failed
	}
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
	int sum = 0, counter=0,currentLevel;
	double average;
	FriendshipFunction *list = queue->m_friendshipFunctionList;
	while(*list){
		currentLevel = (*list)(itemA, itemB);
		if(currentLevel > queue->m_friendshipThreshold){
			return false;
		}
		sum +=currentLevel;
		counter++;
		list++;
		average = ((double)sum/counter);//inside the loop to avoid dividing by 0
	}
	if(average >= queue->m_rivalryThreshold){//need to check if 'ceil()' is allowed
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
	assert(queue);
	IsraeliItem current = queue->m_head, friend=NULL;
	while(current){	//could be while(1) for all that matters
		if(areFriends(current->m_data,item,queue) && current->m_friendsHelped<FRIEND_QUOTA){
			friend = current;//found a friend
			//check if curr is last
			for(current=current->m_next ; current ; current=current->m_next){
				if(areEnemies(current->m_data, item, queue) &&
				   		current->m_enemiesBlocked < RIVAL_QUOTA){ //found an enemy
					current->m_enemiesBlocked++;
					break; //enemy blocked
				}
			}
			if(!current){ //means no enemy blocked
				friend->m_friendsHelped++;
				return friend;
			}
		}
		if(!current->m_next){//it's the last one and not a friend
			return current;
		}
		current=current->m_next;
	}
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
 * @param size: #of items in list
 * @return: head to a new israeli item list. NULL for error.
 * */
static IsraeliItem duplicateItemList(IsraeliItem source, int size){
	if(!source){
		return NULL;
	}
	IsraeliItem *list = listQueue(source, size);
	if(!list){
		return NULL;
	}
	IsraeliItem previous = NULL, current = NULL;
	for(int i = size-1 ; i>=0 ; i--){
		current = malloc(sizeof(*current)); 	//size of the struct
		if(!current){
			destroyIsraeliList(previous);
			return NULL;// malloc failed
		}
		current->m_next=previous;
		previous=current;
		copyIsraeliItem(list[i], current);
	}
	free(list);
	return current;
}

static IsraeliQueueError addFriendshipFunctionsFromList(IsraeliQueue *queueList, IsraeliQueue queue, const unsigned int length){
	for(int i=0;i<length;i++){
		FriendshipFunction *currList= queueList[i]->m_friendshipFunctionList;
		while(*currList){
			if(IsraeliQueueAddFriendshipMeasure(queue, *currList)!=ISRAELIQUEUE_SUCCESS){
				return ISRAELI_QUEUE_ERROR;
			}
			currList++;
		}
	}
	return ISRAELIQUEUE_SUCCESS;
}
/**@param head: Israeli item list head
 * @param size: # of items in q
 * returns a list of ptrs to the israeli items, returns allocated memory
*/
static IsraeliItem* listQueue(IsraeliItem head, int size){
	if(!head || size<=0){
		return NULL;
	}
	IsraeliItem *list = malloc(size*sizeof(IsraeliItem)); //note: sizeof the ptr itself.
	if(!list){
		return NULL;
	}
	for(int i=0;i<size;i++){
		assert(head);
		list[i]=head;
		head=head->m_next;
	}
	return list;
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
	if(!queue){
		return;
	}
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
    if(!queue){
        return NULL;
    }
	IsraeliItem head = queue->m_head;
    if(!head){
        return  NULL;
    }
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
	int size = IsraeliQueueSize(q);
	IsraeliItem newHead = duplicateItemList(q->m_head, size);
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
	if(size<1){
		return ISRAELIQUEUE_SUCCESS;
	}
	IsraeliItem *list = listQueue(queue->m_head, size);
	if(!list){
		return ISRAELIQUEUE_ALLOC_FAILED;
	}
	for(int i=size-1;i>0;i--){	//not including the first one
		list[i-1]->m_next = list[i]->m_next; 	
		IsraeliItem friend = findFriend(queue, list[i]->m_data);
		insertBehind(list[i], friend);
	}
	queue->m_head = list[0]->m_next;	//after trying to improve the 1st item in line, the one after it is the new head
	IsraeliItem friend = findFriend(queue, list[0]->m_data);
	insertBehind(list[0], friend);
	free(list);
	return ISRAELIQUEUE_SUCCESS;
}
//=================================================================
/*merge function:*/
//=================================================================
IsraeliQueue IsraeliQueueMerge(IsraeliQueue* queueList , ComparisonFunction compareFunction){
	if(!queueList || !compareFunction || !queueList[0]){
		return NULL;	//bad parameters
	}
	unsigned int queuesNum=0, maxSize=0;
	double rivalryMultiply=1, friendSum=0;

	for(  ;queueList[queuesNum];queuesNum++) { 
		rivalryMultiply *= queueList[queuesNum]->m_rivalryThreshold;
		friendSum += queueList[queuesNum]->m_friendshipThreshold;
		int currSize = IsraeliQueueSize(queueList[queuesNum]);
		if(currSize>maxSize){
			maxSize=currSize;
		}
	}
	assert(queuesNum);	//already checked but i wanna be sure
	int rivalryThreshold = ceil(pow(ABS(rivalryMultiply), (double)1/queuesNum));
	int friendshipThreshold = ceil(friendSum/queuesNum);

	FriendshipFunction arr[1] = {NULL};	//we need to give em something
	
	IsraeliQueue newQ = IsraeliQueueCreate(arr, compareFunction,
										   friendshipThreshold, rivalryThreshold);
	if(addFriendshipFunctionsFromList(queueList, newQ, queuesNum) != ISRAELIQUEUE_SUCCESS){
		IsraeliQueueDestroy(newQ);
		return NULL;
	}
	for(int i=0;i<maxSize;i++){
		for(int j=0;j<queuesNum;j++){
			if(queueList[j]->m_head==NULL){
				continue;
			}
			IsraeliQueueError flag=IsraeliQueueEnqueue(newQ, IsraeliQueueDequeue(queueList[j]));
			if(flag!=ISRAELIQUEUE_SUCCESS){
				IsraeliQueueDestroy(newQ);
				return NULL;	//error in enqueue
			}
		}
	}
	return newQ;
}