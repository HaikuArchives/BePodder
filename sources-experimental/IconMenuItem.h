#ifndef ICON_MENU_ITEM_H
#define ICON_MENU_ITEM_H

//------------------------------------------------------------------------------
// I N C L U D E S
//------------------------------------------------------------------------------

#include <be/interface/MenuItem.h>
#include <be/interface/Rect.h>
#include <be/interface/Bitmap.h>
#include <String.h>

//------------------------------------------------------------------------------
// D E C L A R A T I O N S
//------------------------------------------------------------------------------

class BBitmap;
class BRect;

class IconMenuItem : public BMenuItem {
	public:
					IconMenuItem(BBitmap* icon, const char *label,
						const char *extra, BMessage *msg = NULL, bool ownIcon = true);
						
	    virtual 	~IconMenuItem();

		const char	*Extra(void) { return fExtra.String(); };

	protected:
	    virtual void GetContentSize(float* width, float* height);
	    virtual void DrawContent();

	private:
		void		_CalculateOffsets(void);
		
		BBitmap		*fIcon;
		BString		fExtra;
		BString 	fLabel;
		BRect		fBounds;

		float		fFontHeightTotal;
		font_height	fFontHeight;
		
		bool		fOwnIcon;
		bool		fCreatedIcon;
};

#endif
