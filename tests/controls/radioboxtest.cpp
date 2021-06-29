///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/radioboxtest.cpp
// Purpose:     wxRadioBox unit test
// Author:      Steven Lamerton
// Created:     2010-07-14
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "doctest.h"

#include "testprec.h"

#if wxUSE_RADIOBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/radiobox.h"
#endif // WX_PRECOMP

#include "wx/tooltip.h"

class RadioBoxTestCase : public CppUnit::TestCase
{
public:
    RadioBoxTestCase() { }

    void setUp() override;
    void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( RadioBoxTestCase );
        CPPUNIT_TEST( FindString );
        CPPUNIT_TEST( RowColCount );
        CPPUNIT_TEST( Enable );
        CPPUNIT_TEST( Show );
        CPPUNIT_TEST( HelpText );
        CPPUNIT_TEST( ToolTip );
        CPPUNIT_TEST( Selection );
        CPPUNIT_TEST( Count );
        CPPUNIT_TEST( SetString );
    CPPUNIT_TEST_SUITE_END();

    void FindString();
    void RowColCount();
    void Enable();
    void Show();
    void HelpText();
    void ToolTip();
    void Selection();
    void Count();
    void SetString();

    wxRadioBox* m_radio;

    RadioBoxTestCase(const RadioBoxTestCase&) = delete;
	RadioBoxTestCase& operator=(const RadioBoxTestCase&) = delete;
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RadioBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RadioBoxTestCase, "RadioBoxTestCase" );

void RadioBoxTestCase::setUp()
{
    std::vector<wxString> choices;
    choices.push_back("item 0");
    choices.push_back("item 1");
    choices.push_back("item 2");

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices);
}

void RadioBoxTestCase::tearDown()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

void RadioBoxTestCase::FindString()
{
    CHECK_EQ(wxNOT_FOUND, m_radio->FindString("not here"));
    CHECK_EQ(1, m_radio->FindString("item 1"));
    CHECK_EQ(2, m_radio->FindString("ITEM 2"));
    CHECK_EQ(wxNOT_FOUND, m_radio->FindString("ITEM 2", true));
}

void RadioBoxTestCase::RowColCount()
{
#ifndef __WXGTK__
    std::vector<wxString> choices;
    choices.push_back("item 0");
    choices.push_back("item 1");
    choices.push_back("item 2");

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices, 2);

    CHECK_EQ(2, m_radio->GetColumnCount());
    CHECK_EQ(2, m_radio->GetRowCount());

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices, 1,
                             wxRA_SPECIFY_ROWS);

    CHECK_EQ(3, m_radio->GetColumnCount());
    CHECK_EQ(1, m_radio->GetRowCount());
#endif
}

void RadioBoxTestCase::Enable()
{
#ifndef __WXOSX__
    m_radio->Enable(false);

    CHECK(!m_radio->IsItemEnabled(0));

    m_radio->Enable(1, true);

    CHECK(!m_radio->IsItemEnabled(0));
    CHECK(m_radio->IsItemEnabled(1));
    CHECK(!m_radio->IsItemEnabled(2));

    m_radio->Enable(true);

    CHECK(m_radio->IsItemEnabled(0));
    CHECK(m_radio->IsItemEnabled(1));
    CHECK(m_radio->IsItemEnabled(2));

    m_radio->Enable(0, false);

    CHECK(!m_radio->IsItemEnabled(0));
    CHECK(m_radio->IsItemEnabled(1));
    CHECK(m_radio->IsItemEnabled(2));
#endif
}

void RadioBoxTestCase::Show()
{
    m_radio->Show(false);

    CHECK(!m_radio->IsItemShown(0));

    m_radio->Show(1, true);

    CHECK(!m_radio->IsItemShown(0));
    CHECK(m_radio->IsItemShown(1));
    CHECK(!m_radio->IsItemShown(2));

    m_radio->Show(true);

    CHECK(m_radio->IsItemShown(0));
    CHECK(m_radio->IsItemShown(1));
    CHECK(m_radio->IsItemShown(2));

    m_radio->Show(0, false);

    CHECK(!m_radio->IsItemShown(0));
    CHECK(m_radio->IsItemShown(1));
    CHECK(m_radio->IsItemShown(2));
}

void RadioBoxTestCase::HelpText()
{
    CHECK_EQ(wxEmptyString, m_radio->GetItemHelpText(0));

    m_radio->SetItemHelpText(1, "Item 1 help");

    CHECK_EQ("Item 1 help", m_radio->GetItemHelpText(1));

    m_radio->SetItemHelpText(1, "");

    CHECK_EQ(wxEmptyString, m_radio->GetItemHelpText(1));
}

void RadioBoxTestCase::ToolTip()
{
#if defined (__WXMSW__) || defined(__WXGTK__)
    //GetItemToolTip returns null if there is no tooltip set
    CHECK(!m_radio->GetItemToolTip(0));

    m_radio->SetItemToolTip(1, "Item 1 help");

    CHECK_EQ("Item 1 help", m_radio->GetItemToolTip(1)->GetTip());

    m_radio->SetItemToolTip(1, "");

    //However if we set a blank tip this does count as a tooltip
    CHECK(!m_radio->GetItemToolTip(1));
#endif
}

void RadioBoxTestCase::Selection()
{
    //Until other item containers the first item is selected by default
    CHECK_EQ(0, m_radio->GetSelection());
    CHECK_EQ("item 0", m_radio->GetStringSelection());

    m_radio->SetSelection(1);

    CHECK_EQ(1, m_radio->GetSelection());
    CHECK_EQ("item 1", m_radio->GetStringSelection());

    m_radio->SetStringSelection("item 2");

    CHECK_EQ(2, m_radio->GetSelection());
    CHECK_EQ("item 2", m_radio->GetStringSelection());
}

void RadioBoxTestCase::Count()
{
    //A trivial test for the item count as items can neither
    //be added or removed
    CHECK_EQ(3, m_radio->GetCount());
    CHECK(!m_radio->IsEmpty());
}

void RadioBoxTestCase::SetString()
{
    m_radio->SetString(0, "new item 0");
    m_radio->SetString(2, "");

    CHECK_EQ("new item 0", m_radio->GetString(0));
    CHECK_EQ("", m_radio->GetString(2));
}

#endif // wxUSE_RADIOBOX
