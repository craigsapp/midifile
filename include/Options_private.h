//
// Copyright 1998-1999 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr  5 13:07:18 PDT 1998
// Last Modified: Sun Jan 10 05:44:48 PST 1999
// Filename:      ...sig/maint/code/base/Options/Options_private.h
// Web Address:   http://sig.sapp.org/include/sigBase/Options_private.h
// Syntax:        C++ 
//
// Description:   A private function for use in the Options class.
//

#ifndef _OPTIONS_PRIVATE_H_INCLUDED
#define _OPTIONS_PRIVATE_H_INCLUDED


class option_register {
   public:
                 option_register          (void);
                 option_register          (const char* aDefinition, char aType,
                                             const char* aDefaultOption,
                                             const char* aModifiedOption);
                 ~option_register         ();
     void         clearModified           (void);
     const char*  getDefinition           (void);
     const char*  getDefault              (void);
     const char*  getOption               (void);
     const char*  getModified             (void);
     int          getModifiedQ            (void);
     char         getType                 (void);
     void         reset                   (void);
     void         setDefault              (const char* aString);
     void         setDefinition           (const char* aString);
     void         setModified             (const char* aString);
     void         setType                 (char aType);

   protected:
      char*       definition;
      char*       defaultOption;
      char*       modifiedOption;
      char        type;

};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


class option_list {
   public:
                     option_list       (void);
                     option_list       (const char* optionName, int anIndex);
                    ~option_list       ();

      int            getIndex          (void);
      const char*    getName           (void);
      void           setName           (const char* aString);
      void           setIndex          (int anIndex);

   protected:
      char*          name;
      int            index;

};



#endif  /* _OPTIONS_PRIVATE_H_INCLUDED */



// md5sum: b440ad2158e9921d0e31463a8c3e1ae0 Options_private.h [20030102]
