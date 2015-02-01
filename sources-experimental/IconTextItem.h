#ifndef ICONTEXTITEM_H
#define ICONTEXTITEM_H

#include <Bitmap.h>
#include <ListItem.h>
#include <String.h>
#include <View.h>
#include <OutlineListView.h>


class IconTextItem : public BListItem {
	public:
	
						IconTextItem(const char *text, BBitmap *icon = NULL);
						~IconTextItem(void);
						
		virtual void	DrawItem(BView *owner, BRect frame, bool complete);
		virtual void	Update(BView *owner, const BFont *font);
		const char 		*Text(void) const;
		const BBitmap	*Icon(void) const;
		
		void		SetSpiaz(BPoint p){ fSpiaz = p;}

	private:
		BBitmap			*fIcon;
		BString			fText;
		float			fFontHeight;
		float			fIconHeight;
		float			fIconWidth;
		float			fFontOffset;
		BPoint		fSpiaz;
};

#endif
