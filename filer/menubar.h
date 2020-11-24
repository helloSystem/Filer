#pragma once

#include <QMenuBar>

namespace Filer
{

class MenuBar : public QMenuBar
{
    Q_OBJECT

    friend class MainWindow;
    friend class DesktopWindow;
public:
    explicit MenuBar(QWidget *parent = Q_NULLPTR);

   void retranslateUi() noexcept;

private:
    QAction *actionGoUp;
    QAction *actionHome;
    QAction *actionReload;
    QAction *actionGo;
    QAction *actionQuit;
    QAction *actionAbout;
    QAction *actionNewWin;
    QAction *actionShowHidden;
    QAction *actionComputer;
    QAction *actionTrash;
    QAction *actionNetwork;
    QAction *actionDesktop;
    QAction *actionAddToBookmarks;
    QAction *actionApplications;
    QAction *actionReload_2;
    QAction *actionIconView;
    QAction *actionCompactView;
    QAction *actionDetailedList;
    QAction *actionThumbnailView;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionSelectAll;
    QAction *actionPreferences;
    QAction *actionAscending;
    QAction *actionDescending;
    QAction *actionByFileName;
    QAction *actionByMTime;
    QAction *actionByFileType;
    QAction *actionByOwner;
    QAction *actionFolderFirst;
    QAction *actionNewTab;
    QAction *actionGoBack;
    QAction *actionGoForward;
    QAction *actionInvertSelection;
    QAction *actionDelete;
    QAction *actionRename;
    QAction *actionCloseTab;
    QAction *actionFileProperties;
    QAction *actionFolderProperties;
    QAction *actionCaseSensitive;
    QAction *actionByFileSize;
    QAction *actionCloseWindow;
    QAction *actionEdit_Bookmarks;
    QAction *actionOpenTerminal;
    QAction *actionOpenAsRoot;
    QAction *actionEditBookmarks;
    QAction *actionNewFolder;
    QAction *actionNewBlankFile;
    QAction *actionFindFiles;
    QAction *actionFilter;
    QMenu *menu_File;
    QMenu *menuCreateNew;
    QMenu *menu_Help;
    QMenu *menu_View;
    QMenu *menuSorting;
    QMenu *menu_Editw;
    QMenu *menu_Bookmarks;
    QMenu *menu_Go;
    QMenu *menu_Tool;
};

} // namespace Filer

