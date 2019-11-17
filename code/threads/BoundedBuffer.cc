#include  "BoundedBuffer.h"
#include <malloc.h>

BoundedBuffer::BoundedBuffer(int maxsize)
{
    Maxsize = maxsize;
    lock = new Lock("Buffer lock");
    NotFull = new Condition("Buffer not full");
    NotEmpty = new Condition("Buffer not empty");
    head = 0,tail=0,count=0;
    buffer = (void *)malloc(maxsize);
}

BoundedBuffer::~BoundedBuffer()
{
    delete lock;
    delete NotFull;
    delete NotEmpty;
    free(buffer);
}

void BoundedBuffer::Write(void* data, int size)
{
    lock->Acquire();	
    while (Maxsize-count<size)
    {
        NotFull->Wait(lock);
    }
    for(int i=0;i<size;i++)
    {
        *((char*)buffer+(head+i)%Maxsize)=*((char*)data+i);
    }
    head = (head + size) % Maxsize;
    count += size;
    NotEmpty->Signal(lock);
    lock->Release();
}

void BoundedBuffer::Read(void* data, int size)
{
    lock->Acquire();
    while (count-size<0)
    {
        NotEmpty->Wait(lock);
    }
    for(int i=0;i<size;i++)
    {
        *((char*)data+i)=*((char*)buffer+(tail+i)%Maxsize);
    }
    tail = (tail + size) % Maxsize;
    count -= size;
    NotFull->Signal(lock);
    lock->Release();
}
