/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */
/* This class was derived from the String class written by Michael Olivero (mike95@mike95.com) */
 
/* NOTE TO MACOS/X X-CODE USERS:  If you are trying to #include <string.h>
 * and X-Code is "helpfully" pulling in this file instead (because the
 * OS/X filesystem is case-insensitive), you can get around that problem
 * by adding "USE_HEADERMAP = NO" to your X-Code target settings.
 * ref:  http://lists.apple.com/archives/xcode-users/2004/Aug/msg00934.html
 *  --Jeremy
 */

#ifndef MuscleString_h
#define MuscleString_h

#include <string.h> 
#include <ctype.h> 
#include "support/Flattenable.h"

BEGIN_NAMESPACE(muscle);

#ifndef SMALL_MUSCLE_STRING_LENGTH
# define SMALL_MUSCLE_STRING_LENGTH 7  // strings shorter than this length can be stored inline, without requiring an extra new[].
#endif

/** A nice hashing function for use with (const char *)'s */
uint32 CStringHashFunc(const char * str); 

/** A character string class.  Represents a dynamically resizable NUL-terminated string. */
class String : public Flattenable {
public:
   /** Constructor.
    *  @param str If non-NULL, the initial value for this String.
    *  @param maxLen The maximum number of characters to place into
    *                this String (not including the NUL terminator byte).
    *                Default is unlimited (i.e. scan the entire string no matter how long it is)
    */
   String(const char * str = NULL, uint32 maxLen = ((uint32)-1)) : Flattenable(), _buffer(NULL), _bufferLen(0), _length(0) {if (str) (void) SetCstr(str, maxLen);}

   /** Copy Constructor. 
     * @param str String to become a copy of.
     */
   String(const String & str) : Flattenable(), _buffer(NULL), _bufferLen(0), _length(0) {(void) SetFromString(str);}

   /** This constructor sets this String to be a substring of the specified String.
     * @param str String to become a copy of.
     * @param beginIndex Index of the first character in (str) to include.
     * @param endIndex Index after the last character in (str) to include.  
     *                 Defaults to a very large number, so that by default the entire remainder of the string is included.
     */
   String(const String & str, uint32 beginIndex, uint32 endIndex=((uint32)-1)) : Flattenable(), _buffer(NULL), _bufferLen(0), _length(0) {(void) SetFromString(str, beginIndex, endIndex);}

   /** Destructor. */
   virtual ~String();

   /** Assignment Operator.  Sets this string to be a one-character string
     * @param val The character that should be the sole character in this String.
     */
   String & operator = (char val) {(void) SetCstr(&val, 1); return *this;}

   /** Assignment Operator. 
     * @param val Pointer to the C-style string to copy from.  If NULL, this string will become "".
     */
   String & operator = (const char * val) {(void) SetCstr(val); return *this;}

   /** Assignment Operator. 
     * @param rhs String to become a copy of.
     */
   String & operator = (const String &rhs) {(void) SetFromString(rhs); return *this;}

   /** Append Operator. 
    *  @param rhs A string to append to this string.
    */
   String & operator += (const String &rhs);

   /** Append Operator. 
    *  @param rhs A string to append to this string.  If NULL, this operation is a no-op.
    */
   String & operator += (const char * rhs);

   /** Append Operator.
    *  @param ch A character to append to this string.
    */
   String & operator += (const char ch)
   {
      if (EnsureBufferSize(Length()+2, true) == B_NO_ERROR)
      {
         _buffer[_length++] = ch;
         _buffer[_length]   = '\0';
      }
      return *this;
   }
   
   /** Remove Operator. 
    *  @param rhs A substring to remove from this string;  the
    *             last instance of the substring will be cut out.
    *             If (rhs) is not found, there is no effect.
    */
   String & operator -= (const String &rhs);

   /** Remove Operator.
    *  @param ch A character to remove from this string;  the last
    *            instance of this char will be cut out.  If (ch) is
    *            not found, there is no effect.
    */
   String & operator -= (const char ch);
   
   /** Append 'Stream' Operator.
    *  @param rhs A String to append to this string.
    *  @return a non const String refrence to 'this' so you can chain appends.
    */
   String & operator << (const String& rhs) {return (*this += rhs);}

   /** Append 'Stream' Operator.
    *  @param rhs A const char* to append to this string.
    *  @return a non const String refrence to 'this' so you can chain appends.
    */
   String & operator << (const char* rhs) {return (*this += rhs);}
   
   /** Append 'Stream' Operator.
    *  @param rhs An int to append to this string.
    *  @return a non const String refrence to 'this' so you can chain appends.
    */
   String & operator << (int rhs);   

   /** Append 'Stream' Operator.
    *  @param rhs A float to append to this string. Formatting is set at 2 decimals of precision.
    *  @return a non const String refrence to 'this' so you can chain appends.
    */
   String & operator << (float rhs);   

   /** Append 'Stream' Operator.
    *  @param rhs A bool to append to this string. Converts to 'true' and 'false' strings appropriately.
    *  @return a non const String refrence to 'this' so you can chain appends.
    */
   String & operator << (bool rhs);

   /** Comparison Operator.  Returns true if the two strings are equal (as determined by strcmp())
     * @param rhs A string to compare ourself with
     */
   bool operator == (const String &rhs) const {return ((this == &rhs)||((Length() == rhs.Length())&&(strcmp(Cstr(), rhs.Cstr()) == 0)));}

   /** Comparison Operator.  Returns true if the two strings are equal (as determined by strcmp())
     * @param rhs Pointer to a C string to compare with.  NULL pointers are considered a synonym for "".
     */
   bool operator == (const char * rhs) const {return (strcmp(Cstr(), rhs?rhs:"") == 0);}

   /** Comparison Operator.  Returns true if the two strings are not equal (as determined by strcmp())
     * @param rhs A string to compare ourself with
     */
   bool operator != (const String &rhs) const {return !(*this == rhs);}

   /** Comparison Operator.  Returns true if the two strings are not equal (as determined by strcmp())
     * @param rhs Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool operator != (const char * rhs) const {return (strcmp(Cstr(), rhs?rhs:"") != 0);}

   /** Comparison Operator.  Returns true if this string comes before (rhs) lexically. 
     * @param rhs A string to compare ourself with
     */
   bool operator < (const String &rhs) const {return (this == &rhs) ? false : (strcmp(Cstr(), rhs.Cstr()) < 0);}

   /** Comparison Operator.  Returns true if this string comes before (rhs) lexically. 
     * @param rhs Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool operator < (const char * rhs) const {return (strcmp(Cstr(), rhs?rhs:"") < 0);}

   /** Comparison Operator.  Returns true if this string comes after (rhs) lexically. 
     * @param rhs A string to compare ourself with
     */
   bool operator > (const String &rhs) const {return (this == &rhs) ? false : (strcmp(Cstr(), rhs.Cstr()) > 0);}

   /** Comparison Operator.  Returns true if this string comes after (rhs) lexically. 
     * @param rhs Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool operator > (const char * rhs) const {return (strcmp(Cstr(), rhs?rhs:"") > 0);}

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes before (rhs) lexically. 
     * @param rhs A string to compare ourself with
     */
   bool operator <= (const String &rhs) const {return (this == &rhs) ? true : (strcmp(Cstr(), rhs.Cstr()) <= 0);}

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes before (rhs) lexically. 
     * @param rhs Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool operator <= (const char * rhs) const {return (strcmp(Cstr(), rhs?rhs:"") <= 0);}

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes after (rhs) lexically. 
     * @param rhs A string to compare ourself with
     */
   bool operator >= (const String &rhs) const {return (this == &rhs) ? true : (strcmp(Cstr(), rhs.Cstr()) >= 0);}

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes after (rhs) lexically. 
     * @param rhs Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool operator >= (const char * rhs) const {return (strcmp(Cstr(), rhs?rhs:"") >= 0);}

   /** Array Operator.  Used to get easy access to the characters that make up this string.
    *  @param index Index of the character to return.  Be sure to only use valid indices!
    */
   char operator [] (uint32 index) const {VerifyIndex(index); return _buffer[index];}

   /** Array Operator.  Used to get easy access to the characters that make up this string.
    *  @param index Index of the character to set.  Be sure to only use valid indices!
    */
   char & operator [] (uint32 index) {VerifyIndex(index); return _buffer[index];}

   /** Returns the character at the (index)'th position in the string.
    *  @param index A value between 0 and (Length()-1), inclusive.
    *  @return A character value.
    */
   char CharAt(uint32 index) const {return operator[](index);}
 
   /** Compares this string to another string using strcmp() 
     * @param rhs A string to compare ourself with
     */
   int CompareTo(const String & rhs) const {return strcmp(Cstr(), rhs.Cstr());}

   /** Compares this string to a C string using strcmp()
     * @param rhs Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   int CompareTo(const char * rhs) const {return strcmp(Cstr(), rhs?rhs:"");}

   /** Returns a read-only C-style pointer to our held character string. */
   const char * Cstr() const {return _buffer ? _buffer : "";}

   /** Convenience synonym for Cstr(). */
   const char * operator()() const {return Cstr();}  

   /** Clears this string so that it contains no characters.  Equivalent to setting it to "". */
   void Clear() {if (_buffer) _buffer[0] = '\0'; _length = 0;}

   /** Sets our state from the given C-style string.
     * @param str The new string to copy from.  If maxLen is negative, this may be NULL.
     * @param maxLen If set, the number of characters to copy (not including the NUL
     *               terminator byte).  By default, the number of characters is determined
     *               automatically by scanning the string.
     */
   status_t SetCstr(const char * str, uint32 maxLen = ((uint32)-1));

   /** Sets our state from the given String.  This is similar to the copy constructor, except
     * that it allows you to optionally specify a maximum length, and it allows you to detect
     * out-of-memory errors.
     * @param str The new string to copy from.
     * @param beginIndex Index of the first character in (str) to include.
     * @param endIndex Index after the last character in (str) to include.  
     *                 Defaults to a very large number, so that by default the entire remainder of the string is included.
     * @returns B_NO_ERROR on success, or B_ERROR on failure (out of memory?)
     */
   status_t SetFromString(const String & str, uint32 firstChar = 0, uint32 maxLen = ((uint32)-1));

   /** Returns true iff this string starts with (prefix) 
     * @param c a character to check for at the end of this String.
     */
   bool EndsWith(char c) const {return (_length > 0)&&(_buffer[_length-1] == c);}

   /** Returns true iff this string ends with (suffix) 
     * @param suffix a String to check for at the end of this String.
     */
   bool EndsWith(const String &suffix) const {return (Length() < suffix.Length()) ? false : (strcmp(Cstr()+(Length()-suffix.Length()), suffix.Cstr()) == 0);}

   /** Returns true iff this string ends with (suffix) 
     * @param suffix a String to check for at the end of this String.  NULL pointers are treated as a synonym for "".
     */
   bool EndsWith(const char * suffix) const
   { 
      if (suffix == NULL) suffix = "";
      uint32 suffixLen = strlen(suffix);
      return (Length() < suffixLen) ? false : (strcmp(Cstr()+(Length()-suffixLen), suffix) == 0); 
   }

   /** Returns true iff this string is equal to (string), as determined by strcmp(). 
     * @param str a String to compare this String with.
     */
   bool Equals(const String & str) const {return (*this == str);}

   /** Returns true iff this string is equal to (str), as determined by strcmp(). 
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool Equals(const char * str) const {return (*this == str);}

   /** Returns true iff this string contains a single character (c). 
     * @param c a character to compare this String with.
     */
   bool Equals(char c) const {return (_length == 1)&&(_buffer[0] == c);}

   /** Returns the first index of (ch) in this string starting at or after (fromIndex), or -1 if not found. 
     * @param ch A character to look for in this string.
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int IndexOf(char ch, uint32 fromIndex = 0) const
   {
      const char * temp = (fromIndex < Length()) ? strchr(Cstr()+fromIndex, ch) : NULL; 
      return temp ? (temp - Cstr()) : -1; 
   }

   /** Returns the first index of substring (str) in this string starting at or after (fromIndex), or -1 if not found. 
     * @param str A String to look for in this string.
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int IndexOf(const String &str, uint32 fromIndex = 0) const
   {
      const char * temp = (fromIndex < Length()) ? strstr(Cstr()+fromIndex, str()) : NULL;
      return temp ? (temp - Cstr()) : -1;
   }

   /** Returns the first index of substring (str) in this string starting at or after (fromIndex), or -1 if not found.
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int IndexOf(const char * str, uint32 fromIndex = 0) const
   {
      const char * temp = (fromIndex < Length()) ? strstr(Cstr()+fromIndex, str?str:"") : NULL;
      return temp ? (temp - Cstr()) : -1;
   }

   /** Returns the last index of (ch) in this string starting at or after (fromIndex), or -1 if not found. 
     * @param ch A character to look for in this string.
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int LastIndexOf(char ch, uint32 fromIndex = 0) const
   {
      const char * lio = (fromIndex < Length()) ? strrchr(Cstr()+fromIndex, ch) : NULL;
      return lio ? (lio - Cstr()) : -1;
   }

   /** Returns the last index of substring (str) in this string  
     * @param str A String to look for in this string.
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int LastIndexOf(const String &str) const {return (str.Length() <= Length()) ? LastIndexOf(str, Length()-str.Length()) : -1;}

   /** Returns the last index of substring (str) in this string 
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   int LastIndexOf(const char * str) const
   {
      if (str == NULL) str = "";
      uint32 strLen = strlen(str);
      return (strLen <= Length()) ? LastIndexOf(str, Length()-strLen) : -1;
   }

   /** Returns the last index of substring (str) in this string starting at or after (fromIndex), or -1 if not found.
     * @param str A String to look for in this string.
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int LastIndexOf(const String &str, uint32 fromIndex) const;

   /** Returns the last index of substring (str) in this string starting at or after (fromIndex), or -1 if not found. 
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     * @param fromIndex Index of the first character to start searching at in this String.  Defaults to zero (i.e. start from the first character)
     */
   int LastIndexOf(const char * str, uint32 fromIndex) const;

   /** Returns the number of characters in the string (not including the terminating NUL byte) */
   uint32 Length() const {return _length;}

   /** Returns the number of instances of (c) in this string. */
   uint32 GetNumInstancesOf(char ch) const;

   /** Returns the number of instances of (substring) in this string. */
   uint32 GetNumInstancesOf(const String & substring) const;

   /** Returns the number of instances of (substring) in this string. */
   uint32 GetNumInstancesOf(const char * substring) const;

   /** Returns true iff this string starts with (prefix) */
   bool StartsWith(char c) const {return (_length > 0)&&(_buffer[0] == c);}

   /** Returns true iff this string starts with (prefix) */
   bool StartsWith(const String &str) const {return ((Length() >= str.Length())&&(strncmp(Cstr(), str(), str.Length()) == 0));}

   /** Returns true iff this string starts with (prefix)
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool StartsWith(const char * prefix) const
   {
      if (prefix == NULL) prefix = "";
      uint32 prefixLen = strlen(prefix);
      return (Length() < prefixLen) ? false : (strncmp(Cstr(), prefix, prefixLen) == 0);
   }

   /** Returns true iff this string starts with the first (offset) characters of (prefix) */
   bool StartsWith(const String &prefix, uint32 offset) const {return ((offset+prefix.Length()<=Length())&&(strncmp(Cstr()+offset, prefix.Cstr(), prefix.Length()) == 0));}

   /** Returns true iff this string starts with the first (offset) characters of (prefix) 
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     */
   bool StartsWith(const char * prefix, uint32 offset) const
   {
      if (prefix == NULL) prefix = "";
      uint32 prefixLen = strlen(prefix);
      return ((offset+prefixLen<=Length())&&(strncmp(Cstr()+offset, prefix, prefixLen) == 0));
   }

   /** Returns a string that consists of (count) copies of (str), followed by this string. */
   String Prepend(const String & str, uint32 count = 1) const;

   /** Returns a string that consists of (count) copies of (str), followed by this string. 
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     * @param count How many instances of (str) should be prepended to this string.  Defaults to 1.
     */
   String Prepend(const char * str, uint32 count = 1) const;

   /** Returns a string that consists of this string followed by (count) copies of (str). */
   String Append(const String & str, uint32 count = 1) const;

   /** Returns a string that consists of this string followed by (count) copies of (str). 
     * @param str Pointer to a C string to compare to.  NULL pointers are considered a synonym for "".
     * @param count How many instances of (str) should be appended to this string.  Defaults to 1.
     */
   String Append(const char * str, uint32 count = 1) const;

   /** Returns a string that is like this string, but padded out to the specified minimum length with (padChar).
    *  @param minLength Minimum length that the returned string should be.
    *  @param padOnRight If true, (padChar)s will be added to the right; if false (the default), they will be added on the left.
    *  @param padChar The character to pad out the string with.  Defaults to ' '.
    *  @returns the new, padded String.
    */
   String Pad(uint32 minLength, bool padOnRight = false, char padChar = ' ') const;

   /** Returns a string that consists of only the last part of this string, starting with index (beginIndex).  Does not modify the string it is called on. */
   String Substring(uint32 beginIndex) const {return String(*this, beginIndex);}

   /** Returns a string that consists of only the characters in this string from range (beginIndex) to (endIndex-1).  Does not modify the string it is called on. */
   String Substring(uint32 beginIndex, uint32 endIndex) const {return String(*this, beginIndex, endIndex);}

   /** Returns a string that consists of only the last part of this string, starting with the first character after the last instance of (markerString).  
    *  If (markerString) is not found in the string, then this entire String is returned.
    *  For example, String("this is a test").Substring("is a") returns " test".
    *  Does not modify the string it is called on. 
    */
   String Substring(const String & markerString) const
   {
      int idx = LastIndexOf(markerString);
      return (idx >= 0) ? String(*this, idx+markerString.Length()) : *this;
   }

   /** See the Substring(const String &) documentation for details. */
   String Substring(const char * markerString) const
   {
      int idx = LastIndexOf(markerString);
      return (idx >= 0) ? String(*this, idx+strlen(markerString)) : *this;  // if (idx >= 0), then we know markerString is non-NULL
   }

   /** Returns a string that consists of only the characters in the string from range (beginIndex) until the character just before
    *  the first character in (markerString).  If (markerString) is not found, then the entire substring starting at (beginIndex) is returned.
    *  For example, String("this is a test").Substring(1, "is a") returns "his ".
    *  Does not modify the string it is called on. 
    */
   String Substring(uint32 beginIndex, const String & markerString) const {return String(*this, beginIndex, (uint32) IndexOf(markerString, beginIndex));}

   /** See the Substring(uint32, const String &) documentation for details. */
   String Substring(uint32 beginIndex, const char * markerString) const {return String(*this, beginIndex, (uint32) IndexOf(markerString, beginIndex));}

   /** Returns an all lower-case version of this string.  Does not modify the string it is called on. */
   String ToLowerCase() const; 

   /** Returns an all upper-case version of this string.  Does not modify the string it is called on. */
   String ToUpperCase() const; 

   /** Returns an version of this string that has all leading and trailing whitespace removed.  Does not modify the string it is called on. */
   String Trim() const;  

   /** Swaps the state of this string with (swapWithMe).  Very efficient since little or no data copying is required. */
   void SwapContents(String & swapWithMe);

   /** Like CompareTo(), but case insensitive. */
   int CompareToIgnoreCase(const String &s) const             {return ToLowerCase().CompareTo(s.ToLowerCase());}

   /** Like EndsWith(), but case insensitive. */
   bool EndsWithIgnoreCase(char c) const                      {return (_length > 0)&&(tolower(_buffer[_length-1]) == tolower(c));}

   /** Like EndsWith(), but case insensitive. */
   bool EndsWithIgnoreCase(const String &s) const             {return ToLowerCase().EndsWith(s.ToLowerCase());}

   /** Like Equals(), but case insensitive. */
   bool EqualsIgnoreCase(const String &s) const               {return ToLowerCase().Equals(s.ToLowerCase());}

   /** Like Equals(), but case insensitive. */
   bool EqualsIgnoreCase(char c) const                        {return (_length==1)&&(tolower(_buffer[0])==tolower(c));}

   /** Like IndexOf(), but case insensitive. */
   int IndexOfIgnoreCase(const String &s) const               {return ToLowerCase().IndexOf(s.ToLowerCase());}

   /** Like IndexOf(), but case insensitive. */
   int IndexOfIgnoreCase(const String &s, uint32 f) const     {return ToLowerCase().IndexOf(s.ToLowerCase(),f);}

   /** Like IndexOf(), but case insensitive. */
   int IndexOfIgnoreCase(char ch) const                       {return ToLowerCase().IndexOf((char)tolower(ch));}

   /** Like IndexOf(), but case insensitive. */
   int IndexOfIgnoreCase(char ch, uint32 f) const             {return ToLowerCase().IndexOf((char)tolower(ch),f);}

   /** Like LastIndexOf(), but case insensitive. */
   int LastIndexOfIgnoreCase(const String &s) const           {return ToLowerCase().LastIndexOf(s.ToLowerCase());}

   /** Like LastIndexOf(), but case insensitive. */
   int LastIndexOfIgnoreCase(const String &s, uint32 f) const {return ToLowerCase().LastIndexOf(s.ToLowerCase(),f);}

   /** Like LastIndexOf(), but case insensitive. */
   int LastIndexOfIgnoreCase(char ch) const                   {return ToLowerCase().LastIndexOf((char)tolower(ch));}

   /** Like LastIndexOf(), but case insensitive. */
   int LastIndexOfIgnoreCase(char ch, uint32 f) const         {return ToLowerCase().LastIndexOf((char)tolower(ch),f);}

   /** Like EndsWith(), but case insensitive. */
   bool StartsWithIgnoreCase(char c) const                    {return (_length > 0)&&(tolower(_buffer[0]) == tolower(c));}

   /** Like StartsWith(), but case insensitive. */
   bool StartsWithIgnoreCase(const String &s) const           {return ToLowerCase().StartsWith(s.ToLowerCase());}

   /** Like StartsWith(), but case insensitive. */
   bool StartsWithIgnoreCase(const String &s, uint32 o) const {return ToLowerCase().StartsWith(s.ToLowerCase(),o);}

   /** Returns a hash code for this string */
   inline uint32 HashCode() const {return CStringHashFunc(Cstr());}

   /** Replaces all instances of (oldChar) in this string with (newChar).
     * @param replaceMe The character to search for.
     * @param withMe The character to replace all occurrences of (replaceMe) with.
     * @returns The number of characters that were successfully replaced.
     */
   uint32 Replace(char replaceMe, char withMe); 

   /** Replaces all instances of (replaceMe) in this string with (withMe).
     * @param replaceMe The substring to search for.
     * @param withMe The substring to replace all occurrences of (replaceMe) with.
     * @returns The number of substrings that were successfully replaced, or -1
     *          if the operation failed (out of memory, or (replaceMe) was of length zero)
     */
   int32 Replace(const String & replaceMe, const String & withMe); 
 
   /** Reverses the order of all characters in the string, so that e.g. "string" becomes "gnirts" */
   void Reverse();

   /** Part of the Flattenable interface.
    *  @return false
    */
   virtual bool IsFixedSize() const {return false;}

   /** Part of the Flattenable interface.
    *  @return B_STRING_TYPE
    */
   virtual uint32 TypeCode() const {return B_STRING_TYPE;}

   /** Part of the Flattenable interface.
    *  @return Length()+1  (the +1 is for the terminating NUL byte)
    */
   virtual uint32 FlattenedSize() const;

   /** Part of the Flattenable interface.  Flattens our string into (buffer).
    *  @param buffer A byte array to receive the flattened version of this string.
    *                There must be at least FlattenedSize() bytes available in this array.
    *  The clever secret here is that a flattened String is just a C-style
    *  null-terminated character array, and can be used interchangably as such.
    */
   virtual void Flatten(uint8 *buffer) const;

   /** Unflattens a String from (buf).
    *  @param buf an array of (size) bytes.
    *  @param size the number of bytes in (buf).
    *  @return B_NO_ERROR (never fails!)
    */
   virtual status_t Unflatten(const uint8 *buf, uint32 size);

   /** Makes sure that we have pre-allocated enough space for a NUL-terminated string 
    *  at least (numChars) bytes long (not including the NUL byte).
    *  If not, this method will try to allocate the space.  
    *  @param numChars How much space to pre-allocate, in ASCII characters.
    *  @returns B_NO_ERROR on success, or B_ERROR on failure (out of memory).
    */ 
   status_t Prealloc(uint32 numChars) {return EnsureBufferSize(numChars+1, true);}

   /** Returns a string like this string, but with the appropriate %# tokens
     * replaced with a textual representation of the values passed in as (value).
     * For example, String("%1 is a %2").Arg(13).Arg("bakers dozen") would return
     * the string "13 is a bakers dozen".
     * @param value The value to replace in the string.
     * @param fmt The format parameter to pass to sprintf().  Note that if you supply your
     *            own format string, you'll need to be careful since a badly chosen format
     *            string could lead to a crash or out-of-bounds memory overwrite.  In particular,
     *            the format string should match the type specified, and should not lead to
     *            more than 256 bytes being written.
     * @returns a new String with the appropriate tokens replaced.
     */
   String Arg(int8 value,   const char * fmt = "%i")   const;

   /** As above, but for uint8 values. */
   String Arg(uint8 value,  const char * fmt = "%u")   const;

   /** As above, but for int16 values. */
   String Arg(int16 value,  const char * fmt = "%i")   const;

   /** As above, but for uint16 values. */
   String Arg(uint16 value, const char * fmt = "%u")   const;

   /** As above, but for int32 values. */
   String Arg(int32 value,  const char * fmt = "%li")  const;

   /** As above, but for uint32 values. */
   String Arg(uint32 value, const char * fmt = "%lu")  const;

   /** As above, but for int64 values. */
   String Arg(int64 value,  const char * fmt = INT64_FORMAT_SPEC) const;

   /** As above, but for uint64 values. */
   String Arg(uint64 value, const char * fmt = UINT64_FORMAT_SPEC) const;

   /** As above, but for double values. */
   String Arg(double value, const char * fmt = "%f")   const;

   /** As above, but for string values. */
   String Arg(const String & value) const;
   String Arg(const char * value) const;

private:
   bool IsSpaceChar(char c) const {return ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'));}
   status_t EnsureBufferSize(uint32 newBufLen, bool retainValue);
   String ArgAux(const char * buf) const;

   char _smallBuffer[SMALL_MUSCLE_STRING_LENGTH+1];  // store very small strings in-line, to avoid dynamic allocation
   char * _buffer;            // Stores the chars.  May point at (_smallBuffer), or a dynamically allocated buffer
   uint32 _bufferLen;         // Number of bytes pointed to by (_buffer)
   uint32 _length;            // cached strlen(_buffer)

   void VerifyIndex(uint32 index) const 
   {
#ifdef MUSCLE_AVOID_ASSERTIONS
      (void) index;  // avoid compiler warnings
#else
      MASSERT(index < _length, "Index Out Of Bounds Exception");
#endif
   }
};

/** Convenience method:  returns a string with no characters in it (a.k.a. "") */
const String & GetEmptyString();

template <class T> class HashFunctor;

template <>
class HashFunctor<String>
{
public:
   uint32 operator () (const String & x) const {return CStringHashFunc(x());}
};

template <>
class HashFunctor<const char *>
{
public:
   uint32 operator () (const char * x) const {return CStringHashFunc(x);}
};

/** A function for comparing (const String &)'s -- calls the String operators */
int StringCompareFunc(const String &, const String &, void *);

/** A function for comparing (const char *)'s -- calls strcmp() */
int CStringCompareFunc(const char * const &, const char * const &, void *);

inline String operator+(const String & lhs, const String &rhs)  {String ret(lhs); ret += rhs; return ret;}
inline String operator+(const String & lhs, const char *rhs)    {String ret(lhs); ret += rhs; return ret;}
inline String operator+(const char * lhs,   const String & rhs) {String ret(lhs); ret += rhs; return ret;}
inline String operator+(const String & lhs, char rhs)           {String ret(lhs); ret += rhs; return ret;}
inline String operator-(const String & lhs, const String &rhs)  {String ret(lhs); ret -= rhs; return ret;}
inline String operator-(const String & lhs, const char *rhs)    {String ret(lhs); ret -= rhs; return ret;}
inline String operator-(const char *lhs,    const String &rhs)  {String ret(lhs); ret -= rhs; return ret;}
inline String operator-(const String & lhs, char rhs)           {String ret(lhs); ret -= rhs; return ret;}

END_NAMESPACE(muscle);

#endif
