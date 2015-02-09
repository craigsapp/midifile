//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Feb  5 19:42:53 PST 1997
// Last Modified: Sun May 11 20:41:28 GMT-0800 1997
// Last Modified: Wed Jul  7 11:44:50 PDT 1999 Added setAll() function
// Last Modified: Wed Mar 30 13:58:18 PST 2005 Fixed for compiling in GCC 3.4
// Last Modified: Fri Jun 12 22:58:34 PDT 2009 Renamed SigCollection class
// Last Modified: Wed Sep  8 17:26:13 PDT 2010 Added operator<< for chars
// Filename:      ...sig/maint/code/base/Array/Array.cpp
// Web Address:   http://sig.sapp.org/src/sigBase/Array.cpp
// Syntax:        C++ 
//
// Description:   An array which can grow dynamically.  Array is derived from 
//                the SigCollection class and adds various mathematical 
//                operators to the SigCollection class.  The Array template 
//                class is used for storing numbers of any type which can be 
//                added, multiplied and divided into one another.
//

#ifndef _ARRAY_CPP_INCLUDED
#define _ARRAY_CPP_INCLUDED

#include "Array.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

//////////////////////////////
//
// Array::Array 
//

template<class type>
Array<type>::Array(void) : SigCollection<type>(4) { 
}

template<class type>
Array<type>::Array(int arraySize) : SigCollection<type>(arraySize) { 
}

template<class type>
Array<type>::Array(Array<type>& anArray) : SigCollection<type>(anArray) { 
}

template<class type>
Array<type>::Array(int arraySize, type *anArray) : 
   SigCollection<type>(arraySize, anArray) { 
}




//////////////////////////////
//
// Array::~Array
//

template<class type>
Array<type>::~Array() { 
} 



//////////////////////////////
//
// Array::setAll -- sets the contents of each element to the 
//   specified value
//

template<class type>
void Array<type>::setAll(type aValue) {
   for (int i=0; i<this->getSize(); i++) {
      this->array[i] = aValue;
   }
}

//
// Incremental version of setAll(): the first number is the starting
// value in the array, and the second value is a successive incrment
// value added to that for each element in the array until the end.
//

template<class type>
void Array<type>::setAll(type aValue, type increment) {
   if (this->getSize() > 0) {
      this->array[0] = aValue;
   }
   for (int i=1; i<this->getSize(); i++) {
      this->array[i] = this->array[i-1] + increment;
   }
}



//////////////////////////////
//
// Array::sum
//

template<class type>
type Array<type>::sum(void) {
   type theSum = 0;
   for (int i=0; i<this->getSize(); i++) {
      theSum += this->array[i];
   }
   return theSum;
}

template<class type>
type Array<type>::sum(int loIndex, int hiIndex) {
   type theSum = 0;
   for (int i=loIndex; i<=hiIndex; i++) {
      theSum += this->array[i];
   }
   return theSum;
}



//////////////////////////////
//
// Array::zero -- (-1, -1)
//

template<class type>
void Array<type>::zero(int minIndex, int maxIndex) {
   if (this->size == 0) return;
   if (minIndex == -1) minIndex = 0;
   if (maxIndex == -1) maxIndex = this->size-1;

   if (minIndex < 0 || maxIndex < 0 || minIndex > maxIndex ||
       maxIndex >= this->size) {
      cerr << "Error in zero function: min = " << minIndex
           << " max = " << maxIndex << " size = " << this->size << endl;
      exit(1);
   }

   for (int i=minIndex; i<=maxIndex; i++) {
      this->array[i] = 0;
   }
}


////////////////////////////////////////////////////////////////////////////
//
// operators
//


//////////////////////////////
//
// Array::operator== --
//

template<class type>
int Array<type>::operator==(const Array<type>& aArray) {
   if (this->getSize() != aArray.getSize()) {
      return 0;
   }
   Array<type>& t = *this;
   int i;
   for (i=0; i<this->getSize(); i++) {
      if (t[i] != aArray[i]) {
         return 0;
      }
   }
   return 1;
}


// this template mostly presumes that the type is char:
template<class type>
int Array<type>::operator==(const char* aString) {
   if (this->getSize() == 0) {
      return 0;
   }
   if (aString == NULL) {
      return 0;
   }
   Array<type>& t = *this;
   return !strcmp(t.getBase(), aString);
}



//////////////////////////////
//
// Array::operator= --
//

template<class type>
Array<type>& Array<type>::operator=(const Array<type>& anArray) {
   if (this == &anArray) {
      return *this;
   }
   if (this->allocSize < anArray.size) {
      if (this->array != NULL) {
         delete [] this->array;
	 this->array = NULL;
      }
      this->allocSize = anArray.size;
      this->size = anArray.size;
      this->array = new type[this->size];
      this->allowGrowthQ = anArray.allowGrowthQ;
      this->growthAmount = anArray.growthAmount;
      this->maxSize = anArray.maxSize;
   }
   this->size = anArray.size;
   for (int i=0; i<this->size; i++) {
      this->array[i] = anArray.array[i];
   }

   return *this;
}



//////////////////////////////
//
// Array::operator+=
//

template<class type>
Array<type>& Array<type>::operator+=(const Array<type>& anArray) {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   for (int i=0; i<this->size; i++) {
      this->array[i] += anArray.array[i];
   }

   return *this;
}



//////////////////////////////
//
// Array::operator+
//

template<class type>
Array<type> Array<type>::operator+(const Array<type>& anArray) const {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   Array<type> bArray(*this);
   bArray += anArray;
   return bArray;
}


template<class type>
Array<type> Array<type>::operator+(type aNumber) const {
   Array<type> anArray(*this);
   for (int i=0; i<this->size; i++) {
      anArray[i] += aNumber;
   }
   return anArray;
}



//////////////////////////////
//
// Array::operator-=
//

template<class type>
Array<type>& Array<type>::operator-=(const Array<type>& anArray) {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   for (int i=0; i<this->size; i++) {
      this->array[i] -= anArray.array[i];
   }

   return *this;
}



//////////////////////////////
//
// Array::operator-
//

template<class type>
Array<type> Array<type>::operator-(const Array<type>& anArray) const {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   Array<type> bArray(*this);
   bArray -= anArray;
   return bArray;
}


template<class type>
Array<type> Array<type>::operator-(void) const {
   Array<type> anArray(*this);
   for (int i=0; i<this->size; i++) {
      anArray[i] = -anArray[i];
   }
   return anArray;
}

template<class type>
Array<type> Array<type>::operator-(type aNumber) const {
   Array<type> anArray(*this);
   for (int i=0; i<this->size; i++) {
      anArray[i] -= aNumber;
   }
   return anArray;
}



//////////////////////////////
//
// Array::operator*=
//

template<class type>
Array<type>& Array<type>::operator*=(const Array<type>& anArray) {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   for (int i=0; i<this->size; i++) {
      this->array[i] *= anArray.array[i];
   }

   return *this;
}



//////////////////////////////
//
// Array::operator*
//

template<class type>
Array<type> Array<type>::operator*(const Array<type>& anArray) const {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   Array<type> bArray(*this);
   bArray *= anArray;
   return bArray;
}


template<class type>
Array<type> Array<type>::operator*(type aNumber) const {
   Array<type> anArray(*this);
   for (int i=0; i<this->size; i++) {
      anArray[i] *= aNumber;
   }
   return anArray;
}



//////////////////////////////
//
// Array::operator/=
//

template<class type>
Array<type>& Array<type>::operator/=(const Array<type>& anArray) {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   for (int i=0; i<this->size; i++) {
      this->array[i] /= anArray.array[i];
   }

   return *this;
}



//////////////////////////////
//
// Array::operator/
//

template<class type>
Array<type> Array<type>::operator/(const Array<type>& anArray) const {
   if (this->size != anArray.size) {
      cerr << "Error: different size arrays " << this->size << " and " 
           << anArray.size << endl;
      exit(1);
   }

   Array<type> bArray(*this);
   bArray /= anArray;
   return bArray;
}



#endif  /* _ARRAY_CPP_INCLUDED */



// md5sum: f9167a143ce5d33d90e9c9a04025ac26 Array.cpp [20121211]
