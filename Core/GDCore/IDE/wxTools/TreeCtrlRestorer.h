/*
 * GDevelop Core
 * Copyright 2016 Victor Levasseur (victorlevasseur52@gmail.com).
 * This project is released under the MIT License.
 */

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#ifndef TREECTRLRESTORER_H
#define TREECTRLRESTORER_H

#include <functional>
#include <map>

#include <wx/treectrl.h>

namespace gd
{

struct TreeCtrlRestorerItem
{
    bool expanded;
    bool selected;
    std::map<std::size_t, TreeCtrlRestorerItem> children;
};

class TreeCtrlRestorer
{
public:
    using ItemHashFunction = std::function<std::size_t(const wxTreeCtrl*, wxTreeItemId)>;

    TreeCtrlRestorer(ItemHashFunction hashFunction);

    void SaveState(const wxTreeCtrl * treeCtrl);

    void RestoreState(wxTreeCtrl * treeCtrl) const;

private:
    void SaveItemState(const wxTreeCtrl * treeCtrl, wxTreeItemId item, TreeCtrlRestorerItem & parentRestoredItem);

    void RestoreItemState(wxTreeCtrl * treeCtrl, wxTreeItemId item, const TreeCtrlRestorerItem & parentRestoredItem) const;

    ItemHashFunction hashFunction;

    TreeCtrlRestorerItem treeRoot;
};

}

#endif
#endif
