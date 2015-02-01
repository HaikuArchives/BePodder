#ifndef BitmapView_H_
#define BitmapView_H_


class BitmapView: public BView{
	public:
			BitmapView(BRect r,BBitmap* map):BView(r,"BitmapView",B_FOLLOW_NONE,B_WILL_DRAW){fMap=map;};
			~BitmapView(){
				if(fMap)
					delete fMap;
			}
			void Draw(BRect /*r*/){
				if(fMap)
					DrawBitmapAsync(fMap,Bounds()); //BPoint(0,0));
			}
			void AttachedToWindow(){
				SetViewColor(B_TRANSPARENT_COLOR);
			}
			
			void	SetBitmap(BBitmap* bitmap){
				fMap = bitmap;
			}
			void MouseUp(BPoint p){
					if(Parent()) Parent()->MakeFocus(true);
			}
	private:
			BBitmap*	fMap;
};

#endif
//---
