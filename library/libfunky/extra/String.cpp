/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include "util/String.h"
#include "system/GlobalMemoryAllocator.h"
#include <stdarg.h>

BEGIN_NAMESPACE(muscle);

String :: ~String() 
{
   if (_buffer != _smallBuffer) muscleFree(_buffer);
}

status_t
String::SetFromString(const String & s, uint32 firstChar, uint32 afterLastChar)
{
   afterLastChar = muscleMin(afterLastChar, s.Length());
   uint32 len = (afterLastChar > firstChar) ? (afterLastChar-firstChar) : 0;
   if (len > 0)
   {
      if (EnsureBufferSize(len+1, false) != B_NO_ERROR) return B_ERROR;
      if (&s == this) memmove(_buffer, s()+firstChar, len);
                 else memcpy( _buffer, s()+firstChar, len);
      _buffer[len] = '\0';
      _length = len;
   }
   else Clear();

   return B_NO_ERROR;
}

status_t
String::SetCstr(const char * str, uint32 maxLen)
{
   // If (str)'s got a NUL byte before maxLen, make (maxLen) smaller.
   // We can't call strlen(str) because we don't have any guarantee that the NUL 
   // byte even exists!  Without a NUL byte, strlen() could run off into the weeds...
   uint32 sLen = 0;
   if (str) {while((sLen<maxLen)&&(str[sLen] != '\0')) sLen++;}
   if (sLen < maxLen) maxLen = sLen;
   if (maxLen > 0)
   {
      if (str[maxLen-1] != '\0') maxLen++;  // make room to add the NUL byte if necessary
      if (EnsureBufferSize(maxLen, false) != B_NO_ERROR) return B_ERROR;
      if (muscleInRange((char *)str, _buffer, &_buffer[_length])) memmove(_buffer, str, maxLen-1);
                                                             else memcpy(_buffer, str, maxLen-1);
      _buffer[maxLen-1] = '\0';
      _length = maxLen-1;
   }
   else Clear();

   return B_NO_ERROR;
}

String &
String::operator-=(const char aChar)
{
   int idx = LastIndexOf(aChar);
   if (idx >= 0)
   {
      String temp = Substring(idx+1);
      (*this) = this->Substring(0, idx);
      (*this) += temp;
   }
   return *this;
}

String &
String::operator+=(const String &other)
{
   uint32 otherLen = other.Length();
   if ((otherLen > 0)&&(EnsureBufferSize(Length() + otherLen + 1, true) == B_NO_ERROR))
   {
      memcpy(&_buffer[_length], other(), other.Length()+1);
      _length += otherLen;
   }
   return *this;
}

String &
String::operator+=(const char * other)
{
   if (other == NULL) other = "";
   uint32 otherLen = strlen(other);
   if (otherLen > 0)
   {
           if (muscleInRange(other, (const char *)_buffer, (const char *)(_buffer+_length))) return operator+=(String(other));  // avoid self-entanglement
      else if (EnsureBufferSize(Length() + otherLen + 1, true) == B_NO_ERROR)
      {
         memcpy(&_buffer[_length], other, otherLen+1);
         _length += otherLen;
      }
   }
   return *this;
}

String &
String::operator-=(const String &other)
{
   if (*this == other) *this = "";
   else
   {
      int idx = LastIndexOf(other);
      if (idx >= 0)
      {
         String temp = Substring(idx+other.Length());
         (*this) = Substring(0, idx);
         (*this) += temp;
      }
   }
   return *this;
}

String &
String::operator<<(int rhs)
{
   char buff[64];
   sprintf(buff, "%d", rhs);
   return *this << buff;
}

String &
String::operator<<(float rhs)
{
   char buff[64];
   sprintf(buff, "%.2f", rhs);
   return *this << buff;
}

String &
String::operator<<(bool rhs)
{
   const char* val = rhs ? "true" : "false"; 
   return *this << val;
}
 
void
String::Reverse()
{
   if ((_buffer)&&(Length() > 0))
   {
      uint32 from = 0;
      uint32 to = Length()-1;
      while(from<to) muscleSwap(_buffer[from++], _buffer[to--]);
   }
}

uint32
String::Replace(char findChar, char replaceChar)
{
   uint32 ret = 0; 
   if ((_buffer)&&(findChar != replaceChar))
   {
      char * c = _buffer;
      while(*c)
      {
         if (*c == findChar) 
         {
            *c = replaceChar;
            ret++;
         }
         c++;
      }
   }
   return ret;
}
   
int32
String::Replace(const String & replaceMe, const String & withMe)
{
   TCHECKPOINT;

   if (replaceMe.Length() == 0) return -1;  // can't replace an empty string, that's silly!
   if (replaceMe == withMe) return GetNumInstancesOf(replaceMe);  // no changes necessary!

   String temp;
   int32 perInstanceDelta = ((int32)withMe.Length())-((int32)replaceMe.Length());
   if (perInstanceDelta > 0)
   {
      // If we are replacing a shorter string with a longer string, we'll have to do a copy-and-swap
      uint32 numInstances = GetNumInstancesOf(replaceMe);
      if (numInstances == 0) return 0;  // no changes necessary!
      if (temp.Prealloc(Length()+(perInstanceDelta*numInstances)) != B_NO_ERROR) return -1;
   }

   // This code works for both the in-place and the copy-over modes!
   int32 ret = 0;
   const char * readPtr = Cstr();
   char * writePtr = (perInstanceDelta > 0) ? temp._buffer : NULL;
   while(1)
   {
      char * nextReplaceMe = strstr((char *) readPtr, (char *) replaceMe());
      if (nextReplaceMe)
      {
         ret++;
         if (writePtr)
         {
            uint32 numBytes = nextReplaceMe-readPtr;
            if (perInstanceDelta != 0) memmove(writePtr, readPtr, numBytes);
            writePtr += numBytes;
         }
         else writePtr = nextReplaceMe;

         memcpy(writePtr, withMe(), withMe.Length());
         readPtr  = nextReplaceMe + replaceMe.Length();
         writePtr += withMe.Length();
      }
      else
      {
         if (writePtr)
         {
            // Finish up
            uint32 numBytes = Cstr()+Length()-readPtr;
            if (perInstanceDelta != 0) memmove(writePtr, readPtr, numBytes);
            writePtr += numBytes;
            *writePtr = '\0';
            if (perInstanceDelta > 0) 
            {
               temp._length = writePtr-temp();
               SwapContents(temp);
            }
            else _length = writePtr-Cstr();
         }
         return ret;
      }
   }
   return ret;  // just to shut the compiler up; we never actually get here
}

void
String::SwapContents(String & s)
{
   TCHECKPOINT;

   if (&s != this)
   {
      bool thisSmall = (  _buffer ==   _smallBuffer);
      bool sSmall    = (s._buffer == s._smallBuffer);

      if ((sSmall)&&(thisSmall))
      {
         for (int32 i=muscleMax(_length, s._length); i>=0; i--) muscleSwap(_smallBuffer[i], s._smallBuffer[i]);
      }
      else if (thisSmall)
      {
         _buffer      = s._buffer;
         _bufferLen   = s._bufferLen;
         s._buffer    = s._smallBuffer;
         s._bufferLen = sizeof(s._smallBuffer);
         memcpy(s._smallBuffer, _smallBuffer, _length+1);
      }
      else if (sSmall)
      {
         s._buffer    = _buffer;
         s._bufferLen = _bufferLen;
         _buffer      = _smallBuffer;
         _bufferLen   = sizeof(_smallBuffer);
         memcpy(_smallBuffer, s._smallBuffer, s._length+1);
      }
      else
      {
         muscleSwap(_buffer,    s._buffer);
         muscleSwap(_bufferLen, s._bufferLen);
      }

      muscleSwap(_length, s._length);   // always do this
   }
}

int
String::LastIndexOf(const String &s2, uint32 fromIndex) const
{
   TCHECKPOINT;

   if (s2.Length() == 0) return Length()-1;
   if (fromIndex >= Length()) return -1;
   for (int i=fromIndex; i>=0; i--) if (strncmp(Cstr()+i, s2.Cstr(), s2.Length()) == 0) return i;
   return -1;
}

int
String::LastIndexOf(const char * s2, uint32 fromIndex) const
{
   TCHECKPOINT;

   if (s2 == NULL) s2 = "";
   uint32 s2Len = strlen(s2);
   if (s2Len == 0) return Length()-1;
   if (fromIndex >= Length()) return -1;
   for (int i=fromIndex; i>=0; i--) if (strncmp(Cstr()+i, s2, s2Len) == 0) return i;
   return -1;
}

String
String::ToLowerCase() const
{
   String ret(*this);
   if (ret._buffer) for (uint32 i=0; i<ret.Length(); i++) ret._buffer[i] = (char)tolower(ret._buffer[i]);
   return ret;
}

String
String::ToUpperCase() const
{
   String ret(*this);
   if (ret._buffer) for (uint32 i=0; i<ret.Length(); i++) ret._buffer[i] = (char)toupper(ret._buffer[i]);
   return ret;
}
 
String 
String::Trim() const 
{ 
   TCHECKPOINT;

   int32 len = (int32) Length();
   const char * s = Cstr();
   int32 startIdx; for (startIdx = 0;     startIdx<len;    startIdx++) if (!IsSpaceChar(s[startIdx])) break; 
   int32 endIdx;   for (endIdx   = len-1; endIdx>startIdx; endIdx--)   if (!IsSpaceChar(s[endIdx]))   break; 
   return String(*this, (uint32)startIdx, (uint32)(endIdx+1));
}

uint32 String :: FlattenedSize() const
{
   return Length()+1;   
}

void String :: Flatten(uint8 *buffer) const
{
   memcpy((char *)buffer, Cstr(), Length()+1);
}

status_t String :: Unflatten(const uint8 *buf, uint32 size)
{
   return SetCstr((const char *)buf, size);
}

uint32 String :: GetNumInstancesOf(char ch) const
{
   uint32 ret = 0;
   for (const char * s = Cstr(); (*s != '\0'); s++) if (*s == ch) ret++; 
   return ret;
}

uint32 String :: GetNumInstancesOf(const String & substring) const
{
   TCHECKPOINT;

   uint32 ret = 0;
   if (substring.Length() > 0)
   {
      uint32 lastIdx = 0;
      int32 idx;
      while((idx = IndexOf(substring, lastIdx)) >= 0)
      {
         ret++;
         lastIdx = idx + substring.Length();
      }
   }
   return ret;
}

uint32 String :: GetNumInstancesOf(const char * substring) const
{
   TCHECKPOINT;

   if (substring == NULL) substring = "";
   uint32 ret = 0;
   uint32 substringLength = strlen(substring);
   if (substringLength > 0)
   {
      uint32 lastIdx = 0;
      int32 idx;
      while((idx = IndexOf(substring, lastIdx)) >= 0)
      {
         ret++;
         lastIdx = idx + substringLength;
      }
   }
   return ret;
}

String String :: Prepend(const String & str, uint32 count) const
{
   TCHECKPOINT;

   String ret;
   uint32 newLen = (count*str.Length())+Length();
   if (ret.Prealloc(newLen) == B_NO_ERROR)
   {
      char * b = ret._buffer;

      if (str.Length() > 0)
      {
         for (uint32 i=0; i<count; i++)
         {
            memcpy(b, str(), str.Length());
            b += str.Length();
         }
      }
      if (Length() > 0)
      {
         memcpy(b, Cstr(), Length()); 
         b += Length();
      }
      ret._length = (b-ret._buffer);
      ret._buffer[ret._length] = '\0';   // terminate the string
   }
   return ret;
}

String String :: Prepend(const char * str, uint32 count) const
{
   TCHECKPOINT;

   if (muscleInRange(str, (const char *)_buffer, (const char *)(_buffer+Length()))) return Prepend(String(str), count);  // avoid self-entanglement!
   else
   {
      if (str == NULL) str = "";
      uint32 sLen = strlen(str);
      String ret;
      uint32 newLen = (count*sLen)+Length();
      if (ret.Prealloc(newLen) == B_NO_ERROR)
      {
         char * b = ret._buffer;

         if (sLen > 0)
         {
            for (uint32 i=0; i<count; i++)
            {
               memcpy(b, str, sLen);
               b += sLen;
            }
         }
         if (Length() > 0)
         {
            memcpy(b, Cstr(), Length()); 
            b += Length();
         }
         ret._length = (b-ret._buffer);
         ret._buffer[ret._length] = '\0';   // terminate the string
      }
      return ret;
   }
}

String String :: Append(const String & str, uint32 count) const
{
   TCHECKPOINT;

   String ret;
   uint32 newLen = Length()+(count*str.Length());
   if (ret.Prealloc(newLen) == B_NO_ERROR)
   {
      char * b = ret._buffer;
      if (Length() > 0)
      {
         memcpy(b, Cstr(), Length()); 
         b += Length();
      }
      if (str.Length() > 0)
      {
         for (uint32 i=0; i<count; i++)
         {
            memcpy(b, str(), str.Length());
            b += str.Length();
         }
      }
      ret._length = (b-ret._buffer);
      ret._buffer[ret._length] = '\0';   // terminate the string
   }
   return ret;
}

String String :: Append(const char * str, uint32 count) const
{
   TCHECKPOINT;

   if (muscleInRange(str, (const char *)_buffer, (const char *)(_buffer+Length()))) return Append(String(str), count);  // avoid self-entanglement!
   else
   {
      if (str == NULL) str = "";
      uint32 sLen = strlen(str);
      String ret;
      uint32 newLen = Length()+(count*sLen);
      if (ret.Prealloc(newLen) == B_NO_ERROR)
      {
         char * b = ret._buffer;
         if (Length() > 0)
         {
            memcpy(b, Cstr(), Length()); 
            b += Length();
         }
         if (sLen > 0)
         {
            for (uint32 i=0; i<count; i++)
            {
               memcpy(b, str, sLen);
               b += sLen;
            }
         }
         ret._length = (b-ret._buffer);
         ret._buffer[ret._length] = '\0';   // terminate the string
      }
      return ret;
   }
}

// This method tries to ensure that at least (newBufLen) chars
// are available for storing data in.  (requestedBufLen) should include
// the terminating NUL.  If (retainValue) is true, the current string value
// will be retained; otherwise it should be set right after this call
// returns...
status_t String::EnsureBufferSize(uint32 requestedBufLen, bool retainValue)
{
   if (requestedBufLen > _bufferLen)
   {
      if (requestedBufLen <= sizeof(_smallBuffer))  // guaranteed only to be true the very first time we are called!
      {
         // For small initial requests, we can just set up our buffer pointer to point to our static-buffer area.  Cheap!
         _buffer    = _smallBuffer;
         _bufferLen = sizeof(_smallBuffer);
      }
      else
      {
         // If we're doing a first-time allocation, allocate exactly the number of the bytes requested.
         // If it's a re-allocation, allocate more than requested as it's more likely to happen yet another time...
         uint32 newBufLen = (_buffer == NULL) ? requestedBufLen : (requestedBufLen * 2);
         if (retainValue)
         {
            if ((_buffer)&&(_buffer != _smallBuffer))
            {
               // We can call muscleRealloc() to hopefully avoid data copying
               char * newBuf = (char *)muscleRealloc(_buffer, newBufLen);
               if (newBuf)
               {
                  _buffer    = newBuf;
                  _bufferLen = newBufLen;
               }
               else
               {
                  WARN_OUT_OF_MEMORY;
                  return B_ERROR;
               }
            }
            else
            {
               // Oops, muscleRealloc() won't do.... we'll just have to copy the bytes over
               char * newBuf = (char *) muscleAlloc(newBufLen);
               if (newBuf == NULL) 
               {
                  WARN_OUT_OF_MEMORY;
                  return B_ERROR;
               }
               if (_buffer) memcpy(newBuf, _buffer, Length()+1);
               if (_buffer != _smallBuffer) muscleFree(_buffer);
               _buffer    = newBuf;
               _bufferLen = newBufLen;
            }
         }
         else
         {
            // If the caller doesn't care about retaining the value, then it's
            // probably cheaper just to free our buffer and get a new one.
            char * newBuf = (char *) muscleAlloc(newBufLen);
            if (newBuf == NULL) 
            {
               WARN_OUT_OF_MEMORY;
               return B_ERROR;
            }
            if (_buffer != _smallBuffer) muscleFree(_buffer);
            _buffer    = newBuf;
            _bufferLen = newBufLen;
         }
      }
   }
   return B_NO_ERROR;
}

String String :: Pad(uint32 minLength, bool padOnRight, char padChar) const
{
   if (Length() < minLength)
   {
      uint32 padLen = minLength-Length();
      String temp; temp += padChar;
      return (padOnRight) ? Append(temp, padLen) : Prepend(temp, padLen);
   }
   else return *this;
}

#define ARG_IMPLEMENTATION   \
   char buf[256];            \
   sprintf(buf, fmt, value); \
   return ArgAux(buf)

String String :: Arg(int8   value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(uint8  value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(int16  value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(uint16 value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(int32  value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(uint32 value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(int64  value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(uint64 value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(double value, const char * fmt) const {ARG_IMPLEMENTATION;}
String String :: Arg(const String & value)           const {return ArgAux(value());}
String String :: Arg(const char * value)             const {return ArgAux(value);}

String String :: ArgAux(const char * buf) const
{
   TCHECKPOINT;

   int32 lowestArg = -1;
   const char * s = Cstr();
   while(*s != '\0')
   {
      if (*s == '%')
      {
         s++;
         if (muscleInRange(*s, '0', '9'))
         {
            int32 val = atol(s);
            lowestArg = (lowestArg < 0) ? val : muscleMin(val, lowestArg);
            while(muscleInRange(*s, '0', '9')) s++; 
         }
      }
      else s++;
   }

   if (lowestArg >= 0)
   {
      char token[64];
      sprintf(token, "%%%li", lowestArg);
      String ret(*this);
      (void) ret.Replace(token, buf);
      return ret;
   }
   else return *this;
}

const String & GetEmptyString()
{
   static const String _empty;
   return _empty;
}

/*--- ElfHash --------------------------------------------------- 
 *  The published hash algorithm used in the UNIX ELF format 
 *  for object files. Accepts a pointer to a string to be hashed 
 *  and returns an unsigned long. 
 *  jaf:  stolen from: http://www.ddj.com/articles/1996/9604/9604b/9604b.htm?topic=algorithms
 *-------------------------------------------------------------*/ 
uint32 CStringHashFunc(const char * n)
{
    uint32 h = 0, g; 
    const unsigned char * name = (const unsigned char *) n;
    while (*name) 
    { 
        h = (h << 4) + *name++; 
        if ((g = h & 0xF0000000) != 0) h ^= g >> 24; 
        h &= ~g; 
    } 
    return h; 
}

int CStringCompareFunc(const char * const & s1, const char * const & s2, void *)
{
    return strcmp(s1, s2);
}

int StringCompareFunc(const String & s1, const String & s2, void *)
{
   return muscleCompare(s1, s2);
}

END_NAMESPACE(muscle);
