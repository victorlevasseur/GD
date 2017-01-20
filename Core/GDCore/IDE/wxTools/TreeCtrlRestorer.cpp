#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include "GDCore/IDE/wxTools/TreeCtrlRestorer.h"

namespace gd
{

TreeCtrlRestorer::TreeCtrlRestorer(ItemHashFunction hashFunction) :
    hashFunction(hashFunction)
{

}

void TreeCtrlRestorer::SaveState(const wxTreeCtrl * treeCtrl)
{
    treeRoot.expanded = false;
    treeRoot.selected = false;
    treeRoot.children.clear();

    horizontalScrollPos = treeCtrl->GetScrollPos( wxHORIZONTAL );
    verticalScrollPos = treeCtrl->GetScrollPos( wxVERTICAL );

    SaveItemState( treeCtrl, treeCtrl->GetRootItem(), treeRoot );
}

void TreeCtrlRestorer::RestoreState(wxTreeCtrl * treeCtrl) const
{
    if( !treeCtrl->GetRootItem().IsOk() )
        return;

    RestoreItemState( treeCtrl, treeCtrl->GetRootItem(), treeRoot );

    treeCtrl->SetScrollPos( wxHORIZONTAL, horizontalScrollPos );
    treeCtrl->SetScrollPos( wxVERTICAL, verticalScrollPos );

    wxArrayTreeItemIds selection;
    if( treeCtrl->GetSelections( selection ) == 1 )
    {
        treeCtrl->ScrollTo( selection[0] );
    }
}

void TreeCtrlRestorer::SaveItemState(const wxTreeCtrl * treeCtrl, wxTreeItemId item, TreeCtrlRestorerItem & parentRestoredItem)
{
    std::size_t itemHash = hashFunction( treeCtrl, item );

    TreeCtrlRestorerItem & itemRestoredItem = parentRestoredItem.children[itemHash];
    itemRestoredItem.expanded = treeCtrl->IsExpanded( item ) || !treeCtrl->ItemHasChildren( item ); // To expand items that may newly contain a new child item
    itemRestoredItem.selected = treeCtrl->IsSelected( item );

    wxTreeItemIdValue cookie;
    for( wxTreeItemId childItem = treeCtrl->GetFirstChild(item, cookie); childItem.IsOk(); childItem = treeCtrl->GetNextChild(item, cookie) )
    {
        SaveItemState( treeCtrl, childItem, itemRestoredItem );
    }
}

void TreeCtrlRestorer::RestoreItemState(wxTreeCtrl * treeCtrl, wxTreeItemId item, const TreeCtrlRestorerItem & parentRestoredItem) const
{
    std::size_t itemHash = hashFunction( treeCtrl, item );

    if( parentRestoredItem.children.count( itemHash ) > 0 )
    {
        const TreeCtrlRestorerItem & itemRestoredItem = parentRestoredItem.children.at( itemHash );
        if( itemRestoredItem.expanded && !treeCtrl->IsExpanded( item ) )
            treeCtrl->Expand( item );
        else if( !itemRestoredItem.expanded && treeCtrl->IsExpanded( item ) )
            treeCtrl->Collapse( item );

        if( treeCtrl->IsSelected( item ) != itemRestoredItem.selected )
            treeCtrl->SelectItem( item, itemRestoredItem.selected );

        wxTreeItemIdValue cookie;
        for( wxTreeItemId childItem = treeCtrl->GetFirstChild(item, cookie); childItem.IsOk(); childItem = treeCtrl->GetNextChild(item, cookie) )
        {
            RestoreItemState( treeCtrl, childItem, itemRestoredItem );
        }
    }
}

}

#endif
