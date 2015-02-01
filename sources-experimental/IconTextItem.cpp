#include "IconTextItem.h"

const float kEdgeOffset = 2.0;
const rgb_color kHighlight = {140, 140, 140, 255};

IconTextItem::IconTextItem(const char *text, BBitmap *icon) 
	: fIcon(NULL),
	fIconHeight(0),
	fIconWidth(0) {
	fText = text;
	fIcon = icon;
};

IconTextItem::~IconTextItem(void) {
	delete fIcon;
};

void IconTextItem::DrawItem(BView *owner, BRect frame, bool complete) {
	if (IsSelected() || complete) {
		rgb_color color;
		rgb_color origHigh;
		
		origHigh = owner->HighColor();
		
		if (IsSelected()) {
			color = kHighlight;
		} else {
			color = owner->ViewColor();
		};
		
		owner->SetHighColor(color);
		BRect fr(frame);
		owner->FillRect(fr);
		owner->SetHighColor(origHigh);
	}
	
	if (fIcon) {
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(fIcon, BPoint(frame.left + kEdgeOffset,frame.bottom - kEdgeOffset - fIconHeight) + fSpiaz);
		owner->SetDrawingMode(B_OP_COPY);
	};
	
	if (IsSelected()) owner->SetDrawingMode(B_OP_OVER);

	owner->MovePenTo(frame.left + kEdgeOffset + fIconWidth + kEdgeOffset + 5,	frame.bottom - fFontOffset);
	owner->DrawString(fText.String());
};

const char *IconTextItem::Text(void) const {
	return fText.String();
};

const BBitmap *IconTextItem::Icon(void) const {
	return fIcon;
};

void IconTextItem::Update(BView */*owner*/, const BFont *font) {
	font_height fontHeight;
	font->GetHeight(&fontHeight);
	fFontHeight = fontHeight.descent + fontHeight.leading + fontHeight.ascent;
	
	if (fIcon) {
		fIconHeight = fIcon->Bounds().Height() + 1;
		fIconWidth = fIcon->Bounds().Width();

		if (fIconHeight > fFontHeight) {
			SetHeight(fIconHeight + (kEdgeOffset * 2));
			fFontOffset = ((fIconHeight - fFontHeight) / 2) + (kEdgeOffset * 2);
		} else {
			SetHeight(fFontHeight + (kEdgeOffset * 2));
			fFontOffset = kEdgeOffset;
		};
	} else {
		SetHeight(fFontHeight + (kEdgeOffset * 2));
		fFontOffset = kEdgeOffset;
	};	
};
