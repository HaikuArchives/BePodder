#ifndef _DownloadListView_H_
#define _DownloadListView_H_

#include <ColumnListView.h>
#include "FileStatusColumn.h"
#include <StringView.h>

#define 	DOWNLOAD_SELECTED			'dwse'
#define 	DOWNLOAD_INVOKED				'dwin'

class DownloadListView : public BColumnListView
{
	public:
			 					DownloadListView(BRect r);
			
				void				SelectionChanged();
				void				AddRow(BRow*, BRow *parent = 0);
				void				AddRow(BRow*, int32 index, BRow *parent = 0);
				
				void				ResetSelectionMessage();

	private:
				void		UpdateCount();
				BStringView*		fCounter;
	
				
};

#endif

//.
