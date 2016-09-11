//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: GDCoreDialogs.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "GDCoreDialogs.h"


// Declare the bitmap loading function
extern void wxC629BInitBitmapResources();

static bool bBitmapLoaded = false;


LayersEditorPanelBase::LayersEditorPanelBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC629BInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    m_auimgr = new wxAuiManager;
    m_auimgr->SetManagedWindow( this );
    m_auimgr->SetFlags( wxAUI_MGR_LIVE_RESIZE|wxAUI_MGR_TRANSPARENT_HINT|wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_ALLOW_ACTIVE_PANE|wxAUI_MGR_ALLOW_FLOATING);
    m_auimgr->GetArtProvider()->SetMetric( wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
    m_auimgr->GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);
    
    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxAUI_TB_PLAIN_BACKGROUND|wxAUI_TB_DEFAULT_STYLE);
    m_toolbar->SetToolBitmapSize(wxSize(16,16));
    
    m_auimgr->AddPane(m_toolbar, wxAuiPaneInfo().Caption(_("Layers editor")).Direction(wxAUI_DOCK_TOP).Layer(0).Row(0).Position(0).Fixed().CaptionVisible(false).MaximizeButton(false).CloseButton(false).MinimizeButton(false).PinButton(false));
    
    m_toolbar->AddTool(ADD_LAYER_TOOL, _("Add a layer"), wxXmlResource::Get()->LoadBitmap(wxT("add16")), wxNullBitmap, wxITEM_NORMAL, _("Add a new layer"), wxT(""), NULL);
    
    m_toolbar->AddTool(DELETE_LAYER_TOOL, _("Delete the selected layer"), wxXmlResource::Get()->LoadBitmap(wxT("delete16")), wxNullBitmap, wxITEM_NORMAL, _("Delete the selected layer"), wxT(""), NULL);
    
    m_toolbar->AddSeparator();
    
    m_toolbar->AddTool(EDIT_LAYER_TOOL, _("Edit the properties of the layer"), wxXmlResource::Get()->LoadBitmap(wxT("properties16")), wxNullBitmap, wxITEM_NORMAL, _("Edit the properties of the layer"), wxT(""), NULL);
    
    m_toolbar->AddTool(LAYER_UP_TOOL, _("Move the layer over"), wxXmlResource::Get()->LoadBitmap(wxT("up16")), wxNullBitmap, wxITEM_NORMAL, _("Move the layer over"), wxT(""), NULL);
    
    m_toolbar->AddTool(LAYER_DOWN_TOOL, _("Move the layer below"), wxXmlResource::Get()->LoadBitmap(wxT("down16")), wxNullBitmap, wxITEM_NORMAL, _("Move the layer below"), wxT(""), NULL);
    
    m_toolbar->AddSeparator();
    
    m_toolbar->AddTool(REFRESH_TOOL, _("Refresh the list"), wxXmlResource::Get()->LoadBitmap(wxT("refreshicon")), wxNullBitmap, wxITEM_NORMAL, _("Refresh the list"), wxT(""), NULL);
    
    m_toolbar->AddSeparator();
    
    m_toolbar->AddTool(HELP_TOOL, _("Help"), wxXmlResource::Get()->LoadBitmap(wxT("help16")), wxNullBitmap, wxITEM_NORMAL, _("Display help about the layers editor"), wxT(""), NULL);
    m_toolbar->Realize();
    
    m_panel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTAB_TRAVERSAL);
    
    m_auimgr->AddPane(m_panel7, wxAuiPaneInfo().Direction(wxAUI_DOCK_CENTER).Layer(0).Row(0).Position(0).BestSize(100,100).MinSize(100,100).MaxSize(100,100).CaptionVisible(false).MaximizeButton(false).CloseButton(false).MinimizeButton(false).PinButton(false));
    m_auimgr->Update();
    
    wxFlexGridSizer* flexGridSizer13 = new wxFlexGridSizer(1, 1, 0, 0);
    flexGridSizer13->SetFlexibleDirection( wxBOTH );
    flexGridSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer13->AddGrowableCol(0);
    flexGridSizer13->AddGrowableRow(0);
    m_panel7->SetSizer(flexGridSizer13);
    
    m_layersList = new wxListCtrl(m_panel7, LAYERS_LIST_ID, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    flexGridSizer13->Add(m_layersList, 0, wxALL|wxEXPAND, 0);
    
    SetName(wxT("LayersEditorPanelBase"));
    SetSize(500,300);
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    // Connect events
    this->Connect(ADD_LAYER_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnAddLayerClicked), NULL, this);
    this->Connect(DELETE_LAYER_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnDeleteLayerClicked), NULL, this);
    this->Connect(EDIT_LAYER_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnEditLayerClicked), NULL, this);
    this->Connect(LAYER_UP_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnLayerUpClicked), NULL, this);
    this->Connect(LAYER_DOWN_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnLayerDownClicked), NULL, this);
    this->Connect(REFRESH_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnRefreshClicked), NULL, this);
    this->Connect(HELP_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnHelpClicked), NULL, this);
    
}

LayersEditorPanelBase::~LayersEditorPanelBase()
{
    this->Disconnect(ADD_LAYER_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnAddLayerClicked), NULL, this);
    this->Disconnect(DELETE_LAYER_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnDeleteLayerClicked), NULL, this);
    this->Disconnect(EDIT_LAYER_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnEditLayerClicked), NULL, this);
    this->Disconnect(LAYER_UP_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnLayerUpClicked), NULL, this);
    this->Disconnect(LAYER_DOWN_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnLayerDownClicked), NULL, this);
    this->Disconnect(REFRESH_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnRefreshClicked), NULL, this);
    this->Disconnect(HELP_TOOL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LayersEditorPanelBase::OnHelpClicked), NULL, this);
    
    m_auimgr->UnInit();
    delete m_auimgr;

}

BaseGroupEventDialog::BaseGroupEventDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC629BInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxFlexGridSizer* flexGridSizer41 = new wxFlexGridSizer(4, 1, 0, 0);
    flexGridSizer41->SetFlexibleDirection( wxBOTH );
    flexGridSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer41->AddGrowableCol(0);
    flexGridSizer41->AddGrowableRow(0);
    this->SetSizer(flexGridSizer41);
    
    wxFlexGridSizer* flexGridSizer43 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer43->SetFlexibleDirection( wxBOTH );
    flexGridSizer43->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer43->AddGrowableCol(1);
    
    flexGridSizer41->Add(flexGridSizer43, 1, wxALL|wxEXPAND, 5);
    
    m_staticText49 = new wxStaticText(this, wxID_ANY, _("Group name:"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer43->Add(m_staticText49, 0, wxALL, 5);
    
    groupNameEdit = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    #if wxVERSION_NUMBER >= 3000
    groupNameEdit->SetHint(wxT(""));
    #endif
    
    flexGridSizer43->Add(groupNameEdit, 0, wxALL|wxEXPAND, 5);
    
    flexGridSizer43->Add(0, 0, 1, wxALL, 5);
    
    backColorBt = new wxButton(this, wxID_ANY, _("Choose background color"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer43->Add(backColorBt, 0, wxALL|wxEXPAND, 5);
    
    flexGridSizer43->Add(0, 0, 1, wxALL, 5);
    
    hideCheck = new wxCheckBox(this, wxID_ANY, _("Hide the events contained in the group"), wxDefaultPosition, wxSize(-1,-1), 0);
    hideCheck->SetValue(false);
    
    flexGridSizer43->Add(hideCheck, 0, wxALL, 5);
    
    m_staticLine47 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxLI_HORIZONTAL);
    
    flexGridSizer41->Add(m_staticLine47, 0, wxALL|wxEXPAND, 5);
    
    wxFlexGridSizer* flexGridSizer45 = new wxFlexGridSizer(0, 4, 0, 0);
    flexGridSizer45->SetFlexibleDirection( wxBOTH );
    flexGridSizer45->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer45->AddGrowableCol(1);
    
    flexGridSizer41->Add(flexGridSizer45, 1, wxALL|wxEXPAND, 5);
    
    m_staticBitmap63 = new wxStaticBitmap(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("help16")), wxDefaultPosition, wxSize(-1,-1), 0 );
    
    flexGridSizer45->Add(m_staticBitmap63, 0, wxALL, 5);
    
    m_hyperLink65 = new wxHyperlinkCtrl(this, wxID_ANY, _("Help"), wxT("http://wiki.compilgames.net/doku.php/en/game_develop/documentation/manual/group_events"), wxDefaultPosition, wxSize(-1,-1), wxHL_DEFAULT_STYLE);
    m_hyperLink65->SetNormalColour(wxColour(wxT("#0000FF")));
    m_hyperLink65->SetHoverColour(wxColour(wxT("#0000FF")));
    m_hyperLink65->SetVisitedColour(wxColour(wxT("#FF0000")));
    
    flexGridSizer45->Add(m_hyperLink65, 0, wxRIGHT, 5);
    
    okBt = new wxButton(this, wxID_ANY, _("Ok"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer45->Add(okBt, 0, wxALL|wxALIGN_RIGHT, 5);
    
    cancelBt = new wxButton(this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer45->Add(cancelBt, 0, wxALL, 5);
    
    SetName(wxT("BaseGroupEventDialog"));
    SetSize(400,200);
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    backColorBt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseGroupEventDialog::onChooseBackgroundBtClick), NULL, this);
    okBt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseGroupEventDialog::onOkBtClick), NULL, this);
    cancelBt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseGroupEventDialog::onCancelBtClick), NULL, this);
    
}

BaseGroupEventDialog::~BaseGroupEventDialog()
{
    backColorBt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseGroupEventDialog::onChooseBackgroundBtClick), NULL, this);
    okBt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseGroupEventDialog::onOkBtClick), NULL, this);
    cancelBt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseGroupEventDialog::onCancelBtClick), NULL, this);
    
}

BaseEventStoreDialog::BaseEventStoreDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC629BInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxFlexGridSizer* flexGridSizer731 = new wxFlexGridSizer(0, 1, 0, 0);
    flexGridSizer731->SetFlexibleDirection( wxBOTH );
    flexGridSizer731->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer731->AddGrowableCol(0);
    flexGridSizer731->AddGrowableRow(0);
    this->SetSizer(flexGridSizer731);
    
    wxBoxSizer* boxSizer134 = new wxBoxSizer(wxHORIZONTAL);
    
    flexGridSizer731->Add(boxSizer134, 1, wxALL|wxEXPAND, 0);
    
    wxFlexGridSizer* flexGridSizer853 = new wxFlexGridSizer(0, 1, 0, 0);
    flexGridSizer853->SetFlexibleDirection( wxBOTH );
    flexGridSizer853->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer853->AddGrowableCol(0);
    flexGridSizer853->AddGrowableRow(0);
    
    boxSizer134->Add(flexGridSizer853, 2, wxALL|wxEXPAND, 0);
    
    templatesList = new wxSimpleHtmlListBox(this, wxID_ANY);
    flexGridSizer853->Add(templatesList, 0, wxALL|wxEXPAND, 5);
    templatesList->SetMinSize(wxSize(200,-1));
    
    searchCtrl = new wxSearchCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    searchCtrl->SetFocus();
    #if wxVERSION_NUMBER >= 3000
    searchCtrl->SetHint(wxT(""));
    #endif
    
    flexGridSizer853->Add(searchCtrl, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
    
    wxBoxSizer* boxSizer145 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer134->Add(boxSizer145, 3, wxALL|wxEXPAND, 0);
    
    nameTxt = new wxStaticText(this, wxID_ANY, _("No template chosen"), wxDefaultPosition, wxSize(-1,-1), 0);
    wxFont nameTxtFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
    nameTxt->SetFont(nameTxtFont);
    
    boxSizer145->Add(nameTxt, 0, wxALL|wxEXPAND, 5);
    nameTxt->SetMinSize(wxSize(350,-1));
    
    authorTxt = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), wxALIGN_RIGHT);
    
    boxSizer145->Add(authorTxt, 0, wxLEFT|wxRIGHT|wxALIGN_RIGHT, 5);
    
    wxBoxSizer* boxSizer146 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer145->Add(boxSizer146, 0, wxALL|wxEXPAND, 0);
    
    descriptionEdit = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), wxTE_READONLY|wxTE_MULTILINE);
    
    boxSizer146->Add(descriptionEdit, 0, wxALL|wxEXPAND, 5);
    descriptionEdit->SetMinSize(wxSize(-1,55));
    
    m_staticLine979 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxLI_HORIZONTAL);
    
    boxSizer145->Add(m_staticLine979, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
    
    parametersScrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxVSCROLL);
    parametersScrolledWindow->SetScrollRate(0, 5);
    
    boxSizer145->Add(parametersScrolledWindow, 1, wxALL|wxEXPAND, 0);
    
    wxGridSizer* gridSizer150 = new wxGridSizer(0, 1, 0, 0);
    parametersScrolledWindow->SetSizer(gridSizer150);
    
    parametersSizer = new wxFlexGridSizer(0, 3, 0, 0);
    gridSizer150->Add(parametersSizer, 1, wxALL|wxEXPAND, 0);
    
    m_staticLine7911 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxLI_HORIZONTAL);
    
    flexGridSizer731->Add(m_staticLine7911, 0, wxALL|wxEXPAND, 0);
    
    wxFlexGridSizer* flexGridSizer7712 = new wxFlexGridSizer(0, 5, 0, 0);
    flexGridSizer7712->SetFlexibleDirection( wxBOTH );
    flexGridSizer7712->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer7712->AddGrowableCol(2);
    
    flexGridSizer731->Add(flexGridSizer7712, 1, wxALL|wxEXPAND, 0);
    
    m_staticBitmap631 = new wxStaticBitmap(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("help16")), wxDefaultPosition, wxSize(-1,-1), 0 );
    
    flexGridSizer7712->Add(m_staticBitmap631, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5);
    
    m_hyperLink652 = new wxHyperlinkCtrl(this, wxID_ANY, _("Help"), wxT("http://wiki.compilgames.net/doku.php/en/game_develop/documentation/manual/event_store"), wxDefaultPosition, wxSize(-1,-1), wxHL_DEFAULT_STYLE);
    m_hyperLink652->SetNormalColour(wxColour(wxT("#0000FF")));
    m_hyperLink652->SetHoverColour(wxColour(wxT("#0000FF")));
    m_hyperLink652->SetVisitedColour(wxColour(wxT("#FF0000")));
    
    flexGridSizer7712->Add(m_hyperLink652, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_hyperLink154 = new wxHyperlinkCtrl(this, wxID_ANY, _("Publish your own events on GDevApp.com"), wxT("https://gdevapp.com"), wxDefaultPosition, wxSize(-1,-1), wxHL_DEFAULT_STYLE);
    m_hyperLink154->SetNormalColour(wxColour(wxT("#0000FF")));
    m_hyperLink154->SetHoverColour(wxColour(wxT("#0000FF")));
    m_hyperLink154->SetVisitedColour(wxColour(wxT("#FF0000")));
    
    flexGridSizer7712->Add(m_hyperLink154, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    okBt = new wxButton(this, wxID_ANY, _("Ok"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer7712->Add(okBt, 0, wxALL|wxALIGN_RIGHT, 5);
    
    cancelBt = new wxButton(this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer7712->Add(cancelBt, 0, wxALL, 5);
    
    SetName(wxT("BaseEventStoreDialog"));
    SetMinClientSize(wxSize(500,300));
    SetSize(750,450);
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    searchCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(BaseEventStoreDialog::OnSearchCtrlText), NULL, this);
    okBt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseEventStoreDialog::OnOkBtClick), NULL, this);
    cancelBt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseEventStoreDialog::OnCancelBtClick), NULL, this);
    
}

BaseEventStoreDialog::~BaseEventStoreDialog()
{
    searchCtrl->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(BaseEventStoreDialog::OnSearchCtrlText), NULL, this);
    okBt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseEventStoreDialog::OnOkBtClick), NULL, this);
    cancelBt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BaseEventStoreDialog::OnCancelBtClick), NULL, this);
    
}

LinkEventEditorBase::LinkEventEditorBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC629BInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxFlexGridSizer* flexGridSizer158 = new wxFlexGridSizer(0, 1, 0, 0);
    flexGridSizer158->SetFlexibleDirection( wxBOTH );
    flexGridSizer158->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer158->AddGrowableCol(0);
    this->SetSizer(flexGridSizer158);
    
    m_staticText160 = new wxStaticText(this, wxID_ANY, _("Link to scene/external events:"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer158->Add(m_staticText160, 0, wxALL|wxEXPAND, 5);
    
    wxArrayString m_eventsComboBoxArr;
    m_eventsComboBox = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), m_eventsComboBoxArr, 0);
    #if wxVERSION_NUMBER >= 3000
    m_eventsComboBox->SetHint(wxT(""));
    #endif
    
    flexGridSizer158->Add(m_eventsComboBox, 0, wxALL|wxEXPAND, 5);
    
    m_includeAllEventsRadio = new wxRadioButton(this, wxID_ANY, _("Include all events"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_includeAllEventsRadio->SetValue(1);
    
    flexGridSizer158->Add(m_includeAllEventsRadio, 0, wxALL, 5);
    
    m_includeEventsGroupRadio = new wxRadioButton(this, wxID_ANY, _("Only include an events group:"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_includeEventsGroupRadio->SetValue(0);
    
    flexGridSizer158->Add(m_includeEventsGroupRadio, 0, wxALL, 5);
    
    wxArrayString m_eventsGroupComboBoxArr;
    m_eventsGroupComboBox = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), m_eventsGroupComboBoxArr, 0);
    #if wxVERSION_NUMBER >= 3000
    m_eventsGroupComboBox->SetHint(wxT(""));
    #endif
    
    flexGridSizer158->Add(m_eventsGroupComboBox, 0, wxALL|wxEXPAND, 5);
    
    m_eventsGroupsNames = new wxStaticText(this, wxID_ANY, _("Warning: Multiple groups have the same name. Only the first one will be included !"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_eventsGroupsNames->Wrap(285);
    wxFont m_eventsGroupsNamesFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_eventsGroupsNamesFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_eventsGroupsNames->SetFont(m_eventsGroupsNamesFont);
    
    flexGridSizer158->Add(m_eventsGroupsNames, 0, wxALL|wxEXPAND|wxALIGN_LEFT, 5);
    
    m_includeEventsByIndexRadio = new wxRadioButton(this, wxID_ANY, _("Only include the events: (deprecated)"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_includeEventsByIndexRadio->SetValue(0);
    
    flexGridSizer158->Add(m_includeEventsByIndexRadio, 0, wxALL, 5);
    
    m_deprecatedPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTAB_TRAVERSAL);
    
    flexGridSizer158->Add(m_deprecatedPanel, 0, wxALL|wxEXPAND, 0);
    
    wxFlexGridSizer* flexGridSizer192 = new wxFlexGridSizer(0, 1, 0, 0);
    flexGridSizer192->SetFlexibleDirection( wxBOTH );
    flexGridSizer192->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer192->AddGrowableCol(0);
    m_deprecatedPanel->SetSizer(flexGridSizer192);
    
    wxFlexGridSizer* flexGridSizer180 = new wxFlexGridSizer(0, 5, 0, 0);
    flexGridSizer180->SetFlexibleDirection( wxBOTH );
    flexGridSizer180->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    
    flexGridSizer192->Add(flexGridSizer180, 1, wxALL|wxEXPAND, 0);
    
    m_staticText182 = new wxStaticText(m_deprecatedPanel, wxID_ANY, _("From"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer180->Add(m_staticText182, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_startTextCtrl = new wxTextCtrl(m_deprecatedPanel, wxID_ANY, wxT("10"), wxDefaultPosition, wxSize(40,-1), 0);
    #if wxVERSION_NUMBER >= 3000
    m_startTextCtrl->SetHint(wxT(""));
    #endif
    
    flexGridSizer180->Add(m_startTextCtrl, 0, wxALL, 5);
    
    m_staticText186 = new wxStaticText(m_deprecatedPanel, wxID_ANY, _("to"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer180->Add(m_staticText186, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_endTextCtrl = new wxTextCtrl(m_deprecatedPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(40,-1), 0);
    #if wxVERSION_NUMBER >= 3000
    m_endTextCtrl->SetHint(wxT(""));
    #endif
    
    flexGridSizer180->Add(m_endTextCtrl, 0, wxALL, 5);
    
    m_staticText194 = new wxStaticText(m_deprecatedPanel, wxID_ANY, _("(included)"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer180->Add(m_staticText194, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_staticText196 = new wxStaticText(m_deprecatedPanel, wxID_ANY, _("This feature is only provided to keep compatibility with older projects."), wxDefaultPosition, wxSize(-1,-1), 0);
    m_staticText196->Wrap(285);
    
    flexGridSizer192->Add(m_staticText196, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_staticText198 = new wxStaticText(m_deprecatedPanel, wxID_ANY, _("Not available for newly created link events."), wxDefaultPosition, wxSize(-1,-1), 0);
    m_staticText198->Wrap(285);
    wxFont m_staticText198Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticText198Font.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticText198->SetFont(m_staticText198Font);
    
    flexGridSizer192->Add(m_staticText198, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_stdBtnSizer172 = new wxStdDialogButtonSizer();
    
    flexGridSizer158->Add(m_stdBtnSizer172, 0, wxALL|wxALIGN_RIGHT, 5);
    
    m_okButton = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer172->AddButton(m_okButton);
    
    m_cancelButton = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer172->AddButton(m_cancelButton);
    m_stdBtnSizer172->Realize();
    
    SetName(wxT("LinkEventEditorBase"));
    SetSize(-1,-1);
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
    // Connect events
    m_eventsComboBox->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LinkEventEditorBase::OnEventsComboBoxTextChanged), NULL, this);
    m_includeAllEventsRadio->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(LinkEventEditorBase::OnIncludeAllEventsRadioButtonClicked), NULL, this);
    m_includeEventsGroupRadio->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(LinkEventEditorBase::OnIncludeEventsGroupRadioButtonClicked), NULL, this);
    m_eventsGroupComboBox->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LinkEventEditorBase::OnEventsGroupComboBoxTextChanged), NULL, this);
    m_includeEventsByIndexRadio->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(LinkEventEditorBase::OnIncludeByIndexRadioButtonClicked), NULL, this);
    m_okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LinkEventEditorBase::OnOkButtonClicked), NULL, this);
    m_cancelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LinkEventEditorBase::OnCancelButtonClicked), NULL, this);
    
}

LinkEventEditorBase::~LinkEventEditorBase()
{
    m_eventsComboBox->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LinkEventEditorBase::OnEventsComboBoxTextChanged), NULL, this);
    m_includeAllEventsRadio->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(LinkEventEditorBase::OnIncludeAllEventsRadioButtonClicked), NULL, this);
    m_includeEventsGroupRadio->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(LinkEventEditorBase::OnIncludeEventsGroupRadioButtonClicked), NULL, this);
    m_eventsGroupComboBox->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LinkEventEditorBase::OnEventsGroupComboBoxTextChanged), NULL, this);
    m_includeEventsByIndexRadio->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(LinkEventEditorBase::OnIncludeByIndexRadioButtonClicked), NULL, this);
    m_okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LinkEventEditorBase::OnOkButtonClicked), NULL, this);
    m_cancelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LinkEventEditorBase::OnCancelButtonClicked), NULL, this);
    
}
