//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Feb  5 19:42:53 PST 1997
// Last Modified: Sun May 11 20:33:13 GMT-0800 1997
// Last Modified: Wed Jul  7 11:44:50 PDT 1999 added setAll() function
// Last Modified: Mon Jul 29 22:08:32 PDT 2002 added operator==
// Last Modified: Fri Jun 12 22:58:34 PDT 2009 renamed SigCollection class
// Last Modified: Wed Sep  8 17:26:13 PDT 2010 added operator<< for chars
// Last Modified: Wed Jan 11 15:53:55 PST 2012 added operator<< for ints
// Last Modified: Fri Aug 10 15:57:25 PDT 2012 added setAll(#,#) function
// Filename:      ...sig/maint/code/base/Array/Array.h
// Web Address:   http://sig.sapp.org/include/sigBase/Array.h
// Documentation: http://sig.sapp.org/doc/classes/Array
// Syntax:        C++ 
//
// Description:   An array which can grow dynamically.  Array is derived from 
//                the SigCollection class and adds various mathematical 
//                operators to the SigCollection class.  The Array template 
//                class is used for storing numbers of any type which can 
//                be added, multiplied and divided into one another.
//

#ifndef _ARRAY_H_INCLUDED
#define _ARRAY_H_INCLUDED

#include "SigCollection.h"

#ifndef OLDCPP
   #include <ostream>
   using namespace std;
#else
   #include <ostream.h>
#endif

template<class type>
class Array : public SigCollection<type> {
   public:
                     Array             (void);
                     Array             (int arraySize);
                     Array             (Array<type>& aArray);
                     Array             (int arraySize, type *anArray);
                    ~Array             ();

      void           setAll            (type aValue);
      void           setAll            (type aValue, type increment);
      type           sum               (void);
      type           sum               (int lowIndex, int hiIndex);
      void           zero              (int minIndex = -1, int maxIndex = -1);

      int            operator==        (const Array<type>& aArray);
      int            operator==        (const char* aString);
      Array<type>&   operator=         (const Array<type>& aArray);
      Array<type>&   operator=         (const char* string);
      Array<type>&   operator+=        (const Array<type>& aArray);
      Array<type>&   operator-=        (const Array<type>& aArray);
      Array<type>&   operator*=        (const Array<type>& aArray);
      Array<type>&   operator/=        (const Array<type>& aArray);

      Array<type>    operator+         (const Array<type>& aArray) const;
      Array<type>    operator+         (type aNumber) const;
      Array<type>    operator-         (const Array<type>& aArray) const;
      Array<type>    operator-         (void) const;

      Array<type>    operator-         (type aNumber) const;
      Array<type>    operator*         (const Array<type>& aArray) const;
      Array<type>    operator*         (type aNumber) const;
      Array<type>    operator/         (const Array<type>& aArray) const;
};


// special function for printing Array<char> values:
// These fuctions are defined in src/Array-typed.cpp
ostream& operator<<(ostream& out, Array<char>& astring);
ostream& operator<<(ostream& out, Array<int>& alist);
ostream& operator<<(ostream& out, Array<double>& alist);
ostream& operator<<(ostream& out, Array<float>& alist);


#include "Array.cpp"   /* necessary for templates */


#endif  /* _ARRAY_H_INCLUDED */


// md5sum: 09d1b1f8e70ecde53f484548e48f33c3 Array.h [20030102]
