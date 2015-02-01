	#include "StackedView.h"

StackedView::StackedView(BRect _frame,const char* _name, 
							                            uint32 _resize, 
							                            uint32 _flags):
						BView(_frame,_name,_resize,_flags){
	fSelected = NULL;
}

uint32				
StackedView::AddView(BView* _view){
	fStackedViewList.AddItem(_view);
	
	_view->MoveTo(0,0);
	_view->ResizeTo(Bounds().Width(),Bounds().Height());
	
	AddChild(_view);
	if(fSelected!=NULL)		_view->Hide();
	else fSelected = _view;
	return 0;
}

BView*
StackedView::RemoveView(BView& _view){
	fStackedViewList.RemoveItem(&_view);	
	return NULL;	
}

uint32		 		
StackedView::CountViews() const {
	return fStackedViewList.CountItems();
}

void				
StackedView::SetSelectedIndex(uint32 _index){
	BView* at=fStackedViewList.ItemAt(_index);
	if(fSelected) fSelected->Hide();
	if(at) at->Show();
	fSelected = at;
	
	
}

void		
StackedView::AttachedToWindow(){
	
	if(Parent())
	SetViewColor(Parent()->ViewColor());
	for(int i=0;i<CountViews();i++){
		BView* at=fStackedViewList.ItemAt(i);
		at->MoveTo(0,0);
		at->ResizeTo(Bounds().Width(),Bounds().Height());
	}
		
}

void
StackedView::SetSelectedView(BView& _view){
	int32 index = fStackedViewList.IndexOf(&_view);
	if(index >=0)
		SetSelectedIndex(index);
}
				
