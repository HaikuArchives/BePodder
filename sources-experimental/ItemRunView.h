#ifndef ItemRunView_H_
#define ItemRunView_H_

#include "runview/RunView.h" 
#include "BitmapView.h"
#include <TranslationUtils.h>

#define BORDER_SIZE	5

class ItemRunView : public RunView {

	public:
	
		ItemRunView(BRect r,
						   const char * n,
						   Theme * t,
						   uint32 a,
							uint32 b)
						   :RunView(r,n,t,a,b){
							fImage=NULL;
							AddChild(viewer = new BitmapView(BRect(1,1,2,2),NULL));
							LoadBitmap(NULL);
						}
						
		void		TextResized(BRect r)
					{
							//printf("Bottom line %f  (value %f)\n",r.bottom,r.top);
							if(fImage)
							 viewer->MoveTo(BORDER_SIZE,r.bottom - r.top + BORDER_SIZE);
					}
	
		
		void		FrameResized (float w , float h)
					{
							w = w -BORDER_SIZE*2; //BORDER!
							
							if(fImage)
							{
							if(fImage->Bounds().Width() > w)
								viewer->ResizeTo(w, w *proportion );
							else
								viewer->ResizeTo(fImage->Bounds().Width(), fImage->Bounds().Height());
							}
					    	RunView::FrameResized(w+2*BORDER_SIZE,h);
					} 
		
		
		float		ExtraTextSize()
					{ 
					
						if(!viewer->IsHidden()) 
								return viewer->Bounds().Height() + BORDER_SIZE; 
						else 
						return 0;
					}
								
								
		void	LoadBitmap(const char* filename) 
				{
	
				if(fImage) 		
				    delete fImage;	
			
			    fImage = NULL;
			  		
			   if(filename!=NULL)
       			   fImage =  BTranslationUtils::GetBitmap(filename);
       		     
       		    
				viewer->SetBitmap(fImage);
			
				if(fImage)
				 {					
				 	if(viewer->IsHidden()) viewer->Show();
					proportion = fImage->Bounds().Height() / fImage->Bounds().Width(); 
					float w = Bounds().Width() - BORDER_SIZE*2;
					
					if(fImage->Bounds().Width() > w) {
						viewer->ResizeTo(w,  w  * proportion );
					}
						else
						viewer->ResizeTo(fImage->Bounds().Width(), fImage->Bounds().Height());
				 }
				 else 
				 {
					if(!viewer->IsHidden()) viewer->Hide();
				 }
				
				
				}

	private:
		BitmapView* 	viewer;
		BBitmap*		fImage;
		float proportion;
		
};

#endif

