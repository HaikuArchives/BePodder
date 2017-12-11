#include "GroupItem.h"
#include "SubscriptionColumn.h"


GroupItem::GroupItem(const char* label, GroupItem* parentGroup):BRow() {
	SetField( new PositionableField(label), 0 );
	f_Parent = parentGroup;
}

BString
GroupItem::GroupName(){
	return ((PositionableField*)GetField(0))->String();
}

void		
GroupItem::SetGroupName(BString name){
	((PositionableField*)GetField(0))->SetString(name.String());
}

GroupItem*		
GroupItem::GetParentItem(){
	return f_Parent;
}
