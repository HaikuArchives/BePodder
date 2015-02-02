//------------------------------------------------------------------------------
// I N C L U D E S
//------------------------------------------------------------------------------

#include "IconMenuItem.h"

const float kIconTextPadding = 5.0;
const float kEdgePadding = 4.0;
const float kTickSpacing = 4.0;

//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------

IconMenuItem::IconMenuItem(BBitmap* icon, const char *label, const char *extra,
	BMessage *msg, bool ownIcon) :
	BMenuItem(label, msg),
	fIcon(icon),
	fExtra(extra),
	fLabel(label),
	fOwnIcon(ownIcon),
	fCreatedIcon(false) {

	if (fIcon == NULL) {
		fCreatedIcon = true;
		fIcon = new BBitmap(BRect(0, 0, 15, 15), B_RGB_32_BIT, true);
		char *bits = (char *)fIcon->Bits();
		int32 length = fIcon->BitsLength();
		for (int32 i = 0; i < length; i++) bits[i] = B_TRANSPARENT_MAGIC_RGBA32; 
	};

	be_plain_font->GetHeight(&fFontHeight);
	fFontHeightTotal = fFontHeight.ascent + fFontHeight.descent + fFontHeight.leading;

	_CalculateOffsets();
};

IconMenuItem::~IconMenuItem() {
	if ((fOwnIcon == true) || (fCreatedIcon == true)) delete fIcon;
};

void IconMenuItem::_CalculateOffsets(void) {
	fBounds = fIcon->Bounds();
	
	if (fFontHeightTotal > fBounds.Height()) fBounds.bottom = fFontHeightTotal;
	fBounds.bottom += kEdgePadding;

	float width = be_plain_font->StringWidth(fLabel.String());

	fBounds.right += width + kIconTextPadding + kTickSpacing + kEdgePadding;
};

void IconMenuItem::GetContentSize(float* width, float* height) {
//	BMenuItem::GetContentSize(width, height);
//	*width += 20;
//	*height += 3;
	*width = fBounds.Width();
	*height = fBounds.Height();
}

void IconMenuItem::DrawContent() {
	
	BPoint drawPoint(ContentLocation());
	drawPoint.x += 20;
	Menu()->MovePenTo(drawPoint);
	BMenuItem::DrawContent();
	
	BPoint where(ContentLocation());
	where.y = Frame().top;
	
	/*if (IsEnabled())
		Menu()->SetDrawingMode(B_OP_OVER);
	else
		Menu()->SetDrawingMode(B_OP_BLEND);
	*/
	drawing_mode mode=Menu()->DrawingMode();		
	Menu()->SetDrawingMode(B_OP_ALPHA);	
	Menu()->DrawBitmapAsync(fIcon, where);
	Menu()->SetDrawingMode(mode);	
	/*BRect		b = Frame();
	BMenu		*parent = Menu();
	BPoint		loc = parent->PenLocation();
	
	//test code: clean the canvas before using
	//parent->PushState();
	
	//parent->SetLowColor(parent->ViewColor());
	parent->FillRect(b,B_SOLID_LOW);
		
	parent->SetDrawingMode(B_OP_ALPHA);
	
	b.OffsetBy(0, kEdgePadding);
	b.left = loc.x;

	BRect iconPos = b;
	iconPos.right = b.left + fIcon->Bounds().Width();
	iconPos.bottom = iconPos.top + fIcon->Bounds().Height();
	parent->DrawBitmap(fIcon, iconPos);
		
	parent->MovePenTo(loc.x + fIcon->Bounds().Width() + kEdgePadding,
		loc.y + fFontHeight.ascent + kEdgePadding);
	parent->SetDrawingMode( B_OP_OVER );
	//parent->SetLowColor(parent->ViewColor());
	parent->DrawString(fLabel.String());
	//parent->PopState();
	*/
};
