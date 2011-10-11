#include "DynamicArray.h"
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <assert.h>
using namespace std;

#define max(a,b)	(((a) > (b)) ? (a) : (b))

DynamicArray::DynamicArray() : m_ElementSize(0), m_MaxCount(0), m_Count(0), m_Buffer(NULL)
{
}

DynamicArray::DynamicArray(uint elementSize, uint startCount) :
   m_Count(0), m_Buffer(NULL)
{
  reinit(elementSize, startCount);
}

DynamicArray::DynamicArray(const DynamicArray& other)
{
  copy(other);
}

DynamicArray& DynamicArray::operator=(const DynamicArray& other)
{
  copy(other);
  return *this;
}

void DynamicArray::copy(const DynamicArray& other)
{
  m_ElementSize = other.m_ElementSize;
  m_MaxCount = other.m_MaxCount;
  m_Count = other.m_Count;
  m_Buffer = other.m_Buffer;
}

void DynamicArray::deepCopy(const DynamicArray* otherDynamicArray)
{
  m_ElementSize = otherDynamicArray->m_ElementSize;
  m_Count = otherDynamicArray->m_Count;

  maybeGrow(otherDynamicArray->m_MaxCount);
  const void* otherBuffer = otherDynamicArray->m_Buffer;
  const string* debugBuffer = (const string*)otherBuffer; 
  memcpy(m_Buffer, otherBuffer, m_ElementSize*m_MaxCount);
}

void DynamicArray::clearData()
{
  m_Count = 0;
}

void DynamicArray::deallocate()
{
  if(m_Buffer != NULL)
    {
      free(m_Buffer);
      m_Buffer = NULL;
      m_MaxCount = 0;
    }
}

void DynamicArray::reinit(unsigned int elementSize, unsigned int startCount)
{
  m_ElementSize = elementSize;
  clearData();
  maybeGrow(startCount);
}
  
void DynamicArray::set(unsigned int index, const void* element)
{
  assert(m_Buffer != NULL);
  assert(m_Count > index);

  unsigned int bIndex = byteIndex(index);  

  void* bufferStart = getBufferAt(index);
  assert(m_ElementSize != 0);

  memcpy(bufferStart, element, m_ElementSize);
}

void* DynamicArray::get(unsigned int index)
{
  assert(m_Buffer != NULL);
  assert(m_Count > index);


  return getBufferAt(index);
}

const void* DynamicArray::get(unsigned int index) const
{
  assert(m_Buffer != NULL);
  assert(m_Count > index);


  return getBufferAt(index);
}
  
void DynamicArray::safeSet(unsigned int index, const void* element, 
			   const void* defaultElement)
{
  bool deallocate = false;
  void* myDefaultElement = NULL;
  if(defaultElement == NULL)
    {
      myDefaultElement = malloc(m_ElementSize);
      memset(myDefaultElement, 0, m_ElementSize);
      deallocate = true;
    }
  else
    {
      myDefaultElement = (void*)defaultElement;
    }

  while(m_Count <= index)
    {
      append(myDefaultElement);
    }

  if(deallocate)
    {
      free(myDefaultElement);
    }

  set(index, element);

}

void DynamicArray::append(const void* element)
{
  m_Count++;
  maybeGrow(m_Count);
  set(m_Count - 1, element);
}

void DynamicArray::appendDefault(const void* defaultElement)
{
  bool deallocate = false;
  void* myDefaultElement = NULL;
  if(defaultElement == NULL)
    {
      myDefaultElement = malloc(m_ElementSize);
      memset(myDefaultElement, 0, m_ElementSize);
      deallocate = true;
    }
  else
    {
      myDefaultElement = (void*)defaultElement;
    }
  
  append(myDefaultElement);
 
  if(deallocate)
    {
      free(myDefaultElement);
    }
}
  
unsigned int DynamicArray::getCount() const
{
  return m_Count;
}

void DynamicArray::setCount(unsigned int count) 
{
  m_Count = count;
  maybeGrow(m_Count);
}

unsigned int DynamicArray::getElementSize() const 
{
  return m_ElementSize;
}

char* DynamicArray::getBuffer()
{
  return getBufferAt(0);
}

const char* DynamicArray::getBuffer() const 
{
  return getBufferAt(0);
}

char* DynamicArray::getBufferAt(unsigned int index)
{
  unsigned int bIndex = byteIndex(index);
  return &((char*)m_Buffer)[bIndex];
}

const char* DynamicArray::getBufferAt(unsigned int index) const
{
  unsigned int bIndex = byteIndex(index);
  return &((const char*)m_Buffer)[bIndex];
}

void DynamicArray::setSome(unsigned int start, unsigned int length, const void* otherBuffer)
{
  unsigned int bStartIndex = byteIndex(start);

  unsigned int potentialNewCount = start + length;

  m_Count = max(m_Count, potentialNewCount);

  maybeGrow(m_Count);
  memcpy(getBufferAt(start), otherBuffer, m_ElementSize * length);
}

void DynamicArray::setMultiple(void* data, unsigned int start, unsigned int count)
{
  for(unsigned int index = 0; index < count; index++)
    {
      unsigned int actualIndex = start + index;
      set(actualIndex, data);
    }
}


void DynamicArray::allocate(unsigned int newCount)
{
  unsigned int amount = m_ElementSize * newCount;
  assert(amount < 50000000);
  if(m_Buffer == NULL)
    {
      m_Buffer = malloc(amount);
    }
  else
    {
      m_Buffer = realloc(m_Buffer, amount);
    }
}

unsigned int DynamicArray::byteIndex(unsigned int index) const
{
  return index * m_ElementSize;
}

void DynamicArray::maybeGrow(unsigned int newCount)
{
  if(newCount > m_MaxCount)
    {
      m_MaxCount = max(newCount, 2 * m_MaxCount);       
      
      allocate(m_MaxCount);
    }
}

void DynamicArray::destroy()
{
    deallocate();
    clearData();
}

void* DynamicArray::top()
{
  return get(m_Count - 1);
}

const void* DynamicArray::top() const
{
  return get(m_Count - 1);
}


void DynamicArray::merge(const DynamicArray& other)
{
  assert(m_ElementSize == other.m_ElementSize);
  
  for(unsigned int index = 0; index < other.getCount(); index++)
    {
      const void* element = other.get(index);
      append(element);
    }
}

void DynamicArray::remove(unsigned int index)
{
  assert(index < m_Count);

  //move the stuff over with memcpy and decrement the m_Count
  if(m_Count > 1)
    {
      char* startPointer = getBufferAt(index);
      char* startOfMove = getBufferAt(index + 1);
      unsigned int amount = m_Count - index;
      unsigned int elementSize = getElementSize();
      unsigned int bufferAmount = elementSize * amount;
      memmove(startPointer, startOfMove, bufferAmount);
    }
  m_Count--;
}

static int intCompare (const void * a_, const void * b_)
{
  unsigned int a = *(unsigned int*)a_;
  unsigned int b = *(unsigned int*)b_;

  if(a < b)
    {
      return -1;
    }
  else if(a > b)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

void DynamicArray::removeMany(unsigned int* indices, unsigned int count)
{
  //sort the remove indices
  qsort(indices, count, sizeof(unsigned int), intCompare);

  for(unsigned int index = 0; index < count; index++)
    {
      //remove something      
      unsigned int indexToRemove = indices[index];
      remove(indexToRemove);
      
      for(unsigned int j = index + 1; j < count; j++)
	{
	  //decrement all of the indices 
	  indices[j]--;
	}      
    }
}
