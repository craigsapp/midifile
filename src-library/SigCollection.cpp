//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Feb  5 19:42:53 PST 1997
// Last Modified: Wed Apr 23 22:08:34 GMT-0800 1997
// Last Modified: Fri Sep 14 15:50:52 PDT 2001 added last() function
// Last Modified: Wed Mar 30 14:00:16 PST 2005 Fixed for compiling in GCC 3.4
// Last Modified: Fri Jun 12 22:58:34 PDT 2009 renamed SigCollection class
// Last Modified: Fri Aug 10 09:17:03 PDT 2012 added reverse()
// Filename:      ...sig/maint/code/base/SigCollection/SigCollection.cpp
// Web Address:   http://sig.sapp.org/src/sigBase/SigCollection.cpp
// Syntax:        C++ 
//
// Description:   A dynamic array which can grow as necessary.
//                This class can hold any type of item, but the
//                derived Array class is specifically for collections
//                of numbers.
//

#ifndef _SIGCOLLECTION_CPP_INCLUDED
#define _SIGCOLLECTION_CPP_INCLUDED

#include "SigCollection.h"
#include <iostream>
#include <stdlib.h>


using namespace std;

//////////////////////////////
//
// SigCollection::SigCollection --
//

template<class type>
SigCollection<type>::SigCollection(void) {
   this->allocSize = 0;
   this->size = 0;
   this->array = NULL;
   this->allowGrowthQ = 0;
   this->growthAmount = 8;
   this->maxSize = 0;
}

template<class type>
SigCollection<type>::SigCollection(int arraySize) {
   this->array = new type[arraySize];
   
   this->size = arraySize;
   this->allocSize = arraySize;
   this->allowGrowthQ = 0;
   this->growthAmount = arraySize;
   this->maxSize = 0;
}


template<class type>
SigCollection<type>::SigCollection(int arraySize, type *aSigCollection) {
   this->size = arraySize;
   this->allocSize = arraySize;
   this->array = new type[size];
   for (int i=0; i<size; i++) {
      this->array[i] = aSigCollection[i];
   }
   this->growthAmount = arraySize;
   this->allowGrowthQ = 0;
   this->maxSize = 0;
}


template<class type>
SigCollection<type>::SigCollection(SigCollection<type>& aSigCollection) {
   this->size = aSigCollection.size;
   this->allocSize = size;
   this->array = new type[size];
   for (int i=0; i<size; i++) {
      this->array[i] = aSigCollection.array[i];
   }
   this->allowGrowthQ = aSigCollection.allowGrowthQ;
   this->growthAmount = aSigCollection.growthAmount;
   this->maxSize = aSigCollection.maxSize;
}



//////////////////////////////
//
// SigCollection::~SigCollection --
//

template<class type>
SigCollection<type>::~SigCollection() {
   if (this->getAllocSize() != 0) {
      delete [] this->array;
   }
}



//////////////////////////////
//
// SigCollection::allowGrowth --
//	default value: status = 1 
//

template<class type>
void SigCollection<type>::allowGrowth(int status) {
   if (status == 0) {
      this->allowGrowthQ = 0;
   } else {
      this->allowGrowthQ = 1;
   }
}



//////////////////////////////
//
// SigCollection::append --
//

template<class type>
void SigCollection<type>::append(type& element) {
   if (this->size == this->getAllocSize()) {
      this->grow();
   }
   this->array[size] = element;
   this->size++;
}

template<class type>
void SigCollection<type>::appendcopy(type element) {
   if (this->size == this->getAllocSize()) {
      this->grow();
   }
   this->array[size] = element;
   this->size++;
}

template<class type>
void SigCollection<type>::append(type *element) {
   if (this->size == this->getAllocSize()) {
      this->grow();
   }
   this->array[size] = *element;
   this->size++;
}



//////////////////////////////
//
// SigCollection::grow --
// 	default parameter: growamt = -1
//

template<class type>
void SigCollection<type>::grow(long growamt) {
   this->allocSize += growamt > 0 ? growamt : this->growthAmount;
   if (this->maxSize != 0 && this->getAllocSize() > this->maxSize) {
      std::cerr << "Error: Maximum size allowed for array exceeded." << std::endl;
      exit(1);
   }
 
   type *temp = new type[this->getAllocSize()];
   for (int i=0; i<size; i++) {
      temp[i] = this->array[i];
   }
   delete [] this->array;
   this->array = temp;
}



//////////////////////////////
//
// SigCollection::pointer --
//

template<class type>
type* SigCollection<type>::pointer(void) {
   return this->array;
}



//////////////////////////////
//
// SigCollection::getBase --
//

template<class type>
type* SigCollection<type>::getBase(void) const {
   return this->array;
}



//////////////////////////////
//
// SigCollection::getAllocSize --
//

template<class type>
long SigCollection<type>::getAllocSize(void) const {
   return this->allocSize;
}



//////////////////////////////
//
// SigCollection::getSize --
//

template<class type>
long SigCollection<type>::getSize(void) const {
   return this->size;
}



//////////////////////////////
//
// SigCollection::last --
//      default value: index = 0
//

template<class type>
type& SigCollection<type>::last(int index) {
   return this->array[getSize()-1-abs(index)];
}



//////////////////////////////
//
// SigCollection::setAllocSize --
//

template<class type>
void SigCollection<type>::setAllocSize(long aSize) {
   if (aSize < this->getSize()) {
      std::cerr << "Error: cannot set allocated size smaller than actual size." 
           << std::endl;
      exit(1);
   }

   if (aSize <= this->getAllocSize()) {
      this->shrinkTo(aSize);
   } else {
      this->grow(aSize-this->getAllocSize());
      this->size = aSize;
   }
}



//////////////////////////////
//
// SigCollection::setGrowth --
//	default parameter: growth = -1
//

template<class type>
void SigCollection<type>::setGrowth(long growth) {
   if (growth > 0) {
      this->growthAmount = growth;
   }
}



//////////////////////////////
//
// SigCollection::setSize --
//

template<class type>
void SigCollection<type>::setSize(long newSize) {
   if (newSize <= this->getAllocSize()) { 
      this->size = newSize;
   } else {
      this->grow(newSize-this->getAllocSize());
      this->size = newSize;
   }
}



////////////////////////////////////////////////////////////////////////////////
//
// SigCollection operators
//

//////////////////////////////
//
// SigCollection::operator[] --
//

template<class type>
type& SigCollection<type>::operator[](int elementIndex) {
   if (this->allowGrowthQ && elementIndex == this->size) {
      if (this->size == this->getAllocSize()) {
         this->grow();
      }
      this->size++;
   } else if ((elementIndex >= this->size) || (elementIndex < 0)) {
      std::cerr << "Error: accessing invalid array location " 
           << elementIndex 
           << " Maximum is " << this->size-1 << std::endl;
      exit(1);
   }
   return this->array[elementIndex];
}


//////////////////////////////
//
// SigCollection::operator[] const --
//

template<class type>
type SigCollection<type>::operator[](int elementIndex) const {
   if ((elementIndex >= this->size) || (elementIndex < 0)) {
      std::cerr << "Error: accessing invalid array location " 
           << elementIndex 
           << " Maximum is " << this->size-1 << std::endl;
      exit(1);
   }
   return this->array[elementIndex];
}



//////////////////////////////
//
// SigCollection::shrinkTo --
//

template<class type>
void SigCollection<type>::shrinkTo(long aSize) {
   if (aSize < this->getSize()) {
      exit(1);
   }

   type *temp = new type[aSize];
   for (int i=0; i<this->size; i++) {
      temp[i] = this->array[i];
   }
   delete [] this->array;
   this->array = temp;

   allocSize = aSize;
   if (size > allocSize) {
      size = allocSize;
   }
}



//////////////////////////////
//
// SigCollection::increase -- equivalent to setSize(getSize()+addcount)
//

template<class type>
int SigCollection<type>::increase(int addcount) {
   if (addcount > 0) {
      this->setSize(this->getSize() + addcount);
   }
   return this->getSize();
}



//////////////////////////////
//
// SigCollection::decrease -- equivalent to setSize(getSize()-subcount)
//

template<class type>
int SigCollection<type>::decrease(int subcount) {
   if (this->getSize() - subcount <= 0) {
      this->setSize(0);
   } else if (subcount > 0) {
      this->setSize(this->getSize() - subcount);
   }
   return this->getSize();
}


//////////////////////////////
//
// SigCollection::reverse -- reverse the order of items in the list.
//

template<class type>
void SigCollection<type>::reverse(void) {
   int i;
   type tempval;
   int mirror;
   int pivot = this->getSize() / 2;
   for (i=0; i<pivot; i++) {
      tempval = this->array[i];
      mirror = this->getSize() - i - 1;
      this->array[i] = this->array[mirror];
      this->array[mirror] = tempval;
   }
}


#endif  /* _SIGCOLLECTION_CPP_INCLUDED */



// md5sum: e5d20829760eaa880e5753116883784c SigCollection.cpp [20050403]
