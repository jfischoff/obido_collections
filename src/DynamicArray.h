#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#ifndef NULL
#define NULL 0
#endif

class DynamicArray
{
 public:
  DynamicArray();
  DynamicArray(unsigned int elementSize, unsigned int startCount);
  DynamicArray(const DynamicArray& other);
  DynamicArray& operator=(const DynamicArray& other);
  void copy(const DynamicArray& other);

  void deepCopy(const DynamicArray* other);
  void destroy();

  void clearData();
  void deallocate();

  void reinit(unsigned int elementSize, unsigned int startCount);
  
  void set(unsigned int index, const void* element);
  void* get(unsigned int index);
  const void* get(unsigned int index) const;
  
  void safeSet(unsigned int index, const void* element, 
	       const void* defaultElement = NULL);
  
  
  void append(const void* element);
  void appendDefault(const void* defaultElement = NULL);
  
  unsigned int getCount() const;
  void setCount(unsigned int count); 

  unsigned int getElementSize() const;
  
  char* getBuffer();
  const char* getBuffer() const;
  char* getBufferAt(unsigned int index);
  const char* getBufferAt(unsigned int index) const;
  
  void setSome(unsigned int start, unsigned int length, 
	       const void* otherBuffer);

  void setMultiple(void* data, unsigned int start, unsigned int count); 

  void* top();
  const void* top() const;
  
  void merge(const DynamicArray& other);
  void remove(unsigned int index);
  void removeMany(unsigned int* indices, unsigned int count);
  
 private:
 
  void allocate(unsigned int newCount);
  unsigned int byteIndex(unsigned int index) const;
  void maybeGrow(unsigned int newCount);

  unsigned int m_ElementSize;
  unsigned int m_MaxCount;
  unsigned int m_Count;
  void* m_Buffer;  
};

#endif //DYNAMICARRAY_H
