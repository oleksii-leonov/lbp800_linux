#include "NewDialog.h"

//(*InternalHeaders(NewDialog)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(NewDialog)
//*)

BEGIN_EVENT_TABLE(NewDialog,wxDialog)
	//(*EventTable(NewDialog)
	//*)
END_EVENT_TABLE()

NewDialog::NewDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(NewDialog)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	//*)
}

NewDialog::~NewDialog()
{
	//(*Destroy(NewDialog)
	//*)
}

