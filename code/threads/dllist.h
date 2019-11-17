#pragma once
#include <synch-sleep.h>
class DLLElement
{
public:
	DLLElement(int sortKey);
	// initialize a list element
	DLLElement *next;
	// next element on list
	// NULL if this is the last
	DLLElement *prev;
	// previous element on list
	// NULL if this is the first
	int key;
	// priority, for a sorted list
	//int *item;
	// pointer to item on the list
	// Lock *lock;
	// Condition *condition;
	// 放错了 放在了链表元素类里
};
class DLList
{
public:
	DLList();
	// initialize the list
	~DLList();
	// de-allocate the list
	void Prepend();
	// add to head of list (set key = min_key-1)
	void Append();
	// add to tail of list (set key = max_key+1)
	void *Remove();
	// remove from head of list
	// set *keyPtr to key of the removed item
	// return item (or NULL if list is empty)
	bool IsEmpty();
	// return true if list has elements
	void SortedInsert(int sortKey);
	// routines to put/get items on/off list in order (sorted by key)
	void *SortedRemove(int sortKey);
	// remove first item with key==sortKey
	// return NULL if no such item exists
	void RemoveN(int N);
	// remove N items in end
	void AppendN(int N);
	// append N items in end
	void travesal();
	// show every element in list
private:
	DLLElement *first;
	// head of the list, NULL if empty
	DLLElement *last;
	// last element of the list, NULL if empty
	Lock *mylock = new Lock("dllist Lock");
	Condition *mycondition = new Condition("list empty con");
};
