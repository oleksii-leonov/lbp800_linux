#ifndef NEWDIALOG_H
#define NEWDIALOG_H

//(*Headers(NewDialog)
#include <wx/dialog.h>
//*)

class NewDialog: public wxDialog
{
	public:

		NewDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~NewDialog();

		//(*Identifiers(NewDialog)
		//*)

	protected:

		//(*Handlers(NewDialog)
		//*)

		//(*Declarations(NewDialog)
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
