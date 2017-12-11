#ifndef _EpisodeListView_H_
#define _EpisodeListView_H_

#include <ColumnListView.h>
#include "FileStatusColumn.h"
#include <StringView.h>

class StatusView;

class EpisodeListView : public BColumnListView  {
	
	public:
			 						EpisodeListView(BRect r);
			
				void				SelectionChanged();
				void				AddRow(BRow*, BRow *parent = 0);
				void				AddRow(BRow*, int32 index, BRow *parent = 0);
				void				RemoveRow(BRow*);

	private:
				void		UpdateCount();
				StatusView*		fStatusView;
};

#endif

//.
