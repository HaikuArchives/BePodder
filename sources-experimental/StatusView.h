/*
 * Copyright 2017. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Janus
 *
 * Based on StyledEdit and ShowImage StatusView
 *
 */
#ifndef STATUS_VIEW_H
#define STATUS_VIEW_H

#include <String.h>
#include <View.h>

enum {
	kItemsNumberCell,
	kItemsRenameCell,
	kItemsDuplicateItemCell,
	kStatusCellCount
};


class StatusView : public BView {
public:
						StatusView(BScrollBar* scrollBar);

	virtual	void		AttachedToWindow();
	virtual	void		Draw(BRect updateRect);
	virtual void		GetPreferredSize(float* _width, float* _height);
	virtual	void		MouseDown(BPoint where);
	virtual	void		ResizeToPreferred();

			void		Update(
							const BString& text, const BString& pages,
							const BString& imageType);
private:
			void		_SetItemsNumberText(const BString& text);
			void		_SetDuplicatesNumberText(const BString& imageType);
			void		_SetRenamesNumberText(const BString& pages);
			void		_ValidatePreferredSize();

		BScrollBar*		fScrollBar;
			BSize		fPreferredSize;
			BString		fCellText[kStatusCellCount];
			float		fCellWidth[kStatusCellCount];
};

#endif	// STATUS_VIEW_H
