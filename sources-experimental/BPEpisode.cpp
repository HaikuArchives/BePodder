#include "BPEpisode.h"
#include "Utils.h"
#include "AttributeMap.h"


BPEpisode::BPEpisode(entry_ref* ref):BNode(ref){
}

FileStatus			
BPEpisode::GetStatus(){
	return (FileStatus)ReadAttributeInt32(this,ATTR_ITEM_STATUS);
}

void
BPEpisode::SetStatus(FileStatus status){
	WriteAttributeInt32(this,ATTR_ITEM_STATUS,(int32)status);
	NotifyListener(ITEM_ENCLOSURE_FILE_STATUS,(void*)&status,sizeof(status));
}
  						
BString				
BPEpisode::EnclosureLocalPath(){
	return ReadAttributeString(this,ATTR_ENCLOSURE_PATH);
}

BString				
BPEpisode::EnclosureLink(){
	return ReadAttributeString(this,ATTR_ENCLOSURE_URL);
}

BString				
BPEpisode::Guid(){
	return ReadAttributeString(this,ATTR_ITEM_GUID);
}
BString				
BPEpisode::EnclosureType(){
	return ReadAttributeString(this,ATTR_ENCLOSURE_TYPE);	
}
