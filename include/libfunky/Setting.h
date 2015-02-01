#ifndef	Setting_H_
#define	Setting_H_

#include <Message.h>
#include <String.h>

class Setting : public BMessage {

	public:
		
				Setting(const char* name);
					
		void	Store();
		void	Retrive();
		
		BString	SettingPath();
		
	private:
			BString filename;
};

#endif

//..
