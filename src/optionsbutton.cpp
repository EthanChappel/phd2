/*
 *  optionsbutton.cpp
 *  PHD Guiding
 *
 *  Created by Andy Galasso
 *  Copyright (c) 2013 Andy Galasso
 *  All rights reserved.
 *
 *  This source code is distributed under the following "BSD" license
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *    Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *    Neither the name of Craig Stark, Stark Labs,
 *     Bret McKee, Dad Dog Development, Ltd, nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <wx/dc.h>
#include <wx/renderer.h>

#include "phd.h"
#include "optionsbutton.h"

#include "icons/down_arrow.xpm"

// clang-format off
wxBEGIN_EVENT_TABLE(OptionsButton, wxPanel)
    EVT_ENTER_WINDOW(OptionsButton::OnMouseEnter)
    EVT_MOTION(OptionsButton::OnMouseMove)
    EVT_LEAVE_WINDOW(OptionsButton::OnMouseLeave)
    EVT_PAINT(OptionsButton::OnPaint)
	EVT_SYS_COLOUR_CHANGED(OptionsButton::OnSysColourChanged)
    EVT_LEFT_UP(OptionsButton::OnClick)
wxEND_EVENT_TABLE();
// clang-format on

enum
{
    PADX = 5,
    PADY = 5,
};

OptionsButton::OptionsButton(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size,
                             long style, const wxString& name)
    : wxPanel(parent, id, pos, size, style, name), m_highlighted(false), m_pressed(false), m_label(label)
{
    DrawDownArrow();

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

OptionsButton::~OptionsButton()
{
    delete m_bmp;
}

wxSize OptionsButton::GetMinSize() const
{
    wxSize txtsz = GetTextExtent(m_label);
    return wxSize(PADX + txtsz.x + PADX + m_bmp->GetWidth() + PADX, PADY + txtsz.y + PADY);
}

void OptionsButton::SetLabel(const wxString& label)
{
    m_label = label;
    Refresh();
}

bool OptionsButton::Enable(bool enable)
{
    if (!wxPanel::Enable(enable))
    {
        return false;
    }
    DrawDownArrow();
    Refresh();
    return true;
}

void OptionsButton::OnPaint(wxPaintEvent& evt)
{
    wxBufferedPaintDC dc(this);
    wxSize txtsz = GetTextExtent(m_label);
    wxSize size(GetSize());
    int xbmp = size.GetWidth() - m_bmp->GetWidth() - PADX;
    wxPoint bmp_pos(xbmp, 7);
    int xtxt;

    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();

    if (GetWindowStyleFlag() & wxALIGN_CENTER_HORIZONTAL)
    {
        xtxt = (size.GetWidth() - txtsz.GetWidth()) / 2;
    }
    else
    {
        xtxt = PADX;
    }

    int flags = 0;
    if (m_highlighted)
    {
		flags |= wxCONTROL_CURRENT;
	}
	if (m_pressed)
    {
        flags |= wxCONTROL_PRESSED;
    }
	if (!IsEnabled())
    {
        flags |= wxCONTROL_DISABLED;
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    }

    wxRendererNative::Get().DrawPushButton(this, dc, wxRect(size), flags);

	dc.DrawText(m_label, xtxt, PADY);
    dc.DrawBitmap(*m_bmp, bmp_pos, true);
}

void OptionsButton::OnMouseEnter(wxMouseEvent& event)
{
    m_highlighted = true;
    Refresh();
}

void OptionsButton::OnMouseMove(wxMouseEvent& event)
{
    if (!m_highlighted)
    {
        m_highlighted = true;
        Refresh();
    }
}

void OptionsButton::OnMouseLeave(wxMouseEvent& event)
{
    m_highlighted = false;
    Refresh();
}

void OptionsButton::OnClick(wxMouseEvent& event)
{
    wxCommandEvent cmd(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
#ifdef __WXGTK__ // Process the event as in wxgtk_button_clicked_callback()
    HandleWindowEvent(cmd);
#else
    ::wxPostEvent(GetParent(), cmd);
#endif
    m_highlighted = false;
    Refresh();
}

void OptionsButton::OnLeftDown(wxMouseEvent& event)
{
    m_pressed = true;
    Refresh();
}

void OptionsButton::OnLeftUp(wxMouseEvent& event)
{
    m_pressed = false;
    Refresh();
}

void OptionsButton::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    DrawDownArrow();
    Refresh();
    event.Skip();
}

void OptionsButton::DrawDownArrow()
{
    wxImage *tmp = new wxImage(down_arrow);

    if (tmp->HasMask()) {
        tmp->InitAlpha();
    }

    wxColour fg = IsEnabled() ? GetForegroundColour() : wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

    tmp->Replace(255, 255, 255, fg.GetRed(), fg.GetGreen(), fg.GetBlue());

    m_bmp = new wxBitmap(*tmp);
}
