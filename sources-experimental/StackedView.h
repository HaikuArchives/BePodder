#ifndef _StackedView_h_
#define _StackedView_h_

#include "ObjectList.h"
#include <View.h>

class StackedView : public BView
{
	public:

									StackedView(BRect _frame, 
							    	                    const char* _name, 
							                            uint32 _resize, 
							                            uint32 _flags);
		//virtual			 			~StackedView();

				uint32		 		CountViews() const;
				//uint32				IndexOf(const BView& _view) const;
				//bool				HasView(const BView& _view) const;

				//BView&				EditViewAt(uint32 _index);
				//const BView&		ViewAt(uint32 _index) const;

				uint32				AddView(BView* _view);
				//uint32				InsertView(BView* _view, uint32 _index); 

				//BView*				RemoveView(uint32 _index);
				BView*				RemoveView(BView& _view);
		
				//BView*				ReplaceView(uint32 _index, BView* _view);

				void				SetSelectedIndex(uint32 _index);
				void				SetSelectedView(BView& _view);
				
				//uint32				GetSelectedIndex() const;
				BView*				GetSelectedView() { return fSelected; }
				
				//bool				GetAutoResize() const;
				//void				SetAutoResize(bool _autoResize);
				
				//bool				DoesLazyAttach() const;
				//void				SetLazyAttach(bool _lazy);

		virtual void		AttachedToWindow();
		//virtual void 		SelectionChanged(uint32 _index);

	private:
		//		status_t	IndexFor(const BView& _view, uint32& _index) const;
		//		void		CheckBounds(uint32 _index) const;

	private:
		
		BObjectList<BView>	fStackedViewList;
		BView*				fSelected;
};
#endif
//--
