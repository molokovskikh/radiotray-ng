// Copyright 2017 Michael A. Burns <michael.burns.oss@gmail.com>
//
// This file is part of Radiotray-NG.
//
// Radiotray-NG is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Radiotray-NG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Radiotray-NG.  If not, see <http://www.gnu.org/licenses/>.

#ifdef WX_PRECOMP
	#include "wx_pch.hpp"
#else
	#include <wx/filedlg.h>
	#include <wx/filename.h>

	#include <radiotray-ng/helpers.hpp>
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "station_editor_dialog.hpp"

#include "images/blank.xpm"


//IMPLEMENT_DYNAMIC_CLASS(StationEditorDialog, EditorDialogBase)

namespace
{
	const wxWindowID STATION_DIALOG_ID = 301;
	const wxString STATION_DIALOG_TITLE = wxT("Редактор станций");

	const wxWindowID NAME_ID = 302;
	const wxWindowID URL_ID = 303;
	const wxWindowID BITMAP_ID = 304;
	const wxWindowID IMAGE_ID = 305;
	const wxWindowID NOTIFY_ID = 306;
}

BEGIN_EVENT_TABLE(StationEditorDialog, EditorDialogBase)
END_EVENT_TABLE()


StationEditorDialog::StationEditorDialog() : EditorDialogBase(),
	name_control(nullptr),
	url_control(nullptr),
	bitmap_control(nullptr),
	image_control(nullptr)
{
}

StationEditorDialog::StationEditorDialog(wxWindow* parent) : StationEditorDialog()
{
	this->create(parent, STATION_DIALOG_ID, STATION_DIALOG_TITLE);
}

StationEditorDialog::~StationEditorDialog()
{
	if (this->image_control)
	{
		delete this->image_control;
	}

	if (this->bitmap_control)
	{
		delete this->bitmap_control;
	}

	if (this->url_control)
	{
		delete this->url_control;
	}

	if (this->name_control)
	{
		delete this->name_control;
	}

	if (this->notify_control)
	{
		delete this->notify_control;
	}
}

bool
StationEditorDialog::createControls()
{
	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(2, 5, 5);

	// name
	grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Название")), 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
	this->name_control = new wxTextCtrl(this, NAME_ID, "", wxDefaultPosition, wxSize(140, -1));
	grid_sizer->Add(this->name_control, 0, wxALIGN_LEFT | wxEXPAND);

	// url
	grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("URL")), 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
	this->url_control = new wxTextCtrl(this, URL_ID, "", wxDefaultPosition, wxSize(270, -1));
	grid_sizer->Add(this->url_control, 0, wxALIGN_LEFT | wxEXPAND);

	// image
	grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Картинка")), 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

	wxFlexGridSizer* image_sizer = new wxFlexGridSizer(3, 5, 5);

	wxImage image = wxImage(blank_xpm).Scale(24, 24);
	this->bitmap_control = new wxStaticBitmap(this, BITMAP_ID, wxBitmap(image));
	image_sizer->Add(this->bitmap_control, 0, wxALIGN_LEFT);
	this->image_control = new wxTextCtrl(this, IMAGE_ID, "", wxDefaultPosition, wxSize(270, -1));
	image_sizer->Add(this->image_control, 0, wxALIGN_LEFT | wxEXPAND);

	this->addImageButton(image_sizer);

	grid_sizer->Add(image_sizer, 0, wxALL);

	// notify
	grid_sizer->AddStretchSpacer();
	this->notify_control = new wxCheckBox(this, NOTIFY_ID, wxT("Уведомление"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	grid_sizer->Add(this->notify_control, 0, wxALIGN_RIGHT);

	// finish
	main_sizer->Add(grid_sizer, 0, wxALL, 10);

	this->finishDialog(main_sizer);

	this->name_control->SetFocus();

	return true;
}

void
StationEditorDialog::setData(const std::string& name, const std::string& url, const std::string& image, const bool notifications)
{
	wxString tmpstr(name.c_str(), wxConvUTF8);
	this->name_control->SetValue(tmpstr);
	this->url_control->SetValue(wxString(url));
	this->image_control->SetValue(wxString(image));
	this->notify_control->SetValue(notifications);
    
	/// @todo set value in notify control to notifications
	if(notifications) {}

	if (!image.empty())
	{
		/// @todo The following is a "temporary" workaround for the
		///       issue identified in 15331. This can be removed once
		///       wxWidgets 3.1 is available.
		///
		///       http://trac.wxwidgets.org/ticket/15331
		wxLogNull log_null;

		wxImage img = wxImage(radiotray_ng::word_expand(image)).Scale(24, 24);
		this->bitmap_control->SetBitmap(wxBitmap(img));
	}
}

void
StationEditorDialog::getData(std::string& name, std::string& url, std::string& image, bool& notifications)
{
	wxString tmpstr = this->name_control->GetValue();
	name = std::string(tmpstr.mb_str(wxConvUTF8));
	name = radiotray_ng::trim(name);
	url = this->url_control->GetValue();
	image = this->image_control->GetValue();
	notifications = this->notify_control->GetValue();
}

std::string
StationEditorDialog::getImagePath()
{
	return std::string(this->image_control->GetValue().c_str());
}

bool
StationEditorDialog::setImage(std::string* path)
{
    LOG(debug) << "путь к файлу -- " << path->c_str();   
    
	this->image_control->SetValue(path->c_str());

	/// @todo The following is a "temporary" workaround for the
	///       issue identified in 15331. This can be removed once
	///       wxWidgets 3.1 is available.
	///
	///       http://trac.wxwidgets.org/ticket/15331
	wxLogNull log_null;

	wxImage img = wxImage(path->c_str()).Scale(24, 24);
	this->bitmap_control->SetBitmap(wxBitmap(img));

	return true;
}
