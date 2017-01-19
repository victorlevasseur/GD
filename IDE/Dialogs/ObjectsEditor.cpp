#include "ObjectsEditor.h"

//(*InternalHeaders(ObjectsEditor)
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include <functional>
#include <set>

#include <wx/toolbar.h>
#include <wx/textdlg.h>
#include <wx/help.h>
#include <wx/config.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <wx/imaglist.h>
#include <wx/richtooltip.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>
#include "GDCore/Tools/Log.h"
#include "GDCore/IDE/Dialogs/ChooseObjectDialog.h"
#include "GDCore/IDE/Dialogs/ChooseVariableDialog.h"
#include "GDCore/IDE/Dialogs/ChooseBehaviorTypeDialog.h"
#include "GDCore/IDE/Dialogs/ChooseObjectTypeDialog.h"
#include "GDCore/IDE/Dialogs/PropertyDescriptor.h"
#include "GDCore/IDE/Dialogs/ObjectListDialogsHelper.h"
#include "GDCore/Tools/HelpFileAccess.h"
#include "GDCore/IDE/wxTools/TreeItemStringData.h"
#include "GDCore/IDE/wxTools/SkinHelper.h"
#include "GDCore/IDE/wxTools/TreeCtrlRestorer.h"
#include "GDCore/IDE/Clipboard.h"
#include "GDCore/IDE/NewNameGenerator.h"
#include "GDCore/IDE/Events/EventsRefactorer.h"
#include "GDCore/IDE/ObjectOrGroupFinder.h"
#include "GDCore/IDE/ObjectsFolderHelper.h"
#include "GDCore/Extensions/Platform.h"
#include "GDCore/Project/Object.h"
#include "GDCore/CommonTools.h"
#include "LayoutEditorPropertiesPnl.h"
#include "../LogFileManager.h"
#include "../EditObjectGroup.h"

#ifdef __WXMSW__
#include <wx/msw/winundef.h>
#include <wx/msw/uxtheme.h>
#endif

using namespace std;

namespace
{
    /**
     * Allow to drop an object in a group.
     */
    class ObjectsListDnd : public wxTextDropTarget
    {
    public:
        ObjectsListDnd(wxTreeCtrl *ctrl, wxTreeItemId objectsRootItem_, wxTreeItemId globalObjectsRootItem_, wxTreeItemId groupsRootItem_, wxTreeItemId globalGroupsRootItem_, gd::Project & project_, gd::Layout & layout_)
         : treeCtrl(ctrl), objectsRootItem(objectsRootItem_), globalObjectsRootItem(globalObjectsRootItem_), groupsRootItem(groupsRootItem_), globalGroupsRootItem(globalGroupsRootItem_), project(project_), layout(layout_) {};

        virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text)
        {
            gd::String content = text;

            //Try to workaround a wxMac making string not ending properly
            //See: http://trac.wxwidgets.org/ticket/9522#comment:4
            gd::String allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_/";
            content = content.substr(0, content.find_first_not_of(allowedCharacters));

            gd::String typeOfDrop = content.substr(0, 3);

            if( typeOfDrop == "OBJ" ) // An object is dropped
            {
                gd::String objectName = content.substr(3);

                //Get the item under the mouse
                int dropFlags;
                wxTreeItemId itemUnderMouse = treeCtrl->HitTest(wxPoint(x, y), dropFlags);

                if(!itemUnderMouse.IsOk())
                    return false; //If not on an item, stop.

                //Find if the item is a group (global ? local ?)
                gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(treeCtrl->GetItemData(itemUnderMouse));

                if( data && (data->GetString() == "LayoutGroup" || data->GetString() == "GlobalGroup") )
                {
                    std::vector<gd::ObjectGroup> *groups = NULL;
                    if(data->GetString() == "LayoutGroup")
                        groups = &layout.GetObjectGroups();
                    else if(data->GetString() == "GlobalGroup")
                        groups = &project.GetObjectGroups();
                    else
                        return false;

                    //Find the group
                    std::vector< gd::ObjectGroup >::iterator group = find_if(groups->begin(), groups->end(),
                        bind2nd(gd::GroupHasTheSameName(), treeCtrl->GetItemText(itemUnderMouse)));
                    if ( group != groups->end() && !group->Find(objectName))
                    {
                        //Add the object in the group
                        group->AddObject(objectName);
                        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
                            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupEdited(
                                project, data->GetString() == "GlobalGroup" ? NULL : &layout, group->GetName()
                                );
                    }
                    else
                    {
                        return false;
                    }

                    //Add the corresponding tree item into the group tree item
                    wxTreeItemId objectIntoGroupItem = treeCtrl->AppendItem(itemUnderMouse, objectName, 0);
                    treeCtrl->SetItemTextColour(objectIntoGroupItem, wxColour(128, 128, 128));
                    treeCtrl->SetItemData(objectIntoGroupItem, new gd::TreeItemStringData("ObjectInGroup"));

                    treeCtrl->Expand(itemUnderMouse);
                }
                else if( itemUnderMouse == objectsRootItem || itemUnderMouse == globalObjectsRootItem ||
                    (data && (data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" || data->GetString() == "LayoutObject" || data->GetString() == "GlobalObject")) )
                {
                    gd::String folder;
                    std::size_t insertionPosInNewFolder;

                    if(!layout.HasObjectNamed(objectName) && !project.HasObjectNamed(objectName))
                        return false; // The object being dragged doesn't exist!

                    bool isGlobal = !layout.HasObjectNamed(objectName);
                    gd::ClassWithObjects & currentObjectsList = layout.HasObjectNamed(objectName) ?
                        dynamic_cast<gd::ClassWithObjects&>(layout) : dynamic_cast<gd::ClassWithObjects&>(project);

                    if( itemUnderMouse == objectsRootItem || itemUnderMouse == globalObjectsRootItem )
                    {
                        if( itemUnderMouse == objectsRootItem && isGlobal || itemUnderMouse == globalObjectsRootItem && !isGlobal )
                            return false;

                        std::size_t i = 1;
                        folder = _("NewFolder");
                        while( gd::ObjectsFolderHelper::HasFolder( currentObjectsList, folder ) )
                        {
                            folder = _("NewFolder") + gd::String::From(i);
                            ++i;
                        }

                        insertionPosInNewFolder = 0;
                    }
                    else if( data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" )
                    {
                        //TODO: Check if global or layout object <-> global or layout folder
                        folder = data->GetSecondString();
                        insertionPosInNewFolder = gd::ObjectsFolderHelper::GetObjectsCount( folder, currentObjectsList );
                    }
                    else // On an object
                    {
                        gd::String hoveredObjectName = treeCtrl->GetItemText(itemUnderMouse);

                        if(!layout.HasObjectNamed(hoveredObjectName) && !project.HasObjectNamed(hoveredObjectName))
                            return false; // The hovered object doesn't exist!

                        gd::ClassWithObjects & hoveredObjectsList = layout.HasObjectNamed(hoveredObjectName) ?
                            dynamic_cast<gd::ClassWithObjects&>(layout) : dynamic_cast<gd::ClassWithObjects&>(project);
                        bool isHoveredGlobal = !layout.HasObjectNamed(hoveredObjectName);

                        gd::Object & hoveredObject = hoveredObjectsList.GetObject(hoveredObjectName);

                        folder = hoveredObject.GetFolder();
                        // If both are local or global, then insert the moved object to the position of the hovered object
                        if(isHoveredGlobal == isGlobal)
                        {
                            insertionPosInNewFolder = gd::ObjectsFolderHelper::GetObjectPositionInFolder( folder, hoveredObjectsList, hoveredObjectName );
                        }
                        else
                        {
                            return false;
                        }
                    }

                    // Change the folder of the object
                    gd::ObjectsFolderHelper::ChangeObjectFolder( currentObjectsList, objectName, folder, insertionPosInNewFolder );
                }
                else if( itemUnderMouse == groupsRootItem || itemUnderMouse == globalGroupsRootItem )
                {
                    //Create a new group
                    gd::ObjectGroup newGroup;
                    newGroup.AddObject(text);

                    std::vector<gd::ObjectGroup> & objectsGroups = itemUnderMouse == groupsRootItem ? layout.GetObjectGroups() : project.GetObjectGroups();

                    gd::String name = gd::NewNameGenerator::Generate(text + "Group", [&objectsGroups](const gd::String & name){
                        return std::find_if(objectsGroups.begin(), objectsGroups.end(), std::bind2nd(gd::GroupHasTheSameName(), name))
                            != objectsGroups.end();
                    });

                    newGroup.SetName(name);
                    objectsGroups.push_back(newGroup);

                    //Notify the game of the new group
                    for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
                        project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupAdded(project, &layout, name);
                    gd::LogStatus( _( "The group was correctly added." ) );

                    return true;
                }
                else
                {
                    return false;
                }

                return true;
            }
            else if( typeOfDrop == "FOL" ) // A folder is dropped
            {
                bool global = content.substr(3, 3) == "GLO" ? true : false;
                gd::String folder = content.substr(6);

                //Get the item under the mouse
                int dropFlags;
                wxTreeItemId itemUnderMouse = treeCtrl->HitTest(wxPoint(x, y), dropFlags);

                if(!itemUnderMouse.IsOk())
                    return false; //If not on an item, stop.

                //Find if the item is a group (global ? local ?)
                gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(treeCtrl->GetItemData(itemUnderMouse));

                auto folderComponents = folder.Split( U'/' );
                gd::String folderSuffixName = folderComponents[ folderComponents.size() - 1 ];

                if( data && ( !global && data->GetString() == "ObjectsFolder" || global && data->GetString() == "GlobalObjectsFolder" ) )
                {
                    return gd::ObjectsFolderHelper::MoveFolder(
                        global ? dynamic_cast<gd::ClassWithObjects&>( project ) : dynamic_cast<gd::ClassWithObjects&>( layout ),
                        folder,
                        data->GetSecondString() + "/" + folderSuffixName );
                }
                else if( !global && itemUnderMouse == objectsRootItem || global && itemUnderMouse == globalObjectsRootItem )
                {
                    return gd::ObjectsFolderHelper::MoveFolder(
                        global ? dynamic_cast<gd::ClassWithObjects&>( project ) : dynamic_cast<gd::ClassWithObjects&>( layout ),
                        folder,
                        folderSuffixName );
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

    private:
        wxTreeCtrl *treeCtrl;
        wxTreeItemId objectsRootItem;
        wxTreeItemId globalObjectsRootItem;
        wxTreeItemId groupsRootItem;
        wxTreeItemId globalGroupsRootItem;

        gd::Project & project;
        gd::Layout & layout;
    };
}

namespace gd {

//(*IdInit(ObjectsEditor)
const long ObjectsEditor::ID_TREECTRL1 = wxNewId();
const long ObjectsEditor::ID_TEXTCTRL1 = wxNewId();
const long ObjectsEditor::idMenuModObj = wxNewId();
const long ObjectsEditor::idMenuAddAuto = wxNewId();
const long ObjectsEditor::idMenuProp = wxNewId();
const long ObjectsEditor::idMenuAddObj = wxNewId();
const long ObjectsEditor::idMenuDelObj = wxNewId();
const long ObjectsEditor::idMenuModName = wxNewId();
const long ObjectsEditor::ID_SETGLOBALITEM = wxNewId();
const long ObjectsEditor::idMoveUp = wxNewId();
const long ObjectsEditor::idMoveDown = wxNewId();
const long ObjectsEditor::idMenuCopy = wxNewId();
const long ObjectsEditor::idMenuCut = wxNewId();
const long ObjectsEditor::idMenuPaste = wxNewId();
const long ObjectsEditor::ID_MENUITEM7 = wxNewId();
const long ObjectsEditor::IdGroupEdit = wxNewId();
const long ObjectsEditor::idModName = wxNewId();
const long ObjectsEditor::ID_MENUITEM8 = wxNewId();
const long ObjectsEditor::idAddGroup = wxNewId();
const long ObjectsEditor::idDelGroup = wxNewId();
const long ObjectsEditor::ID_MENUITEM1 = wxNewId();
const long ObjectsEditor::ID_MENUITEM2 = wxNewId();
const long ObjectsEditor::ID_MENUITEM3 = wxNewId();
const long ObjectsEditor::ID_MENUITEM4 = wxNewId();
const long ObjectsEditor::ID_MENUITEM5 = wxNewId();
const long ObjectsEditor::ID_MENUITEM6 = wxNewId();
//*)
const long ObjectsEditor::idRibbonAdd = wxNewId();
const long ObjectsEditor::idRibbonAddGroup = wxNewId();
const long ObjectsEditor::idRibbonDel = wxNewId();
const long ObjectsEditor::idRibbonUp = wxNewId();
const long ObjectsEditor::idRibbonDown = wxNewId();
const long ObjectsEditor::idRibbonModProp = wxNewId();
const long ObjectsEditor::idRibbonModName = wxNewId();
const long ObjectsEditor::idRibbonCopy = wxNewId();
const long ObjectsEditor::idRibbonCut = wxNewId();
const long ObjectsEditor::idRibbonPaste = wxNewId();
const long ObjectsEditor::idRibbonHelp = wxNewId();
const long ObjectsEditor::idRibbonRefresh = wxNewId();

const long ObjectsEditor::idMoveUpFolderMenuItem = wxNewId();
const long ObjectsEditor::idMoveDownFolderMenuItem = wxNewId();
const long ObjectsEditor::idRenameFolderMenuItem = wxNewId();
const long ObjectsEditor::idDeleteFolderMenuItem = wxNewId();

wxRibbonButtonBar *ObjectsEditor::objectsRibbonBar = NULL;
wxRibbonButtonBar *ObjectsEditor::selectionRibbonBar = NULL;
wxRibbonButtonBar *ObjectsEditor::clipboardRibbonBar = NULL;

BEGIN_EVENT_TABLE(ObjectsEditor,wxPanel)
    //(*EventTable(ObjectsEditor)
    //*)
END_EVENT_TABLE()

ObjectsEditor::ObjectsEditor(wxWindow* parent, gd::Project & project_, gd::Layout & layout_, gd::MainFrameWrapper & mainFrameWrapper_) :
    project(project_),
    layout(layout_),
    mainFrameWrapper(mainFrameWrapper_),
    propPnl(NULL),
    propPnlManager(NULL),
    listsHelper(project, layout)
{
    //(*Initialize(ObjectsEditor)
    wxMenuItem* delObjMenuI;
    wxMenuItem* editNameMenuI;
    wxMenuItem* editPropMenuItem;
    wxMenuItem* editMenuI;
    wxMenuItem* editMenuItem;
    wxFlexGridSizer* FlexGridSizer1;
    wxMenuItem* addObjMenuI;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(0);
    objectsList = new wxTreeCtrl(this, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS|wxTR_HIDE_ROOT|wxTR_MULTIPLE|wxTR_DEFAULT_STYLE|wxNO_BORDER, wxDefaultValidator, _T("ID_TREECTRL1"));
    FlexGridSizer1->Add(objectsList, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    searchCtrl = new wxSearchCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer1->Add(searchCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(FlexGridSizer1);
    editMenuI = new wxMenuItem((&contextMenu), idMenuModObj, _("Edit"), wxEmptyString, wxITEM_NORMAL);
    contextMenu.Append(editMenuI);
    MenuItem13 = new wxMenuItem((&contextMenu), idMenuAddAuto, _("Add a behavior"), wxEmptyString, wxITEM_NORMAL);
    contextMenu.Append(MenuItem13);
    editPropMenuItem = new wxMenuItem((&contextMenu), idMenuProp, _("Other properties"), wxEmptyString, wxITEM_NORMAL);
    editPropMenuItem->SetBitmap(gd::SkinHelper::GetIcon("properties", 16));
    contextMenu.Append(editPropMenuItem);
    contextMenu.AppendSeparator();
    addObjMenuI = new wxMenuItem((&contextMenu), idMenuAddObj, _("Add an object"), wxEmptyString, wxITEM_NORMAL);
    addObjMenuI->SetBitmap(gd::SkinHelper::GetIcon("add", 16));
    contextMenu.Append(addObjMenuI);
    delObjMenuI = new wxMenuItem((&contextMenu), idMenuDelObj, _("Delete\tDel"), wxEmptyString, wxITEM_NORMAL);
    delObjMenuI->SetBitmap(gd::SkinHelper::GetIcon("delete", 16));
    contextMenu.Append(delObjMenuI);
    editNameMenuI = new wxMenuItem((&contextMenu), idMenuModName, _("Rename\tF2"), wxEmptyString, wxITEM_NORMAL);
    editNameMenuI->SetBitmap(gd::SkinHelper::GetIcon("rename", 16));
    contextMenu.Append(editNameMenuI);
    MenuItem11 = new wxMenuItem((&contextMenu), ID_SETGLOBALITEM, _("Set as global object"), wxEmptyString, wxITEM_NORMAL);
    contextMenu.Append(MenuItem11);
    contextMenu.AppendSeparator();
    moveUpMenuI = new wxMenuItem((&contextMenu), idMoveUp, _("Move up\tCtrl-Up"), wxEmptyString, wxITEM_NORMAL);
    moveUpMenuI->SetBitmap(gd::SkinHelper::GetIcon("up", 16));
    contextMenu.Append(moveUpMenuI);
    moveDownMenuI = new wxMenuItem((&contextMenu), idMoveDown, _("Move down\tCtrl-Down"), wxEmptyString, wxITEM_NORMAL);
    moveDownMenuI->SetBitmap(gd::SkinHelper::GetIcon("down", 16));
    contextMenu.Append(moveDownMenuI);
    contextMenu.AppendSeparator();
    copyMenuI = new wxMenuItem((&contextMenu), idMenuCopy, _("Copy\tCtrl-C"), wxEmptyString, wxITEM_NORMAL);
    copyMenuI->SetBitmap(gd::SkinHelper::GetIcon("copy", 16));
    contextMenu.Append(copyMenuI);
    cutMenuI = new wxMenuItem((&contextMenu), idMenuCut, _("Cut\tCtrl-X"), wxEmptyString, wxITEM_NORMAL);
    cutMenuI->SetBitmap(gd::SkinHelper::GetIcon("cut", 16));
    contextMenu.Append(cutMenuI);
    pasteMenuI = new wxMenuItem((&contextMenu), idMenuPaste, _("Paste\tCtrl-V"), wxEmptyString, wxITEM_NORMAL);
    pasteMenuI->SetBitmap(gd::SkinHelper::GetIcon("paste", 16));
    contextMenu.Append(pasteMenuI);
    MenuItem2 = new wxMenuItem((&multipleContextMenu), ID_MENUITEM7, _("Delete\tDEL"), _("Delete all selected items"), wxITEM_NORMAL);
    MenuItem2->SetBitmap(gd::SkinHelper::GetIcon("delete", 16));
    multipleContextMenu.Append(MenuItem2);
    editMenuItem = new wxMenuItem((&groupContextMenu), IdGroupEdit, _("Add object(s) to group"), wxEmptyString, wxITEM_NORMAL);
    editMenuItem->SetBitmap(gd::SkinHelper::GetIcon("properties", 16));
    groupContextMenu.Append(editMenuItem);
    MenuItem4 = new wxMenuItem((&groupContextMenu), idModName, _("Rename"), wxEmptyString, wxITEM_NORMAL);
    MenuItem4->SetBitmap(gd::SkinHelper::GetIcon("rename", 16));
    groupContextMenu.Append(MenuItem4);
    MenuItem12 = new wxMenuItem((&groupContextMenu), ID_MENUITEM8, _("Set as global group"), wxEmptyString, wxITEM_NORMAL);
    groupContextMenu.Append(MenuItem12);
    groupContextMenu.AppendSeparator();
    MenuItem1 = new wxMenuItem((&groupContextMenu), idAddGroup, _("Add a group"), wxEmptyString, wxITEM_NORMAL);
    MenuItem1->SetBitmap(gd::SkinHelper::GetIcon("add", 16));
    groupContextMenu.Append(MenuItem1);
    MenuItem3 = new wxMenuItem((&groupContextMenu), idDelGroup, _("Delete"), wxEmptyString, wxITEM_NORMAL);
    MenuItem3->SetBitmap(gd::SkinHelper::GetIcon("delete", 16));
    groupContextMenu.Append(MenuItem3);
    groupContextMenu.AppendSeparator();
    MenuItem5 = new wxMenuItem((&groupContextMenu), ID_MENUITEM1, _("Copy"), wxEmptyString, wxITEM_NORMAL);
    MenuItem5->SetBitmap(gd::SkinHelper::GetIcon("copy", 16));
    groupContextMenu.Append(MenuItem5);
    MenuItem6 = new wxMenuItem((&groupContextMenu), ID_MENUITEM2, _("Cut"), wxEmptyString, wxITEM_NORMAL);
    MenuItem6->SetBitmap(gd::SkinHelper::GetIcon("cut", 16));
    groupContextMenu.Append(MenuItem6);
    MenuItem7 = new wxMenuItem((&groupContextMenu), ID_MENUITEM3, _("Paste"), wxEmptyString, wxITEM_NORMAL);
    MenuItem7->SetBitmap(gd::SkinHelper::GetIcon("paste", 16));
    groupContextMenu.Append(MenuItem7);
    MenuItem8 = new wxMenuItem((&emptyContextMenu), ID_MENUITEM4, _("Add an object"), wxEmptyString, wxITEM_NORMAL);
    MenuItem8->SetBitmap(gd::SkinHelper::GetIcon("add", 16));
    emptyContextMenu.Append(MenuItem8);
    MenuItem9 = new wxMenuItem((&emptyContextMenu), ID_MENUITEM5, _("Add a group"), wxEmptyString, wxITEM_NORMAL);
    emptyContextMenu.Append(MenuItem9);
    emptyContextMenu.AppendSeparator();
    MenuItem10 = new wxMenuItem((&emptyContextMenu), ID_MENUITEM6, _("Paste"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->SetBitmap(gd::SkinHelper::GetIcon("paste", 16));
    emptyContextMenu.Append(MenuItem10);

    // Folder context menu
    {
        wxMenuItem * item = new wxMenuItem((&folderContextMenu), idRenameFolderMenuItem, _("Rename\tF2"), wxEmptyString, wxITEM_NORMAL);
        item->SetBitmap(gd::SkinHelper::GetIcon("rename", 16));
        folderContextMenu.Append(item);
        Connect(idRenameFolderMenuItem, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ObjectsEditor::OnMenuRenameSelected);
    }
    {
        wxMenuItem * item = new wxMenuItem((&folderContextMenu), idDeleteFolderMenuItem, _("Delete\tDEL"), wxEmptyString, wxITEM_NORMAL);
        item->SetBitmap(gd::SkinHelper::GetIcon("delete", 16));
        folderContextMenu.Append(item);
        Connect(idRenameFolderMenuItem, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ObjectsEditor::OnDeleteSelected);
    }
    folderContextMenu.AppendSeparator();
    {
        wxMenuItem * item = new wxMenuItem((&folderContextMenu), idMoveUpFolderMenuItem, _("Move up\tCtrl-Up"), wxEmptyString, wxITEM_NORMAL);
        item->SetBitmap(gd::SkinHelper::GetIcon("up", 16));
        folderContextMenu.Append(item);
        Connect(idMoveUpFolderMenuItem, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ObjectsEditor::OnMoveupSelected);
    }
    {
        wxMenuItem * item = new wxMenuItem((&folderContextMenu), idMoveDownFolderMenuItem, _("Move down\tCtrl-Down"), wxEmptyString, wxITEM_NORMAL);
        item->SetBitmap(gd::SkinHelper::GetIcon("down", 16));
        folderContextMenu.Append(item);
        Connect(idMoveDownFolderMenuItem, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ObjectsEditor::OnMoveDownSelected);
    }

    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_BEGIN_DRAG,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListBeginDrag);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListBeginLabelEdit);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_END_LABEL_EDIT,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListEndLabelEdit);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListItemActivated);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListItemRightClick);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListSelectionChanged);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_KEY_DOWN,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListKeyDown);
    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_ITEM_MENU,(wxObjectEventFunction)&ObjectsEditor::OnobjectsListItemMenu);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&ObjectsEditor::OnsearchCtrlText);
    Connect(idMenuModObj,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMenuEditObjectSelected);
    Connect(idMenuAddAuto,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMenuAddBehaviorSelected);
    Connect(idMenuProp,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMenuPropertiesSelected);
    Connect(idMenuAddObj,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnAddObjectSelected);
    Connect(idMenuDelObj,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnDeleteSelected);
    Connect(idMenuModName,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMenuRenameSelected);
    Connect(ID_SETGLOBALITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnSetGlobalSelected);
    Connect(idMoveUp,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMoveupSelected);
    Connect(idMoveDown,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMoveDownSelected);
    Connect(idMenuCopy,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnCopySelected);
    Connect(idMenuCut,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnCutSelected);
    Connect(idMenuPaste,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnPasteSelected);
    Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnDeleteSelected);
    Connect(IdGroupEdit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMenuEditObjectSelected);
    Connect(idModName,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnMenuRenameSelected);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnSetGlobalSelected);
    Connect(idAddGroup,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnAddGroupSelected);
    Connect(idDelGroup,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnDeleteSelected);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnCopySelected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnCutSelected);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnPasteSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnAddObjectSelected);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnAddGroupSelected);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectsEditor::OnPasteSelected);
    Connect(wxEVT_SET_FOCUS,(wxObjectEventFunction)&ObjectsEditor::OnSetFocus);
    //*)

    #if defined(__WXMSW__) //Offer nice look to wxTreeCtrl
    wxUxThemeEngine* theme =  wxUxThemeEngine::GetIfActive();
    if(theme) theme->SetWindowTheme((HWND) objectsList->GetHWND(), L"EXPLORER", NULL);
    #endif

    Refresh();
}

ObjectsEditor::~ObjectsEditor()
{
    //(*Destroy(ObjectsEditor)
    //*)
}

void ObjectsEditor::SetAssociatedPropertiesPanel(LayoutEditorPropertiesPnl * propPnl_, wxAuiManager * manager_)
{
    propPnl = propPnl_;
    propPnlManager = manager_;
}

/**
 * Static method for creating ribbon page for this kind of editor
 */
void ObjectsEditor::CreateRibbonPage(wxRibbonPage * page)
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    bool hideLabels = false;
    pConfig->Read( _T( "/Skin/HideLabels" ), &hideLabels );

    {
        wxRibbonPanel *ribbonPanel = new wxRibbonPanel(page, wxID_ANY, _("Objects and groups"), gd::SkinHelper::GetRibbonIcon("list"), wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE);
        objectsRibbonBar = new wxRibbonButtonBar(ribbonPanel, wxID_ANY);
        objectsRibbonBar->AddButton(idRibbonAdd, !hideLabels ? _("Add an object") : gd::String(), gd::SkinHelper::GetRibbonIcon("add"), _("Add a new object to the list of the objects of the scene"));
        objectsRibbonBar->AddButton(idRibbonAddGroup, !hideLabels ? _("Add a group") : gd::String(), gd::SkinHelper::GetRibbonIcon("add"), _("Add a new group to the scene"));
        objectsRibbonBar->AddButton(idRibbonDel, !hideLabels ? _("Delete") : gd::String(), gd::SkinHelper::GetRibbonIcon("delete"), _("Delete the selected object or group"));
        objectsRibbonBar->AddButton(idRibbonUp, !hideLabels ? _("Move up") : gd::String(), gd::SkinHelper::GetRibbonIcon("up"));
        objectsRibbonBar->AddButton(idRibbonDown, !hideLabels ? _("Move down") : gd::String(), gd::SkinHelper::GetRibbonIcon("down"));
    }
    {
        wxRibbonPanel *ribbonPanel = new wxRibbonPanel(page, wxID_ANY, _("Selection"), gd::SkinHelper::GetRibbonIcon("edit"), wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE);
        selectionRibbonBar = new wxRibbonButtonBar(ribbonPanel, wxID_ANY);
        selectionRibbonBar->AddButton(idRibbonModProp, !hideLabels ? _("Edit") : gd::String(""), gd::SkinHelper::GetRibbonIcon("editprop"), _("Edit the selected object"));
        selectionRibbonBar->AddButton(idRibbonModName, !hideLabels ? _("Rename") : gd::String(""), gd::SkinHelper::GetRibbonIcon("editname"), _("Rename the selected item"));
    }
    {
        wxRibbonPanel *ribbonPanel = new wxRibbonPanel(page, wxID_ANY, _("Clipboard"), gd::SkinHelper::GetRibbonIcon("copy"), wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE);
        clipboardRibbonBar = new wxRibbonButtonBar(ribbonPanel, wxID_ANY);
        clipboardRibbonBar->AddButton(idRibbonCopy, !hideLabels ? _("Copy") : gd::String(), gd::SkinHelper::GetRibbonIcon("copy"));
        clipboardRibbonBar->AddButton(idRibbonCut, !hideLabels ? _("Cut") : gd::String(), gd::SkinHelper::GetRibbonIcon("cut"));
        clipboardRibbonBar->AddButton(idRibbonPaste, !hideLabels ? _("Paste") : gd::String(), gd::SkinHelper::GetRibbonIcon("paste"));
    }
    {
        wxRibbonPanel *ribbonPanel = new wxRibbonPanel(page, wxID_ANY, _("Help"), gd::SkinHelper::GetRibbonIcon("help"), wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE);
        wxRibbonButtonBar *ribbonBar = new wxRibbonButtonBar(ribbonPanel, wxID_ANY);
        ribbonBar->AddButton(idRibbonHelp, !hideLabels ? _("Help") : gd::String(), gd::SkinHelper::GetRibbonIcon("help"), _("Show the online help"));
    }

}

void ObjectsEditor::OnHelpSelected( wxCommandEvent& event )
{
    gd::HelpFileAccess::Get()->OpenPage("gdevelop/documentation/manual/edit_object");
}

void ObjectsEditor::ConnectEvents()
{
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonAdd, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnAddObjectSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonAddGroup, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnAddGroupSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonDel, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnDeleteSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonUp, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnMoveupSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonDown, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnMoveDownSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonModProp, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnMenuPropertiesSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonModName, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnMenuRenameSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonCopy, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnCopySelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonCut, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnCutSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonPaste, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnPasteSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonHelp, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::OnHelpSelected, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonRefresh, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&ObjectsEditor::Refresh, NULL, this);
}

void ObjectsEditor::Refresh()
{
    gd::TreeCtrlRestorer restorer( [](const wxTreeCtrl * ctrl, wxTreeItemId item) -> std::size_t
    {
        std::size_t itemNameHash = std::hash<std::string>{}( gd::String( ctrl->GetItemText( item ) ).ToUTF8() );

        gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>( ctrl->GetItemData( item ) );
        if( !data )
            return itemNameHash;

        std::size_t seed = itemNameHash;
        seed ^= std::hash<std::string>{}( data->GetString().ToUTF8() ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<std::string>{}( data->GetSecondString().ToUTF8() ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    });

    restorer.SaveState( objectsList );

    listsHelper.SetGroupExtraRendering([this](wxTreeItemId groupItem) {
        this->UpdateGroup(groupItem);
    });
    listsHelper.SetSearchText(searchCtrl->GetValue());
    listsHelper.RefreshList(objectsList, &objectsRootItem, &globalObjectsRootItem, &groupsRootItem, &globalGroupsRootItem);
    objectsList->SetDropTarget(new ObjectsListDnd(objectsList, objectsRootItem, globalObjectsRootItem, groupsRootItem, globalGroupsRootItem, project, layout));

    restorer.RestoreState( objectsList );

    if (onRefreshedCb) onRefreshedCb();
}

void ObjectsEditor::UpdateGroup(wxTreeItemId groupItem)
{
    if(!groupItem.IsOk())
        return;

    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(groupItem));
    bool wasExpanded = objectsList->IsExpanded(groupItem);

    if(!data || (data->GetString() != "LayoutGroup" && data->GetString() != "GlobalGroup"))
        return;

    //Clear the group item content
    objectsList->DeleteChildren(groupItem);

    std::vector<gd::ObjectGroup> & groups = (data->GetString() == "LayoutGroup") ? layout.GetObjectGroups() : project.GetObjectGroups();

    //Find the group in the container
    vector<gd::ObjectGroup>::iterator i = std::find_if( groups.begin(),
                                                        groups.end(),
                                                        std::bind2nd(gd::GroupHasTheSameName(), objectsList->GetItemText(groupItem)));

    if ( i != groups.end() )
    {
        //Add all objects items into the group item
        for(std::vector<gd::String>::const_iterator it = i->GetAllObjectsNames().begin(); it != i->GetAllObjectsNames().end(); it++)
        {
            wxTreeItemId objectItem = objectsList->AppendItem( groupItem, *it, 0 );
            objectsList->SetItemTextColour(objectItem, wxColour(128, 128, 128));
            objectsList->SetItemData(objectItem, new gd::TreeItemStringData("ObjectInGroup"));
        }
    }
    else
    {
        //If the group has not been found, we delete it from the tree
        objectsList->Delete(groupItem);
    }
}

void ObjectsEditor::OnobjectsListItemActivated(wxTreeEvent& event)
{
    mainFrameWrapper.SetRibbonPage(_("Objects"));
    ConnectEvents();

    lastSelectedItem = event.GetItem();

    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if( data && ( data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" ) )
    {
        if( objectsList->IsExpanded( lastSelectedItem ) )
            objectsList->Collapse( lastSelectedItem );
        else
            objectsList->Expand( lastSelectedItem );
    }
    else
    {
        wxCommandEvent useless;
        OnMenuEditObjectSelected( useless );
    }
}

void ObjectsEditor::OnobjectsListItemRightClick(wxTreeEvent& event)
{
    mainFrameWrapper.SetRibbonPage(_("Objects"));
    ConnectEvents();

    lastSelectedItem = event.GetItem();
}

void ObjectsEditor::OnobjectsListItemMenu(wxTreeEvent& event)
{
    lastSelectedItem = event.GetItem();
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    wxArrayTreeItemIds selection;

    if ( objectsList->GetSelections(selection) > 1 || (data && data->GetString() == "ObjectInGroup") )
    {
        PopupMenu( &multipleContextMenu );
    }
    else if ( lastSelectedItem == objectsRootItem ||
        lastSelectedItem == globalObjectsRootItem ||
        lastSelectedItem == groupsRootItem ||
        lastSelectedItem == globalGroupsRootItem )
        PopupMenu( &emptyContextMenu );
    else if ( data && ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" ) )
    {
        contextMenu.Enable(ID_SETGLOBALITEM, data->GetString() == "LayoutObject");
        PopupMenu( &contextMenu );
    }
    else if ( data && ( data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup" ) )
    {
        contextMenu.Enable(ID_SETGLOBALITEM, data->GetString() == "LayoutGroup");
        PopupMenu( &groupContextMenu );
    }
    else if ( data && ( data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" ) )
    {
        PopupMenu( &folderContextMenu );
    }
}

void ObjectsEditor::OnobjectsListKeyDown(wxTreeEvent& event)
{
    mainFrameWrapper.SetRibbonPage(_("Objects"));
    ConnectEvents();

    if ( event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_BACK )
    {
        wxCommandEvent unusedEvent;
        OnDeleteSelected( unusedEvent );
    }
    else if (event.GetKeyCode() == WXK_F2)
    {
        wxCommandEvent unusedEvent;
        OnMenuRenameSelected( unusedEvent );
    }
    else if (event.GetKeyEvent().GetModifiers() == wxMOD_CMD)
    {
        switch ( event.GetKeyCode() )
        {
            case 67: //Ctrl C
            {
                wxCommandEvent unusedEvent;
                OnCopySelected( unusedEvent );
                break;
            }
            case 86: //Ctrl-V
            {
                wxCommandEvent unusedEvent;
                OnPasteSelected( unusedEvent );
                break;
            }
            case 88: //Ctrl-X
            {
                wxCommandEvent unusedEvent;
                OnCutSelected( unusedEvent );
                break;
            }
            case WXK_UP:
            {
                wxCommandEvent unusedEvent;
                OnMoveupSelected( unusedEvent );
                break;
            }
            case WXK_DOWN:
            {
                wxCommandEvent unusedEvent;
                OnMoveDownSelected( unusedEvent );
                break;
            }
            default:
                break;
        }
    }
}

void ObjectsEditor::OnobjectsListBeginLabelEdit(wxTreeEvent& event)
{
    if ( !event.GetLabel().empty() ) //event.GetLabel() is empty on linux.
        renamedItemOldName = event.GetLabel();

    lastSelectedItem = event.GetItem();
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));

    if(!data || data->GetString() == "ObjectInGroup")
    {
        event.Veto();
        return;
    }
}

void ObjectsEditor::OnobjectsListEndLabelEdit(wxTreeEvent& event)
{
    if ( event.IsEditCancelled() ) return;

    gd::ObjectOrGroupFinder nameChecker(project, &layout);

    gd::String newName = event.GetLabel();
    gd::String oldName = renamedItemOldName;

    lastSelectedItem = event.GetItem();
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));

    //Rename an object
    if ( data && (data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject") )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        //Be sure there is not already another object with this name
        unsigned int nameCheckResult = nameChecker.HasObjectOrGroupNamed(newName, globalObject);
        if ( nameCheckResult != gd::ObjectOrGroupFinder::No )
        {
            gd::LogWarning( _( "Unable to rename the object : \n" ) + GetExistingObjectsErrorMessage(nameCheckResult, nameChecker.GetLayoutsWithSameObjectName()) );

            event.Veto();
            return;
        }

        //Be sure the name is valid
        if ( !project.ValidateObjectName(newName) )
        {
            wxRichToolTip tip(_("Invalid name"), project.GetBadObjectNameWarning());
            tip.SetIcon(wxICON_INFORMATION);
            tip.ShowFor(this);

            event.Veto();
            return;
        }

        if ( !objects.HasObjectNamed(oldName) ) return;
        objects.GetObject(oldName).SetName( newName );

        if ( !globalObject) //Change the object name in the layout.
        {
            gd::EventsRefactorer::RenameObjectInEvents(project.GetCurrentPlatform(), project, layout, layout.GetEvents(), oldName, newName);
            layout.GetInitialInstances().RenameInstancesOfObject(oldName, newName);
            for (std::size_t g = 0;g<layout.GetObjectGroups().size();++g)
            {
                if ( layout.GetObjectGroups()[g].Find(oldName))
                {
                    layout.GetObjectGroups()[g].RemoveObject(oldName);
                    layout.GetObjectGroups()[g].AddObject(newName);
                }
            }
            //TODO: Factor this? And change the name in external events.
        }
        else if ( globalObject ) //Change the object name in all layouts
        {
            for (std::size_t g = 0;g<project.GetObjectGroups().size();++g)
            {
                if ( project.GetObjectGroups()[g].Find(oldName))
                {
                    project.GetObjectGroups()[g].RemoveObject(oldName);
                    project.GetObjectGroups()[g].AddObject(newName);
                }
            }

            for (std::size_t i = 0;i<project.GetLayoutsCount();++i)
            {
                gd::Layout & layout = project.GetLayout(i);
                if ( layout.HasObjectNamed(oldName) ) continue;

                gd::EventsRefactorer::RenameObjectInEvents(project.GetCurrentPlatform(), project, layout, layout.GetEvents(), oldName, newName);
                layout.GetInitialInstances().RenameInstancesOfObject(oldName, newName);
                for (std::size_t g = 0;g<layout.GetObjectGroups().size();++g)
                {
                    if ( layout.GetObjectGroups()[g].Find(oldName))
                    {
                        layout.GetObjectGroups()[g].RemoveObject(oldName);
                        layout.GetObjectGroups()[g].AddObject(newName);
                    }
                }
            }

            //TODO: Factor this? And change the name in external events.
        }

        //Update the groups items (without refreshing the entire tree control)
        wxTreeItemIdValue cookie;
        for(wxTreeItemId groupItem = objectsList->GetFirstChild(groupsRootItem, cookie); groupItem.IsOk(); groupItem = objectsList->GetNextChild(groupsRootItem, cookie))
        {
            UpdateGroup(groupItem);
        }

        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectRenamed(project, globalObject ? NULL : &layout, objects.GetObject(newName), oldName);
    }
    // Rename a group
    else if ( data && (data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup") )
    {
        bool globalGroup = data->GetString() == "GlobalGroup";
        vector<gd::ObjectGroup> & objectsGroups =
            globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

        //Test if there are no other objects/groups with the same name
        unsigned int nameCheckResult = nameChecker.HasObjectOrGroupNamed(event.GetLabel(), globalGroup);
        if ( nameCheckResult != gd::ObjectOrGroupFinder::No )
        {
            gd::LogWarning( _( "Unable to rename the group : \n" ) + GetExistingObjectsErrorMessage(nameCheckResult, nameChecker.GetLayoutsWithSameObjectName()) );

            event.Veto();
            return;
        }

        vector<gd::ObjectGroup>::iterator i = std::find_if( objectsGroups.begin(),
                                                        objectsGroups.end(),
                                                        std::bind2nd(gd::GroupHasTheSameName(), oldName));

        if ( i != objectsGroups.end() )
        {
            i->SetName( newName );

            //TODO: Factor this? And change the name in external events.
            if (!globalGroup)
                gd::EventsRefactorer::RenameObjectInEvents(project.GetCurrentPlatform(), project, layout, layout.GetEvents(), oldName, newName);
            else
            {
                for (std::size_t i = 0;i<project.GetLayoutsCount();++i)
                {
                    gd::Layout & layout = project.GetLayout(i);
                    if (std::find_if( layout.GetObjectGroups().begin(),
                                      layout.GetObjectGroups().end(),
                                      std::bind2nd(gd::GroupHasTheSameName(), newName)) != layout.GetObjectGroups().end())
                        continue;

                    gd::EventsRefactorer::RenameObjectInEvents(project.GetCurrentPlatform(), project, layout, layout.GetEvents(), oldName, newName);
                }
            }

            for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
                project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupRenamed(project, globalGroup ? NULL : &layout, newName, oldName);
        }
    }
    //Rename a folder
    else if( data && (data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder") )
    {
        if( !project.ValidateObjectName(newName) )
        {
            wxRichToolTip tip(_("Invalid name"), project.GetBadObjectNameWarning());
            tip.SetIcon(wxICON_INFORMATION);
            tip.ShowFor(this);

            event.Veto();
            return;
        }

        bool global = data->GetString() == "GlobalObjectsFolder";
        gd::ClassWithObjects & objects = !global ? static_cast<gd::ClassWithObjects&>(layout) : project;

        if( !gd::ObjectsFolderHelper::RenameFolder( objects, data->GetSecondString(), newName ) )
        {
            wxRichToolTip tip(_("Invalid name"), "This folder already exists!");
            tip.SetIcon(wxICON_INFORMATION);
            tip.ShowFor(this);

            event.Veto();
            return;
        }

        event.Veto(); //Need to veto the event before refreshing to ignore it, avoiding a crash
        Refresh();
    }
    else
    {
        //Undo the item text change
        event.Veto();
        return;
    }

    renamedItemOldName = newName;

    project.SetDirty();
}

void ObjectsEditor::OnobjectsListSelectionChanged(wxTreeEvent& event)
{
    lastSelectedItem = event.GetItem();
    renamedItemOldName = objectsList->GetItemText(lastSelectedItem);

    mainFrameWrapper.SetRibbonPage(_("Objects"));
    ConnectEvents();

    //Get the selected item and update the ribbon
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));

    objectsRibbonBar->EnableButton(idRibbonUp, false);
    objectsRibbonBar->EnableButton(idRibbonDown, false);
    objectsRibbonBar->EnableButton(idRibbonDel, false);
    clipboardRibbonBar->EnableButton(idRibbonCopy, false);
    clipboardRibbonBar->EnableButton(idRibbonCut, false);
    clipboardRibbonBar->EnableButton(idRibbonPaste, false);
    selectionRibbonBar->EnableButton(idRibbonModProp, false);
    selectionRibbonBar->EnableButton(idRibbonModName, false);
    if ( data && (data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject") )
    {
        objectsList->SetToolTip( "" );
        objectsRibbonBar->EnableButton(idRibbonUp, true);
        objectsRibbonBar->EnableButton(idRibbonDown, true);
        objectsRibbonBar->EnableButton(idRibbonDel, true);
        clipboardRibbonBar->EnableButton(idRibbonCopy, true);
        clipboardRibbonBar->EnableButton(idRibbonCut, true);
        clipboardRibbonBar->EnableButton(idRibbonPaste, true);
        selectionRibbonBar->EnableButton(idRibbonModProp, true);
        selectionRibbonBar->EnableButton(idRibbonModName, true);
    }
    else if ( data && (data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup") )
    {
        objectsList->SetToolTip(_("Unfold the group to see the objects inside it."));
        objectsRibbonBar->EnableButton(idRibbonDel, true);
        clipboardRibbonBar->EnableButton(idRibbonCopy, true);
        clipboardRibbonBar->EnableButton(idRibbonCut, true);
        clipboardRibbonBar->EnableButton(idRibbonPaste, true);
        selectionRibbonBar->EnableButton(idRibbonModName, true);
    }
    else if ( data && (data->GetString() == "ObjectInGroup"))
    {
        objectsRibbonBar->EnableButton(idRibbonDel, true);
    }

    UpdateAssociatedPropertiesPanel();
}

gd::Object * ObjectsEditor::GetSelectedObject()
{
    gd::String objectName = objectsList->GetItemText(lastSelectedItem);
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    bool globalObject = data->GetString() == "GlobalObject";
    gd::Object * object = NULL;
    if ( globalObject && project.HasObjectNamed(objectName) )
        object = &project.GetObject(objectName);
    else if ( !globalObject && layout.HasObjectNamed(objectName) )
        object = &layout.GetObject(objectName);

    return object;
}

gd::ObjectGroup * ObjectsEditor::GetSelectedGroup()
{
    gd::String groupName = objectsList->GetItemText(lastSelectedItem);
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    bool globalGroup = data->GetString() == "GlobalGroup";

    std::vector<gd::ObjectGroup> & objectsGroups =
        globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

    std::vector<gd::ObjectGroup>::iterator it = std::find_if(objectsGroups.begin(), objectsGroups.end(),
        std::bind2nd(gd::GroupHasTheSameName(), groupName));

    return it != objectsGroups.end() ? &(*it) : NULL;
}

void ObjectsEditor::OnMenuEditObjectSelected(wxCommandEvent& event)
{
    //Get the selected item
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));

    //Object clicked?
    if ( data && (data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject") )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::Object * object = GetSelectedObject();
        if ( !object ) return;

        auto properties = object->GetProperties(project);
        if ( properties.empty() || properties.find("PLEASE_ALSO_SHOW_EDIT_BUTTON_THANKS") != properties.end() ) {

            //Open object editor
            object->EditObject(this, project, mainFrameWrapper);
            for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
                project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectEdited(project, globalObject ? NULL : &layout, *object);

            project.SetDirty();

            listsHelper.MakeObjectItem(objectsList, lastSelectedItem, *object, globalObject);

            //Reload resources : Do not forget to switch the working directory.
            wxString oldWorkingDir = wxGetCwd();
            if ( wxDirExists(wxFileName::FileName(project.GetProjectFile()).GetPath()))
                wxSetWorkingDirectory(wxFileName::FileName(project.GetProjectFile()).GetPath());

            object->LoadResources(project, layout);

            wxSetWorkingDirectory(oldWorkingDir);
        } else {
            //No object editor: open properties panel
            wxCommandEvent useless;
            OnMenuPropertiesSelected(useless);
        }
    }
    //Group clicked?
    else if ( data && (data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup") )
    {
        bool globalGroup = data->GetString() == "GlobalGroup";
        gd::ObjectGroup * group = GetSelectedGroup();
        objectsList->Expand(lastSelectedItem);

        if (!group) return;

        ChooseObjectDialog dialog(this, project, layout, /*canSelectGroup=*/false, "", /*multipleSelection=*/true);
        if (dialog.ShowModal() == 1) { //Add objects to the group
            for(std::size_t i = 0;i < dialog.GetChosenObjects().size();++i)
                group->AddObject(dialog.GetChosenObjects()[i]);
        }

        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupEdited(project, globalGroup ? NULL : &layout, group->GetName());

        listsHelper.MakeGroupItem(objectsList, lastSelectedItem, *group, globalGroup);
    }

    mainFrameWrapper.GetMainEditor()->SetFocus();
}

void ObjectsEditor::OnMenuPropertiesSelected(wxCommandEvent& event)
{
    if ( propPnl && propPnlManager ) {
        propPnlManager->GetPane("PROPERTIES").Show();
        propPnlManager->Update();
    }
}

void ObjectsEditor::OnMenuRenameSelected(wxCommandEvent& event)
{
    objectsList->EditLabel( lastSelectedItem );
}

void ObjectsEditor::OnAddObjectSelected(wxCommandEvent& event)
{
    gd::ChooseObjectTypeDialog chooseTypeDialog(this, project);
    if ( chooseTypeDialog.ShowModal() == 0 )
        return;

    gd::ObjectOrGroupFinder nameChecker(project, &layout);

    //Find a new unique name for the object
    gd::String name = gd::NewNameGenerator::Generate(_("NewObject"), [&nameChecker](const gd::String & name) {
        return nameChecker.HasObjectOrGroupNamed(name) != gd::ObjectOrGroupFinder::No;
    });

    //Add a new object of selected type to objects list
    gd::Object & object = layout.InsertNewObject(project, chooseTypeDialog.GetSelectedObjectType(),
        name, layout.GetObjectsCount());

    //And to the TreeCtrl
    wxTreeItemId itemAdded;
    wxTreeItemId lastLayoutObject = GetLastLayoutObjectItem();
    if(lastLayoutObject.IsOk())
        itemAdded = objectsList->InsertItem(objectsRootItem, lastLayoutObject, name);
    else
        itemAdded = objectsList->PrependItem(objectsRootItem, name);

    listsHelper.MakeObjectItem(objectsList, itemAdded, object, false);

    for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
        project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectAdded(project, &layout, object);

    objectsList->EditLabel(itemAdded);
    renamedItemOldName = name; //With wxGTK, calling EditLabel do not update renamedItemOldName with the name of the new object.

    gd::LogStatus( _( "The object was correctly added" ) );
    objectsList->Expand(objectsRootItem);
}

void ObjectsEditor::OnAddGroupSelected(wxCommandEvent& event)
{
    gd::ObjectGroup newGroup;

    vector<gd::ObjectGroup> & objectsGroups = layout.GetObjectGroups();

    gd::ObjectOrGroupFinder nameChecker(project, &layout);

    gd::String name = gd::NewNameGenerator::Generate(_("NewGroup"), [&nameChecker](const gd::String & name) {
        return nameChecker.HasObjectOrGroupNamed(name) != gd::ObjectOrGroupFinder::No;
    });

    newGroup.SetName(name);
    objectsGroups.push_back( newGroup );

    wxTreeItemId itemAdded;
    wxTreeItemId lastLayoutItem = GetLastLayoutGroupItem();
    if(lastLayoutItem.IsOk())
        itemAdded = objectsList->InsertItem(groupsRootItem, lastLayoutItem, name, 1);
    else
        itemAdded = objectsList->PrependItem(groupsRootItem, name, 1);

    objectsList->SetItemData( itemAdded, new gd::TreeItemStringData("LayoutGroup") );

    for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
        project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupAdded(project, &layout, name);
    gd::LogStatus( _( "The group was correctly added." ) );

    //Make sure that the group root item is expanded
    objectsList->Expand(groupsRootItem);
}


void ObjectsEditor::OnDeleteSelected(wxCommandEvent& event)
{
    wxArrayTreeItemIds selection;
    objectsList->GetSelections(selection);
    std::vector < gd::String > objectsDeleted;
    std::vector < gd::String > gObjectsDeleted;

    //Detect if only group's sub-objects have been selected (to avoid showing the next question)
    int objectsInGroupCount = 0;
    for(std::size_t i = 0;i<selection.GetCount();++i)
    {
        if (!selection[i].IsOk()) continue;
        gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(selection[i]));
        if (!data)
            continue;

        if(data->GetString() == "ObjectInGroup")
            objectsInGroupCount++;
    }

    int answer = -1;

    if(objectsInGroupCount != selection.GetCount())
    {
        answer = wxMessageBox(selection.GetCount() <= 1 ? wxString(_("Delete also all references to this item in groups and events ( i.e. Actions and conditions using the object )\?")) :
                                                          wxString::Format(wxString(_("Delete also all references to these %i items in groups and events ( i.e. Actions and conditions using the objects )\?")), selection.GetCount() - objectsInGroupCount),
                              _("Confirm deletion"), wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT);

        if ( answer == wxCANCEL ) return;
    }

    //Removing objects
    for (std::size_t i = 0;i<selection.GetCount();++i)
    {
        if (!selection[i].IsOk()) continue;
        gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(selection[i]));
        if (!data) continue;

        if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
        {
            bool globalObject = data->GetString() == "GlobalObject";
            gd::String objectName = objectsList->GetItemText( selection[i] );
            gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;
            //Generate also a list containing the names of the objects deleted :
            if ( globalObject ) gObjectsDeleted.push_back(objectName);
            else  objectsDeleted.push_back(objectName);

            if ( objects.HasObjectNamed(objectName) )
            {
                //Remove objects
                gd::ObjectsFolderHelper::RemoveObject( objects, objectName );

                if ( !globalObject)
                {
                    if ( answer == wxYES )
                    {
                        gd::EventsRefactorer::RemoveObjectInEvents(project.GetCurrentPlatform(), project, layout, layout.GetEvents(), objectName);
                        for (std::size_t g = 0;g<layout.GetObjectGroups().size();++g)
                        {
                            if ( layout.GetObjectGroups()[g].Find(objectName)) layout.GetObjectGroups()[g].RemoveObject(objectName);
                        }
                    }
                    layout.GetInitialInstances().RemoveInitialInstancesOfObject(objectName);
                    //TODO: Refactor also in external events
                }
                else if ( globalObject )
                {
                    //TODO: Refactor
                }
            }
        }
        else if ( data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup" )
        {
            bool globalGroup = data->GetString() == "GlobalGroup";
            gd::String groupName = objectsList->GetItemText( selection[i] );

            std::vector<gd::ObjectGroup> & objectsGroups =
                globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

            vector<gd::ObjectGroup>::iterator g = std::find_if( objectsGroups.begin(),
                                                            objectsGroups.end(),
                                                            std::bind2nd(gd::GroupHasTheSameName(), groupName));
            if ( g != objectsGroups.end() )
                objectsGroups.erase( g );

            if ( answer == wxYES )
            {
                if (!globalGroup)
                    gd::EventsRefactorer::RemoveObjectInEvents(project.GetCurrentPlatform(), project, layout, layout.GetEvents(), groupName);
                else
                    ;//TODO
            }

            for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
                project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupDeleted(project, globalGroup ? NULL : &layout, groupName);
        }
        else if( data->GetString() == "ObjectInGroup")
        {
            //Remove the object from its group
            gd::String objectName = objectsList->GetItemText( selection[i] );
            wxTreeItemId groupItem = objectsList->GetItemParent( selection[i] );
            if(!groupItem.IsOk())
                continue;

            gd::TreeItemStringData * groupData = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(groupItem));
            if (!groupData)
                continue;

            bool globalGroup = (groupData->GetString() == "GlobalGroup");
            gd::String groupName = objectsList->GetItemText( groupItem );

            std::vector<gd::ObjectGroup> & objectsGroups =
                globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

            vector<gd::ObjectGroup>::iterator g = std::find_if( objectsGroups.begin(),
                                                            objectsGroups.end(),
                                                            std::bind2nd(gd::GroupHasTheSameName(), groupName));
            if( g != objectsGroups.end() && g->Find(objectName))
            {
                g->RemoveObject(objectName);
            }
        }

        //Delete the item from the tree control
        objectsList->Delete(selection[i]);
    }

    //Update the groups items (without refreshing the entire tree control)
    wxTreeItemIdValue cookie;
    for(wxTreeItemId groupItem = objectsList->GetFirstChild(groupsRootItem, cookie); groupItem.IsOk(); groupItem = objectsList->GetNextChild(groupsRootItem, cookie))
    {
        UpdateGroup(groupItem);
    }

    //Call the notifiers
    for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
        project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectsDeleted(project, &layout, objectsDeleted);
    for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
        project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectsDeleted(project, NULL, gObjectsDeleted);
}

void ObjectsEditor::OnMoveupSelected(wxCommandEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;
    gd::String dataStr = data->GetString();
    gd::String dataStr2 = data->GetSecondString();
    gd::String name = objectsList->GetItemText( lastSelectedItem );

    //Object clicked?
    if ( data && (data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject") )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        gd::String folder = objects.GetObject(name).GetFolder();

        std::size_t positionInFolder = gd::ObjectsFolderHelper::GetObjectPositionInFolder( folder, objects, name );
        if( positionInFolder == 0 )
            return;

        objects.SwapObjects(
            gd::ObjectsFolderHelper::GetObjectAbsolutePosition( folder, objects, positionInFolder ),
            gd::ObjectsFolderHelper::GetObjectAbsolutePosition( folder, objects, positionInFolder - 1 ) );
    }
    else if( data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" )
    {
        bool globalObject = data->GetString() == "GlobalObjectsFolder";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        if( objectsList->GetPrevSibling( lastSelectedItem ).IsOk() )
            gd::ObjectsFolderHelper::OffsetFolder( objects, data->GetSecondString(), -1 );
    }

    Refresh();
    SelectItem(objectsRootItem, name, dataStr, dataStr2); //Select again the moved item
}

void ObjectsEditor::OnMoveDownSelected(wxCommandEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;
    gd::String dataStr = data->GetString();
    gd::String dataStr2 = data->GetSecondString();
    gd::String name = objectsList->GetItemText( lastSelectedItem );

    //Object clicked?
    if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        gd::String folder = objects.GetObject(name).GetFolder();

        std::size_t positionInFolder = gd::ObjectsFolderHelper::GetObjectPositionInFolder( folder, objects, name );
        if( positionInFolder >= gd::ObjectsFolderHelper::GetObjectsCount( folder, objects ) - 1 )
            return;

        objects.SwapObjects(
            gd::ObjectsFolderHelper::GetObjectAbsolutePosition( folder, objects, positionInFolder ),
            gd::ObjectsFolderHelper::GetObjectAbsolutePosition( folder, objects, positionInFolder + 1 ) );
    }
    else if( data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" )
    {
        bool globalObject = data->GetString() == "GlobalObjectsFolder";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        gd::ObjectsFolderHelper::OffsetFolder( objects, data->GetSecondString(), 1 );
    }

    Refresh();
    SelectItem(objectsRootItem, name, dataStr, dataStr2); //Select again the moved item
}

bool ObjectsEditor::SelectItem(wxTreeItemId parent, gd::String name, gd::String dataStr1, gd::String dataStr2)
{
    wxTreeItemId item = objectsList->GetLastChild(parent);
    while ( item.IsOk() )
    {
        gd::TreeItemStringData * itemData = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(item));
        if (itemData)
        {
            if (objectsList->GetItemText( item ) == name
                && itemData->GetString() == dataStr1
                && itemData->GetSecondString() == dataStr2)
            {
                objectsList->SelectItem(item);
                return true;
            }
            else
            {
                if(SelectItem(item, name, dataStr1, dataStr2))
                    return true; // We found the item as a child of the current one
            }
        }
        item = objectsList->GetPrevSibling(item);
    }

    return false;
}

void ObjectsEditor::OnCopySelected(wxCommandEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;
    gd::String name = objectsList->GetItemText( lastSelectedItem );

    //Object clicked?
    if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        if ( !objects.HasObjectNamed(name) )
            return;

        gd::Clipboard::Get()->SetObject(objects.GetObject(name));
        gd::Clipboard::Get()->ForgetObjectGroup();
    }
    else if ( data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup" )
    {
        bool globalGroup = data->GetString() == "GlobalGroup";
        std::vector<gd::ObjectGroup> & objectsGroups =
            globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

        if ( !HasGroupNamed(name, objectsGroups) )
            return;

        gd::Clipboard::Get()->SetObjectGroup(GetGroup(name, objectsGroups));
        gd::Clipboard::Get()->ForgetObject();
    }
}

void ObjectsEditor::OnCutSelected(wxCommandEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;
    gd::String name = objectsList->GetItemText( lastSelectedItem );

    //Object clicked?
    if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        if ( !objects.HasObjectNamed(name) )
            return;

        gd::Clipboard::Get()->SetObject(objects.GetObject(name));
        gd::Clipboard::Get()->ForgetObjectGroup();

        objects.RemoveObject(name);

        std::vector<gd::String> objectsDeleted;
        objectsDeleted.push_back(name);
        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectsDeleted(project, globalObject ? NULL : &layout, objectsDeleted);
    }
    else if ( data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup" )
    {
        bool globalGroup = data->GetString() == "GlobalGroup";
        std::vector<gd::ObjectGroup> & objectsGroups =
            globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

        if ( !HasGroupNamed(name, objectsGroups) )
            return;

        gd::Clipboard::Get()->SetObjectGroup(GetGroup(name, objectsGroups));
        gd::Clipboard::Get()->ForgetObject();

        RemoveGroup(name, objectsGroups);
        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupDeleted(project, globalGroup ? NULL : &layout, name);
    }

    Refresh();
}

void ObjectsEditor::OnPasteSelected(wxCommandEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;
    gd::Clipboard * clipboard = gd::Clipboard::Get();

    gd::ObjectOrGroupFinder nameChecker(project, &layout);

    if ( clipboard->HasObject() )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::ClassWithObjects & objects = !globalObject ? static_cast<gd::ClassWithObjects&>(layout) : project;

        //Add a new object of selected type to objects list
        std::unique_ptr<gd::Object> object = clipboard->GetObject();
        object->SetName(gd::NewNameGenerator::Generate(object->GetName(), _("CopyOf"), [&nameChecker, globalObject](const gd::String & name) {
            return nameChecker.HasObjectOrGroupNamed(
                name, globalObject /* Only search other layouts if it's a global object */) != gd::ObjectOrGroupFinder::No;
        }));

        //Add it to the list
        objects.InsertObject(*object, objects.GetObjectsCount());
        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectAdded(project, globalObject ? NULL : &layout, *object);
    }
    else if ( clipboard->HasObjectGroup())
    {
        bool globalGroup = data->GetString() == "GlobalGroup";
        std::vector<gd::ObjectGroup> & objectsGroups =
            globalGroup ? project.GetObjectGroups() : layout.GetObjectGroups();

        gd::ObjectGroup groupPasted = clipboard->GetObjectGroup();

        groupPasted.SetName(gd::NewNameGenerator::Generate(groupPasted.GetName(), _("CopyOf"), [&nameChecker, globalGroup](const gd::String & name) {
            return nameChecker.HasObjectOrGroupNamed(name, globalGroup /* Only search other layouts if it's a global object */) != gd::ObjectOrGroupFinder::No;
        }));
        objectsGroups.push_back( groupPasted );

        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupAdded(project, globalGroup ? NULL : &layout,  groupPasted.GetName());
    }

    //Refresh the list and select the newly added item
    searchCtrl->Clear();
    Refresh();
}

void ObjectsEditor::OnsearchCtrlText(wxCommandEvent& event)
{
    Refresh();
}

void ObjectsEditor::OnSetGlobalSelected(wxCommandEvent& event)
{
    //Get the selected item
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;

    gd::ObjectOrGroupFinder nameChecker(project, &layout);

    //Object clicked?
    if ( data->GetString() == "LayoutObject")
    {
        gd::Object * object = GetSelectedObject();
        if ( !object ) return;

        gd::String objectName = object->GetName();
        gd::String secondData = data->GetSecondString();

        unsigned int searchSameNames = nameChecker.HasObjectOrGroupNamed(objectName, true);
        if ( searchSameNames != gd::ObjectOrGroupFinder::No && ((searchSameNames & gd::ObjectOrGroupFinder::InLayout) != searchSameNames) )
        //Test if there is a global object/group or an object/group in another layout with the same name
        //Indeed the object in the same layout with the same name is not taken into account because it's the object we want to set global.
        {
            gd::String errorMessage = _("Can't set \"") + objectName + _("\" global because :\n") +
                GetExistingObjectsErrorMessage(searchSameNames & ~gd::ObjectOrGroupFinder::InLayout, nameChecker.GetLayoutsWithSameObjectName() );
            gd::LogWarning(errorMessage);
            return;
        }

        project.InsertObject(*object, -1);
        layout.RemoveObject(objectName);

        std::vector<gd::String> removedObject;
        removedObject.push_back(objectName);
        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
        {
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectsDeleted(project, &layout, removedObject);
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectAdded(project, NULL, project.GetObject(objectName));
        }

        Refresh();

        // Select the globalized object (now it's first string data is "GlobalObject")
        SelectItem(objectsRootItem, objectName, "GlobalObject", secondData);
    }
    //Group clicked?
    else if ( data->GetString() == "LayoutGroup" )
    {
        gd::ObjectGroup * group = GetSelectedGroup();
        if ( !group ) return;
        gd::String groupName = group->GetName();

        int searchSameNames = nameChecker.HasObjectOrGroupNamed(groupName, true);
        if ( searchSameNames != gd::ObjectOrGroupFinder::No && ((searchSameNames & gd::ObjectOrGroupFinder::InLayout) != searchSameNames) )
        //Test if there is a global object/group or an object/group in another layout with the same name
        //Indeed the object in the same layout with the same name is not taken into account because it's the object we want to set global.
        {
            gd::String errorMessage = _("Can't set \"") + groupName + _("\" global because :\n") +
                GetExistingObjectsErrorMessage(searchSameNames & ~gd::ObjectOrGroupFinder::InLayout, nameChecker.GetLayoutsWithSameObjectName() );
            gd::LogWarning(errorMessage);

            return;
        }

        project.GetObjectGroups().push_back(*group);
        RemoveGroup(groupName, layout.GetObjectGroups());

        for ( std::size_t j = 0; j < project.GetUsedPlatforms().size();++j)
        {
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupDeleted(project, &layout, groupName);
            project.GetUsedPlatforms()[j]->GetChangesNotifier().OnObjectGroupAdded(project, NULL, groupName);
        }

        //Add the item corresponding to the global group
        bool wasExpanded = objectsList->IsExpanded(lastSelectedItem);
        objectsList->Delete(lastSelectedItem);

        wxTreeItemId newItem;
        wxTreeItemId lastGlobalItem = GetLastGlobalGroupItem();
        if(lastGlobalItem.IsOk())
            newItem = objectsList->InsertItem(groupsRootItem, lastGlobalItem, groupName);
        else
            newItem = objectsList->AppendItem(groupsRootItem, groupName);

        auto it = std::find_if( project.GetObjectGroups().begin(), project.GetObjectGroups().end(), std::bind2nd(gd::GroupHasTheSameName(), groupName));
        if (it != project.GetObjectGroups().end()) listsHelper.MakeGroupItem(objectsList, newItem, *it, true);
        objectsList->SelectItem(newItem);

        if(wasExpanded) //Expand the group item if it was expanded before
            objectsList->Expand(newItem);
    }
}

void ObjectsEditor::OnSetFocus(wxFocusEvent& event)
{
    mainFrameWrapper.SetRibbonPage(_("Objects"));
    ConnectEvents();

    if ( objectsList->GetFocusedItem().IsOk() )
    {
        lastSelectedItem = objectsList->GetFocusedItem();
        UpdateAssociatedPropertiesPanel();
    }
}

void ObjectsEditor::OnobjectsListBeginDrag(wxTreeEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;

    if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
    {
        if(data->GetString() == "LayoutObject")
            objectsList->SetItemText(objectsRootItem, _("Create a new folder"));
        else
            objectsList->SetItemText(globalObjectsRootItem, _("Create a new folder"));
        objectsList->SetItemText(groupsRootItem, _("Create a new group"));
        objectsList->SetItemText(globalGroupsRootItem, _("Create a new global group"));

        gd::Object * object = GetSelectedObject();
        if ( !object ) return;

        wxTextDataObject objectName("OBJ" + object->GetName());
        wxDropSource dragSource( this );
        dragSource.SetData( objectName );
        dragSource.DoDragDrop( true );

        objectsList->SetItemText(objectsRootItem, _("Objects"));
        objectsList->SetItemText(globalObjectsRootItem, _("Global objects"));
        objectsList->SetItemText(groupsRootItem, _("Groups"));
        objectsList->SetItemText(globalGroupsRootItem, _("Global groups"));

        Refresh();
    }
    else if ( data->GetString() == "ObjectsFolder" || data->GetString() == "GlobalObjectsFolder" )
    {
        wxTextDataObject folderName("FOL" + ( data->GetString() == "ObjectsFolder" ? gd::String("LAY") : gd::String("GLO") ) + data->GetSecondString());
        wxDropSource dragSource( this );
        dragSource.SetData( folderName );
        dragSource.DoDragDrop( true );

        Refresh();
    }
}

void ObjectsEditor::UpdateAssociatedPropertiesPanel()
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;

    if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::Object * object = GetSelectedObject();
        if ( !object ) return;

        //Log the selection
        if (!globalObject)
            LogFileManager::Get()->WriteToLogFile("Object \""+object->GetName()+"\" selected ( Layout \""+layout.GetName()+"\" )");
        else
            LogFileManager::Get()->WriteToLogFile("Object \""+object->GetName()+"\" selected");

        //Notify other editors of the selection of the object
        if ( propPnl ) propPnl->SelectedObject(object);
    }
    else
        if ( propPnl ) propPnl->SelectedObject(NULL);

    if ( data && (data->GetString() == "GlobalGroup" || data->GetString() == "LayoutGroup") )
    {
        //TODO: Use the associated property grid for groups
        if ( propPnl ) propPnl->SelectedObject(NULL);
    }
}

wxTreeItemId ObjectsEditor::GetLastLayoutObjectItem() const
{
    return objectsList->GetLastChild( objectsRootItem );
}

wxTreeItemId ObjectsEditor::GetLastGlobalObjectItem() const
{
    return objectsList->GetLastChild( globalObjectsRootItem );
}

wxTreeItemId ObjectsEditor::GetLastLayoutGroupItem() const
{
    return objectsList->GetLastChild( groupsRootItem );
}

wxTreeItemId ObjectsEditor::GetLastGlobalGroupItem() const
{
    return objectsList->GetLastChild( globalGroupsRootItem );
}

bool ObjectsEditor::HasGroupNamed(gd::String name, const std::vector<gd::ObjectGroup> & groups) const
{
    return std::find_if( groups.begin(), groups.end(), std::bind2nd(gd::GroupHasTheSameName(), name))
            != groups.end();
}

gd::ObjectGroup & ObjectsEditor::GetGroup(gd::String name, std::vector<gd::ObjectGroup> & groups)
{
    return *std::find_if( groups.begin(), groups.end(), std::bind2nd(gd::GroupHasTheSameName(), name));
}

void ObjectsEditor::RemoveGroup(gd::String name, std::vector<gd::ObjectGroup> & groups)
{
    groups.erase(std::remove_if(groups.begin(), groups.end(), std::bind2nd(gd::GroupHasTheSameName(), name)), groups.end());
}

gd::String ObjectsEditor::GetExistingObjectsErrorMessage(unsigned int nameCheckResult, const std::vector<gd::String> &layoutsWithSameName) const
{
    gd::String errorMessage;

    if((nameCheckResult & gd::ObjectOrGroupFinder::InLayout) != 0)
    {
        errorMessage += " - ";

        if((nameCheckResult & gd::ObjectOrGroupFinder::AsObjectInLayout) != 0)
            errorMessage += _("an object") + " ";
        else
            errorMessage += _("a group") + " ";

        errorMessage += _("with the same name exists in the current scene");
    }
    if((nameCheckResult & gd::ObjectOrGroupFinder::InAnotherLayout) != 0)
    {
        if(!errorMessage.empty())
            errorMessage += "\n";

        errorMessage += " - ";

        if((nameCheckResult & gd::ObjectOrGroupFinder::AsObjectInAnotherLayout) != 0)
            errorMessage += _("objects") + " ";
        if((nameCheckResult & gd::ObjectOrGroupFinder::AsGroupInAnotherLayout) != 0)
            errorMessage += ((nameCheckResult & gd::ObjectOrGroupFinder::AsObjectInAnotherLayout) != 0) ?
                _("/ groups") :
                _("groups") + " ";

        errorMessage += _("with the same name exist in these scenes : ");

        for(auto it = layoutsWithSameName.begin(); it != layoutsWithSameName.end(); ++it)
        {
            errorMessage += "\n    * " + (*it);
        }
    }
    if((nameCheckResult & gd::ObjectOrGroupFinder::InGlobal) != 0)
    {
        if(!errorMessage.empty())
            errorMessage += "\n";

        errorMessage += " - ";

        if((nameCheckResult & gd::ObjectOrGroupFinder::AsGlobalObject) != 0)
            errorMessage += _("a global object with the same name exists");
        else
            errorMessage += _("a global group with the same name exists");
    }

    return errorMessage;
}

void ObjectsEditor::OnMenuAddBehaviorSelected(wxCommandEvent& event)
{
    gd::TreeItemStringData * data = dynamic_cast<gd::TreeItemStringData*>(objectsList->GetItemData(lastSelectedItem));
    if (!data) return;

    if ( data->GetString() == "GlobalObject" || data->GetString() == "LayoutObject" )
    {
        bool globalObject = data->GetString() == "GlobalObject";
        gd::Object * object = GetSelectedObject();
        if ( !object ) return;

        if ( gd::ChooseBehaviorTypeDialog::ChooseAndAddBehaviorToObject(this, project,
            object, &layout, globalObject))
            UpdateAssociatedPropertiesPanel();

        //Show and update the properties panel.
        if ( propPnl && propPnlManager ) {
            propPnlManager->GetPane("PROPERTIES").Show();
            propPnlManager->Update();
        }
    }
    /*else: No object is selected, do nothing.*/
}

}
