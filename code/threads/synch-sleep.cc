// synch-sleep.cc
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch-sleep.h"
#include "system.h"
#include <iostream>

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    while (value == 0) { 			// semaphore not available
        queue->Append((void *)currentThread);	// so go to sleep
        currentThread->Sleep();
    }
    value--; 					// semaphore available,
    // consume its value

    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!
// 锁就是二元信号量 相当于将一般信号量简化
Lock::Lock(char* debugName) {
    name = debugName;
    lock = FALSE;
    holder = NULL;
    queue = new List;
}
Lock::~Lock() {
    delete queue;
}
void Lock::Acquire() {
    // 如果当前进程已经被锁进来了 出错
    std::wcout << "Lock Acquire" << std::endl;
    ASSERT(!this->isHeldByCurrentThread()) 
    // 关闭中断 保证原子操作
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // 当锁去锁到其他进程时, 轮不到你索尼🐎的时候
    while (lock)      
    {
        queue->Append((void *)currentThread);
        currentThread->Sleep();
    }
    // 二元信号量： 我锁了
    lock = TRUE;       
    holder = currentThread;   // 锁住当前线程
    (void) interrupt->SetLevel(oldLevel); 
}
void Lock::Release() {
    Thread *thread;
    // 当前进程没有使用这个锁 保证一个锁一个进程
    std::wcout << "Lock Release" << std::endl;
    ASSERT(this->isHeldByCurrentThread())
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    
    thread = (Thread *)queue->Remove();
    // 取出这个
    if(thread != NULL)
        scheduler->ReadyToRun(thread);
    lock = FALSE;
    holder = NULL; 
    (void) interrupt->SetLevel(oldLevel);
}
bool Lock::isHeldByCurrentThread(){
    return (holder == currentThread);
}

Condition::Condition(char* debugName) { 
    name = debugName;
    queue = new List;
}
Condition::~Condition() {
    delete queue;
 }

void Condition::Wait(Lock* conditionLock) {
    // 当前条件的锁 对应当前的线程
    ASSERT(conditionLock->isHeldByCurrentThread());
    // 原子操作 禁用中断 也就是 释放锁之后没有其他影响
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    // 先释放当前进程的 条件锁
    conditionLock->Release();
    queue->Append((void *)currentThread);
    // 当前进程睡眠
    currentThread->Sleep();

    (void)interrupt->SetLevel(oldLevel);
    // 锁住当前 入队 但是不休眠进程
    conditionLock->Acquire();
}
 // 如果满足条件变量 就释放锁 并且将线程从队列取出 变为就绪态
void Condition::Signal(Lock* conditionLock) {
    Thread *thread;

    ASSERT(conditionLock->isHeldByCurrentThread());
    thread = (Thread *)queue->Remove();
    if(thread != NULL)
        scheduler->ReadyToRun(thread);
 }

void Condition::Broadcast(Lock* conditionLock) { 
    Thread *thread;

    ASSERT(conditionLock->isHeldByCurrentThread());
    do{
        thread = (Thread *)queue->Remove();
        if(thread != NULL){
            scheduler->ReadyToRun(thread);
        }
    }while(!queue->IsEmpty());
}
