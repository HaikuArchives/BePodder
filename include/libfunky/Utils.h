#ifndef _Utils_H_
#define _Utils_H_

#include <String.h>
#include <Bitmap.h>
#include <PictureButton.h>
#include <Node.h>


BString		GetAppRelativePath();
BString		GetAppFilename();
bool		CheckMIME(BString mime,BNode* node);

BBitmap*		LoadIcon(const char* name);
void			AddIcon(const char* name,BBitmap*);



status_t	 ReadAttributeData(BNode* node,const char *name, char **buffer, int32 *size);

status_t	 ReadAttributeString(BNode* node,const char *name, char **buffer, int32 *size); 
BString		 ReadAttributeString(BNode* node,const char *name); 

status_t 	 ReadAttributeInt32(BNode* node,const char *name, int32 *dest);
int32	 	 ReadAttributeInt32(BNode* node,const char *name);

status_t 	 WriteAttributeInt32(BNode* node,const char *name, int32 dest);
status_t 	 WriteAttributeString(BNode* node,const char *name, const char* string);

status_t	WriteAttributeMessage(BNode* node,const char* name,BMessage*);
status_t	ReadAttributeMessage(BNode* node,const char* name,BMessage*);

BString		ReadableSize(off_t	bytes);
BString		ReadableSpeed(float	speed);

void	BPDrawString(const char* string, BView* parent, BRect rect,alignment ali,float kTextMargin = 0.0);


#endif
