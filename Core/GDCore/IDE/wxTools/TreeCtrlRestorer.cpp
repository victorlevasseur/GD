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
    treeRoot.children.clear();

    SaveItemState( treeCtrl, treeCtrl->GetRootItem(), treeRoot );
}

void TreeCtrlRestorer::RestoreState(wxTreeCtrl * treeCtrl) const
{
    if( !treeCtrl->GetRootItem().IsOk() )
        return;

    RestoreItemState( treeCtrl, treeCtrl->GetRootItem(), treeRoot );
}

void TreeCtrlRestorer::SaveItemState(const wxTreeCtrl * treeCtrl, wxTreeItemId item, TreeCtrlRestorerItem & parentRestoredItem)
{
    std::size_t itemHash = hashFunction( treeCtrl, item );

    TreeCtrlRestorerItem & itemRestoredItem = parentRestoredItem.children[itemHash];
    itemRestoredItem.expanded = treeCtrl->IsExpanded( item );

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
        if( itemRestoredItem.expanded )
            treeCtrl->Expand( item );

        wxTreeItemIdValue cookie;
        for( wxTreeItemId childItem = treeCtrl->GetFirstChild(item, cookie); childItem.IsOk(); childItem = treeCtrl->GetNextChild(item, cookie) )
        {
            RestoreItemState( treeCtrl, childItem, itemRestoredItem );
        }
    }
}

}
