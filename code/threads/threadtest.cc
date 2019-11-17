// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "dllist.h"
#include <iostream>
#include <synch-sleep.h>
#include <scheduler.h>
using namespace std;
// testnum is set in main.cc
int testnum = 2;
DLList *dl = new DLList();
Scheduler sc;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++)
    {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}
//-------------------------------------
// SimpleThread2
// insert
//-------------------------------------
void SimpleThread2(int which)
{
    for (size_t i = 0; i < 4; i++)
    {
        wcout << "Thread : " << which << endl;
        dl->Append();
        Thread *nextThread = sc.FindNextToRun();
        if(nextThread != NULL)
            // currentThread->Yield();
            SWITCH(currentThread, nextThread);
    }
    dl->travesal();
    wcout << "Test End" <<endl;
}
//----------------------------------------------------------------------
// ThreadTest2
// 展示并发错误
//----------------------------------------------------------------------
void ThreadTest2()
{
    DEBUG('t', "正在演示并发错误");
    char *threadName0 = new char[30];
    char *threadName1 = new char[30];
    sprintf(threadName0, "%s%d", "Test Thread ", 0);
    sprintf(threadName1, "%s%d", "Test Thread ", 1);
    Thread *t0 = new Thread(threadName0);
    Thread *t1 = new Thread(threadName1);
    t0->Fork(SimpleThread2, 0);
    t1->Fork(SimpleThread2, 1);
    sc.Run(t1);
    // SimpleThread2(0);
    // t1->Fork((VoidFunctionPtr)&dl.DLList::AppendN, 1);
    // dl.DLList::AppendN(0);
    // dl.DLList::Append();
    // dl.AppendN(6);
    // dl.travesal();  // 遍历
    // dl.SortedRemove(3);
    // dl.travesal();

    // dl.Prepend();
    // dl.SortedInsert(8);
    // dl.travesal();

    // dl.SortedInsert(5);
    // dl.RemoveN(1);
}
//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest()
{
    switch (testnum)
    {
    case 1:
        ThreadTest1();
        break;
    case 2:
        ThreadTest2();
        break;
    default:
        printf("No test specified.\n");
        break;
    }
}
