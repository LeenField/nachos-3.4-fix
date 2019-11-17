#include "dllist.h"
// stdlib 头文件冲突
// #include "stdio.h"
// #include <stdlib.h>
#include <malloc.h>
#include <iostream>
// #include <thread.h>
#include <system.h>
#include <synch-sleep.h>
using namespace std;

// std::wcout.imbue(std::locale("chs"));
DLList::DLList()
{
	first = last = NULL;
	// lock = new Lock("dllist Lock");
	// condition = new Condition("list empty con");
}

DLList::~DLList()
{
}

void DLList::Prepend()
{
	if (first == NULL)
	{
		DLLElement *temp_element = (DLLElement *)malloc(sizeof(DLLElement));
		temp_element->key = 0;
		temp_element->prev = temp_element->next = NULL;
		// 初始化
		first = last = temp_element;
		wcout << "Prepend in empty head key=0" << endl;
	}
	else
	{
		// 链表非空在首部添加节点
		DLLElement *temp_element = (DLLElement *)malloc(sizeof(DLLElement));
		temp_element->key = first->key - 1;
		temp_element->prev = temp_element->next = NULL;
		// 初始化
		temp_element->prev = temp_element->next = NULL;
		temp_element->next = first;
		first->prev = temp_element;
		first = temp_element;
		wcout << "Prepend in not empty head key=" << temp_element->key << endl;
	}
}

void DLList::Append()
{
	mylock->Acquire();
	if (last == NULL)
	{
		// DLLElement* temp_element = (DLLElement*)malloc(sizeof(DLLElement));
		// temp_element->key = 0;
		DLLElement *temp_element = new DLLElement(0);
		temp_element->prev = temp_element->next = NULL;
		// 初始化
		first = last = temp_element;
		wcout << "append in head key=0" << endl;
	}
	else
	{
		// 非空链表在末位添加
		DLLElement *temp_element = (DLLElement *)malloc(sizeof(DLLElement));
		temp_element->key = last->key + 1;
		temp_element->prev = temp_element->next = NULL;
		// currentThread->Yield();
		// 初始化
		last->next = temp_element;
		temp_element->prev = last;
		last = temp_element;
		wcout << "now thread name " << currentThread->getName() << " ";
		wcout << "append in end key =" << temp_element->key << endl;
	}
	mycondition->Signal(mylock);
	mylock->Release();
}

void *DLList::Remove()
{
	mylock->Acquire();	
	while (IsEmpty())		
	{
		mycondition->Wait(mylock);
	}
	// if (first != NULL)
	// {
		int temp_key = first->key;
		first = first->next;
		wcout << "Remove from head key = " << temp_key << endl;
	// }
	if(IsEmpty())
	{
		wcout << "Empty dllist !" << endl;
	}
	mylock->Release();
	return NULL;
}

bool DLList::IsEmpty()
{
	if (last == NULL and first == NULL)
		return true;
	return false;
}

void DLList::SortedInsert(int sortKey)
{
	DLLElement *temp = first, *element = (DLLElement *)malloc(sizeof(DLLElement));
	element->key = sortKey;
	while (temp != NULL)
	{
		// 遇到元素比该值大的节点
		if (temp->key >= sortKey)
		{
			if (temp->prev != NULL)
			{
				// 非首部插入
				temp->prev->next = element;
				element->prev = temp->prev;
				element->next = temp;
				temp->prev = element;
				wcout << "insert not head key = " << sortKey << endl;
				return;
			}
			else
			{
				// 链表首部插入
				element->next = first;
				first->prev = element;
				first = element;
				wcout << "insert head key = " << sortKey << endl;
				return;
			}
		}
		temp = temp->next;
	}
	// 如果空链表 或者key大于整个链表
	Append();
	last->key = sortKey;
}

void *DLList::SortedRemove(int sortKey)
{
	DLLElement *temp = first;
	while (temp != NULL)
	{
		if (temp->key == sortKey)
		{
			if (temp->prev == NULL)
			{
				first = temp->next;
				if (last == temp)
				{
					// 链表里面只有一个元素
					last = NULL;
				}
				wcout << "delect head key =" << sortKey << endl;
				return NULL;
			}
			else if (temp->next == NULL)
			{
				last = temp->prev;
				if (last != NULL)
				{
					last->next = NULL;
				}
				wcout << "delect end key = " << sortKey << endl;
				return NULL;
			}
			else
			{
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				wcout << "delect middle key = " << sortKey << endl;
				return NULL;
			}
		}
		temp = temp->next;
	}
	wcout << "no exist element key = " << sortKey << endl;
	return NULL;
}

DLLElement::DLLElement(int sortKey)
{
	key = sortKey;
	prev = NULL;
	next = NULL;
}

void DLList::travesal()
{
	DLLElement *temp = first;
	// wcout.imbue(locale("", LC_CTYPE));
	// 	wchar_t str[] = L"遍历链表：";
	wcout << "traversal:" << endl;
	while (temp != NULL)
	{
		wcout << temp->key << endl;
		temp = temp->next;
	}
}
