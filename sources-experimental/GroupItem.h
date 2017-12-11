#ifndef _GroupItem_H_
#define _GroupItem_H_

#include <ColumnListView.h>
#include <ColumnTypes.h>


class GroupItem : public BRow {

	public:
		GroupItem(const char* label, GroupItem* parentGroup);		
		
		BString GroupName();
		void		SetGroupName(BString);
		
		GroupItem*	GetParentItem();
		
	private:
	
		GroupItem*	f_Parent;
};

#endif
