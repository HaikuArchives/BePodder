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


#include <ControlLook.h>
#include <Entry.h>
#include <Path.h>
#include <ScrollView.h>

#include "StatusView.h"

const float kHorzSpacing = 5.f;


StatusView::StatusView(BScrollBar* scrollBar)
	:
	BView(BRect(), "statusview", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT, B_WILL_DRAW),
		fScrollBar(scrollBar),
	fPreferredSize(0.0, 0.0)
{
	memset(fCellWidth, 0, sizeof(fCellWidth));
}


void
StatusView::AttachedToWindow()
{
	SetFont(be_plain_font);
	SetFontSize(10.0);

	AdoptParentColors();

	ResizeToPreferred();
}


void
StatusView::Draw(BRect updateRect)
{
	if (fPreferredSize.width <= 0)
		return;

	if (be_control_look != NULL) {
		BRect bounds(Bounds());
		bounds.left++;
		bounds.top++;
		bounds.bottom--;
		be_control_look->DrawMenuBarBackground(this,
			bounds, updateRect,	ui_color(B_PANEL_BACKGROUND_COLOR));
	}

	BRect bounds(Bounds());
	rgb_color highColor = ui_color(B_PANEL_TEXT_COLOR);

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_2_TINT));
	StrokeLine(bounds.LeftTop(), bounds.RightTop());
	StrokeLine(bounds.LeftBottom(), bounds.RightBottom());
	StrokeLine(bounds.LeftTop(), bounds.LeftBottom());

	float x = bounds.left;
	for (size_t i = 0; i < kStatusCellCount - 1; i++) {
		x += fCellWidth[i];
		if (fCellWidth[i + 1] > 0)
			StrokeLine(BPoint(x, bounds.top + 3), BPoint(x, bounds.bottom - 3));
	}

	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(highColor);

	font_height fontHeight;
	GetFontHeight(&fontHeight);

	x = bounds.left;
	float y = (bounds.bottom + bounds.top
		+ ceilf(fontHeight.ascent) - ceilf(fontHeight.descent)) / 2;

	for (size_t i = 0; i < kStatusCellCount; i++) {
		if (fCellText[i].Length() == 0)
			continue;
		DrawString(fCellText[i], BPoint(x + kHorzSpacing, y));
		x += fCellWidth[i];
	}
}


void
StatusView::GetPreferredSize(float* _width, float* _height)
{
	_ValidatePreferredSize();

	if (_width)
		*_width = fPreferredSize.width;

	if (_height)
		*_height = fPreferredSize.height;
}


void
StatusView::ResizeToPreferred()
{
	float width, height;
	GetPreferredSize(&width, &height);

	if (Bounds().Width() > width)
		width = Bounds().Width();

	BView::ResizeTo(width, height);
}


void
StatusView::MouseDown(BPoint where)
{
}


void
StatusView::Update(const BString& text,	const BString& pages,
	const BString& imageType)
{
	_SetItemsNumberText(text);
	_SetRenamesNumberText(pages);
	_SetDuplicatesNumberText(imageType);

	_ValidatePreferredSize();
	Invalidate();
}


#pragma mark -- Private Methods --


void
StatusView::_SetItemsNumberText(const BString& numItems)
{
	fCellText[kItemsNumberCell] = numItems;
}


void
StatusView::_SetDuplicatesNumberText(const BString& numItems)
{
	fCellText[kItemsDuplicateItemCell] = numItems;
}


void
StatusView::_SetRenamesNumberText(const BString& numItems)
{
	fCellText[kItemsRenameCell] = numItems;
}


void
StatusView::_ValidatePreferredSize()
{
	float orgWidth = fPreferredSize.width;
	// width
	fPreferredSize.width = 0.f;
	for (size_t i = 0; i < kStatusCellCount; i++) {
		if (fCellText[i].Length() == 0) {
			fCellWidth[i] = 0;
			continue;
		}
		float width = ceilf(StringWidth(fCellText[i]));
		if (width > 0)
			width += kHorzSpacing * 2;
		fCellWidth[i] = width;
		fPreferredSize.width += fCellWidth[i];
	}

	// height
	font_height fontHeight;
	GetFontHeight(&fontHeight);

	fPreferredSize.height = ceilf(fontHeight.ascent + fontHeight.descent
		+ fontHeight.leading);

	if (fPreferredSize.height < B_H_SCROLL_BAR_HEIGHT)
		fPreferredSize.height = B_H_SCROLL_BAR_HEIGHT;

	float delta = fPreferredSize.width - orgWidth;
	ResizeBy(delta, 0);
	fScrollBar->ResizeBy(-delta, 0);
	fScrollBar->MoveBy(delta, 0);
}
