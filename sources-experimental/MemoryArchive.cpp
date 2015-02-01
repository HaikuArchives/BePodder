/*
 *   Copyright (C) 2005 by Andrea Anzani
 *   oxygenum@tiscali.it      
 *	
*/

#include "MemoryArchive.h"
#include <stdlib.h>



MemoryArchive & MemoryArchive::operator=(const MemoryArchive &msg){
	this->fData = msg.fData;
	return *this;
}

void	
MemoryArchive::SetKey(ItemProperty key,void* data,ssize_t numBytes)	{

	BString superkey("key_");
	superkey << key;
	
	if(fData.HasData(superkey.String(),B_ANY_TYPE))
		fData.ReplaceData(superkey.String(),B_ANY_TYPE,data,numBytes);
	else
		fData.AddData(superkey.String(),B_ANY_TYPE,data,numBytes);
} 

void		
MemoryArchive::SetKeyString(ItemProperty key,const char* data){
	size_t len = strlen(data);
	char copy[ len + 1];
	
	strncpy(copy,data,len);
	copy[len] = '\0';		
	SetKey(key,(void*)copy, len+1); 
}

void		
MemoryArchive::SetKeyInt32(ItemProperty key,int32 data){ 
	SetKey(key,(void*)&data,sizeof(int32)); 
}
	
ssize_t	
MemoryArchive::GetData(ItemProperty key,const void** data){
	BString superkey("key_");
	superkey << key;
	ssize_t		numBytes = 0;
	fData.FindData(superkey.String(),B_ANY_TYPE,0,data,&numBytes);
	
	return numBytes;
}

BString		
MemoryArchive::GetDataString(ItemProperty key){
	
		void* data=NULL;
		ssize_t ret = GetData(key,(const void**)&data);
		
		if(ret>0)
		{
			BString str((const char*)data,ret);
			//if(data) free(data);
			return str;
		}
		else
			return NULL;
			
		
	}
	
int32		
MemoryArchive::GetDataInt32(ItemProperty key){
	
		void* data = NULL;
		ssize_t ret = GetData(key,(const void**)&data);
		
		if(ret == sizeof(int32) )
		{
			int32 num = *((int32*)data);
			//if(data) free(data);
			return num;
		}
		else
			return 0;
			
		
	}
	
int32
MemoryArchive::CountData(){
	return fData.CountNames(B_ANY_TYPE);
}

void 
MemoryArchive::KeyModified(int32 key, void* data, ssize_t numBytes){
	SetKey((ItemProperty)key,data,numBytes);
}

void		
MemoryArchive::FillListener(Notifier* fListener){
#ifdef ZETA
  //now update all my info! (is always true?)
  void *cookie = NULL;
  char *name; 
  uint32 type=0; 
  int32 count = 0;
  
  while (fData.GetNextName(&cookie, (const char**)&name,NULL,NULL) == B_OK) {

  	void*	data = NULL;
  	ssize_t	numBytes;
  	if(fData.FindData(name,B_ANY_TYPE,0,(const void**)&data,&numBytes)==B_OK)
  	{
  		//FIX MANCA SSIZE_T!  (unmangling the name..)
  		BString uname(name);
  		uname.RemoveAll("key_");
  		int32 key = atol(uname.String());
  			
  		fListener->SetKey((ItemProperty)key,data,numBytes);
  		//if(data && numBytes>0)free(data);
  	}
  }
#else
  //now update all my info! (is always true?)
 //void *cookie = NULL;
  char *name; 
  uint32 type=0; 
  int32 count = fData.CountNames(B_ANY_TYPE);
  
  for(int i=0;i<count;i++)
  //while (fData.GetNextName(&cookie, (const char**)&name,NULL,NULL) == B_OK)
   {
	fData.GetInfo(B_ANY_TYPE,i,(char**)&name,&type);
  	void*	data = NULL;
  	ssize_t	numBytes;
  	if(fData.FindData(name,B_ANY_TYPE,0,(const void**)&data,&numBytes)==B_OK)
  	{
  		//FIX MANCA SSIZE_T!  (unmangling the name..)
  		BString uname(name);
  		uname.RemoveAll("key_");
  		int32 key = atol(uname.String());
  			
  		fListener->SetKey((ItemProperty)key,data,numBytes);
  		//if(data && numBytes>0)free(data);
  	}
  }
#endif
}

void
MemoryArchive::MakeEmpty(){
	fData.MakeEmpty();
}
//--

