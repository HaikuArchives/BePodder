#include "ImageCache.h"

#include <AppDefs.h>
#include <Bitmap.h>
#include <Debug.h>
#include <TranslationUtils.h>

// ---------------------------------------------------------------------------
// Class Data Initialization

ImageCache *
ImageCache::m_instance = NULL;

// ---------------------------------------------------------------------------
// Constructor/Destructor

ImageCache::ImageCache() {

}

ImageCache::~ImageCache()
{
	while(m_bitmaps.CountItems()) {
		BBitmap* bit=m_bitmaps.ValueFor(0);
		delete bit;
	}
}

// ---------------------------------------------------------------------------
// Operations

BBitmap *
ImageCache::GetImage(
	BString which, BString name)
{
	if (m_instance == NULL)
		m_instance = new ImageCache();

	/* cerca.. se non c'e' lo carica altrimenti usa quello */
	bool found;
	BBitmap *bitmap=m_instance->m_bitmaps.ValueFor(name,&found);
	

	if(!found){
		bitmap = LoadImage(which.String(),name.String());
		if(bitmap) 	m_instance->m_bitmaps.AddItem(name,bitmap);
		return bitmap;
	}
	else  {
		return bitmap;
	}
	return NULL;
}


void
ImageCache::AddImage(BString name,BBitmap* which){

	if (m_instance == NULL)
		m_instance = new ImageCache();
		
	m_instance->m_bitmaps.AddItem(name,which);		
}

void
ImageCache::DeleteImage(BString name){

	if (m_instance == NULL)
		m_instance = new ImageCache();
		
	BBitmap* bitmap = m_instance->m_bitmaps.ValueFor(name);	
	if(bitmap){
		m_instance->m_bitmaps.RemoveItemFor(name);
		delete bitmap;	
	}
}

void
ImageCache::Release()
{
	if (m_instance != NULL){
		delete m_instance;
		m_instance = NULL;
	}
}

BBitmap *
ImageCache::LoadImage(
	const char *fullName, const char* shortName) {
	BBitmap *bitmap = BTranslationUtils::GetBitmap(fullName);
	if(!bitmap){
		bitmap = BTranslationUtils::GetBitmap('PNG ', shortName);
	}	
	
	if(!bitmap)
		printf("ImageCache: Can't load bitmap! %s\n",fullName);
	return bitmap;
}

// END - ImageCache.cpp
