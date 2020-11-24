#include <QCoreApplication>

#include "application.h"
#include "menubar.h"

namespace Filer {

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent)
{
    setObjectName(QString::fromUtf8("menubar"));
    setMinimumHeight(24);
    setMinimumWidth(400);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    Settings& settings = static_cast<Application*>(qApp)->settings();

    actionGoUp = new QAction(this);
    actionGoUp->setObjectName(QStringLiteral("actionGoUp"));
    QIcon icon1;
    QString iconThemeName = QStringLiteral("go-up");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon1 = QIcon::fromTheme(iconThemeName);
    } else {
        icon1.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionGoUp->setIcon(icon1);

    actionHome = new QAction(this);
    actionHome->setObjectName(QStringLiteral("actionHome"));
    QIcon icon2;
    iconThemeName = QStringLiteral("user-home");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon2 = QIcon::fromTheme(iconThemeName);
    } else {
        icon2.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionHome->setIcon(icon2);

    actionReload = new QAction(this);
    actionReload->setObjectName(QStringLiteral("actionReload"));
    QIcon icon3;
    iconThemeName = QStringLiteral("view-refresh");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon3 = QIcon::fromTheme(iconThemeName);
    } else {
        icon3.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionReload->setIcon(icon3);
    actionReload->setVisible(true);

    actionGo = new QAction(this);
    actionGo->setObjectName(QStringLiteral("actionGo"));
    QIcon icon4;
    iconThemeName = QStringLiteral("go-jump");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon4 = QIcon::fromTheme(iconThemeName);
    } else {
        icon4.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionGo->setIcon(icon4);
    actionGo->setVisible(false);

    actionQuit = new QAction(this);
    actionQuit->setObjectName(QStringLiteral("actionQuit"));
    QIcon icon5;
    iconThemeName = QStringLiteral("application-exit");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon5 = QIcon::fromTheme(iconThemeName);
    } else {
        icon5.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionQuit->setIcon(icon5);

    actionAbout = new QAction(this);
    actionAbout->setObjectName(QStringLiteral("actionAbout"));
    QIcon icon6;
    iconThemeName = QStringLiteral("help-about");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon6 = QIcon::fromTheme(iconThemeName);
    } else {
        icon6.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionAbout->setIcon(icon6);

    actionNewWin = new QAction(this);
    actionNewWin->setObjectName(QStringLiteral("actionNewWin"));
    QIcon icon7;
    iconThemeName = QStringLiteral("window-new");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon7 = QIcon::fromTheme(iconThemeName);
    } else {
        icon7.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionNewWin->setIcon(icon7);

    actionShowHidden = new QAction(this);
    actionShowHidden->setObjectName(QStringLiteral("actionShowHidden"));
    actionShowHidden->setCheckable(true);

    actionComputer = new QAction(this);
    actionComputer->setObjectName(QStringLiteral("actionComputer"));
    QIcon icon8;
    iconThemeName = QStringLiteral("computer");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon8 = QIcon::fromTheme(iconThemeName);
    } else {
        icon8.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionComputer->setIcon(icon8);

    actionTrash = new QAction(this);
    actionTrash->setObjectName(QStringLiteral("actionTrash"));
    QIcon icon9;
    iconThemeName = QStringLiteral("user-trash");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon9 = QIcon::fromTheme(iconThemeName);
    } else {
        icon9.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionTrash->setIcon(icon9);
    actionTrash->setVisible(settings.supportTrash());

    actionNetwork = new QAction(this);
    actionNetwork->setObjectName(QStringLiteral("actionNetwork"));

    actionDesktop = new QAction(this);
    actionDesktop->setObjectName(QStringLiteral("actionDesktop"));
    QIcon icon10;
    iconThemeName = QStringLiteral("user-desktop");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon10 = QIcon::fromTheme(iconThemeName);
    } else {
        icon10.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionDesktop->setIcon(icon10);

    actionAddToBookmarks = new QAction(this);
    actionAddToBookmarks->setObjectName(QStringLiteral("actionAddToBookmarks"));
    QIcon icon11;
    iconThemeName = QStringLiteral("list-add");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon11 = QIcon::fromTheme(iconThemeName);
    } else {
        icon11.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionAddToBookmarks->setIcon(icon11);

    actionApplications = new QAction(this);
    actionApplications->setObjectName(QStringLiteral("actionApplications"));
    actionApplications->setVisible(false);

    actionReload_2 = new QAction(this);
    actionReload_2->setObjectName(QStringLiteral("actionReload_2"));

    actionIconView = new QAction(this);
    actionIconView->setObjectName(QStringLiteral("actionIconView"));
    actionIconView->setCheckable(true);

    actionCompactView = new QAction(this);
    actionCompactView->setObjectName(QStringLiteral("actionCompactView"));
    actionCompactView->setCheckable(true);

    actionDetailedList = new QAction(this);
    actionDetailedList->setObjectName(QStringLiteral("actionDetailedList"));
    actionDetailedList->setCheckable(true);

    actionThumbnailView = new QAction(this);
    actionThumbnailView->setObjectName(QStringLiteral("actionThumbnailView"));
    actionThumbnailView->setCheckable(true);

    actionCut = new QAction(this);
    actionCut->setObjectName(QStringLiteral("actionCut"));
    QIcon icon12;
    iconThemeName = QStringLiteral("edit-cut");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon12 = QIcon::fromTheme(iconThemeName);
    } else {
        icon12.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionCut->setIcon(icon12);

    actionCopy = new QAction(this);
    actionCopy->setObjectName(QStringLiteral("actionCopy"));
    QIcon icon13;
    iconThemeName = QStringLiteral("edit-copy");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon13 = QIcon::fromTheme(iconThemeName);
    } else {
        icon13.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionCopy->setIcon(icon13);

    actionPaste = new QAction(this);
    actionPaste->setObjectName(QStringLiteral("actionPaste"));
    QIcon icon14;
    iconThemeName = QStringLiteral("edit-paste");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon14 = QIcon::fromTheme(iconThemeName);
    } else {
        icon14.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionPaste->setIcon(icon14);

    actionSelectAll = new QAction(this);
    actionSelectAll->setObjectName(QStringLiteral("actionSelectAll"));

    actionPreferences = new QAction(this);
    actionPreferences->setObjectName(QStringLiteral("actionPreferences"));

    actionAscending = new QAction(this);
    actionAscending->setObjectName(QStringLiteral("actionAscending"));
    actionAscending->setCheckable(true);

    actionDescending = new QAction(this);
    actionDescending->setObjectName(QStringLiteral("actionDescending"));
    actionDescending->setCheckable(true);

    actionByFileName = new QAction(this);
    actionByFileName->setObjectName(QStringLiteral("actionByFileName"));
    actionByFileName->setCheckable(true);

    actionByMTime = new QAction(this);
    actionByMTime->setObjectName(QStringLiteral("actionByMTime"));
    actionByMTime->setCheckable(true);

    actionByFileType = new QAction(this);
    actionByFileType->setObjectName(QStringLiteral("actionByFileType"));
    actionByFileType->setCheckable(true);

    actionByOwner = new QAction(this);
    actionByOwner->setObjectName(QStringLiteral("actionByOwner"));
    actionByOwner->setCheckable(true);

    actionFolderFirst = new QAction(this);
    actionFolderFirst->setObjectName(QStringLiteral("actionFolderFirst"));
    actionFolderFirst->setCheckable(true);

    actionNewTab = new QAction(this);
    actionNewTab->setObjectName(QStringLiteral("actionNewTab"));
    actionNewTab->setIcon(icon7);
    actionNewTab->setVisible(false);

    actionGoBack = new QAction(this);
    actionGoBack->setObjectName(QStringLiteral("actionGoBack"));
    QIcon icon15;
    iconThemeName = QStringLiteral("go-previous");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon15 = QIcon::fromTheme(iconThemeName);
    } else {
        icon15.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionGoBack->setIcon(icon15);

    actionGoForward = new QAction(this);
    actionGoForward->setObjectName(QStringLiteral("actionGoForward"));
    QIcon icon16;
    iconThemeName = QStringLiteral("go-next");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon16 = QIcon::fromTheme(iconThemeName);
    } else {
        icon16.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionGoForward->setIcon(icon16);
    actionGoForward->setVisible(true);
    actionGoForward->setIconVisibleInMenu(true);
    actionGoForward->setShortcutVisibleInContextMenu(true);

    actionInvertSelection = new QAction(this);
    actionInvertSelection->setObjectName(QStringLiteral("actionInvertSelection"));

    actionDelete = new QAction(this);
    actionDelete->setObjectName(QStringLiteral("actionDelete"));
    QIcon icon17;
    iconThemeName = QStringLiteral("edit-delete");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon17 = QIcon::fromTheme(iconThemeName);
    } else {
        icon17.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionDelete->setIcon(icon17);

    actionRename = new QAction(this);
    actionRename->setObjectName(QStringLiteral("actionRename"));

    actionCloseTab = new QAction(this);
    actionCloseTab->setObjectName(QStringLiteral("actionCloseTab"));

    actionFileProperties = new QAction(this);
    actionFileProperties->setObjectName(QStringLiteral("actionFileProperties"));

    actionFolderProperties = new QAction(this);
    actionFolderProperties->setObjectName(QStringLiteral("actionFolderProperties"));

    actionCaseSensitive = new QAction(this);
    actionCaseSensitive->setObjectName(QStringLiteral("actionCaseSensitive"));
    actionCaseSensitive->setCheckable(true);

    actionByFileSize = new QAction(this);
    actionByFileSize->setObjectName(QStringLiteral("actionByFileSize"));
    actionByFileSize->setCheckable(true);

    actionCloseWindow = new QAction(this);
    actionCloseWindow->setObjectName(QStringLiteral("actionCloseWindow"));

    actionEdit_Bookmarks = new QAction(this);
    actionEdit_Bookmarks->setObjectName(QStringLiteral("actionEdit_Bookmarks"));

    actionOpenTerminal = new QAction(this);
    actionOpenTerminal->setObjectName(QStringLiteral("actionOpenTerminal"));

    actionOpenAsRoot = new QAction(this);
    actionOpenAsRoot->setObjectName(QStringLiteral("actionOpenAsRoot"));

    actionEditBookmarks = new QAction(this);
    actionEditBookmarks->setObjectName(QStringLiteral("actionEditBookmarks"));

    actionNewFolder = new QAction(this);
    actionNewFolder->setObjectName(QStringLiteral("actionNewFolder"));
    QIcon icon18;
    iconThemeName = QStringLiteral("folder-new");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon18 = QIcon::fromTheme(iconThemeName);
    } else {
        icon18.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionNewFolder->setIcon(icon18);

    actionNewBlankFile = new QAction(this);
    actionNewBlankFile->setObjectName(QStringLiteral("actionNewBlankFile"));
    QIcon icon19;
    iconThemeName = QStringLiteral("document-new");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon19 = QIcon::fromTheme(iconThemeName);
    } else {
        icon19.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
    }
    actionNewBlankFile->setIcon(icon19);

    actionFindFiles = new QAction(this);
    actionFindFiles->setObjectName(QStringLiteral("actionFindFiles"));

    actionFilter = new QAction(this);
    actionFilter->setObjectName(QStringLiteral("actionFilter"));
    actionFilter->setCheckable(true);
    actionFilter->setChecked(settings.showFilter());

    menu_File = new QMenu(this);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    menuCreateNew = new QMenu(menu_File);
    menuCreateNew->setObjectName(QString::fromUtf8("menuCreateNew"));
    menu_Help = new QMenu(this);
    menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
    menu_View = new QMenu(this);
    menu_View->setObjectName(QString::fromUtf8("menu_View"));
    menuSorting = new QMenu(menu_View);
    menuSorting->setObjectName(QString::fromUtf8("menuSorting"));
    menu_Editw = new QMenu(this);
    menu_Editw->setObjectName(QString::fromUtf8("menu_Editw"));
    menu_Bookmarks = new QMenu(this);
    menu_Bookmarks->setObjectName(QString::fromUtf8("menu_Bookmarks"));
    menu_Go = new QMenu(this);
    menu_Go->setObjectName(QString::fromUtf8("menu_Go"));
    menu_Tool = new QMenu(this);
    menu_Tool->setObjectName(QString::fromUtf8("menu_Tool"));

    menu_File->addAction(actionNewTab);
    menu_File->addAction(actionNewWin);
    menu_File->addSeparator();
    menu_File->addAction(menuCreateNew->menuAction());
    menu_File->addSeparator();
    menu_File->addAction(actionFileProperties);
    menu_File->addAction(actionFolderProperties);
    menu_File->addSeparator();
    menu_File->addAction(actionCloseTab);
    menu_File->addAction(actionCloseWindow);
    menuCreateNew->addAction(actionNewFolder);
    menuCreateNew->addAction(actionNewBlankFile);
    menu_Help->addAction(actionAbout);
    menu_View->addAction(actionReload);
    menu_View->addAction(actionShowHidden);
    menu_View->addSeparator();
    menu_View->addAction(actionIconView);
    menu_View->addAction(actionThumbnailView);
    menu_View->addAction(actionCompactView);
    menu_View->addAction(actionDetailedList);
    menu_View->addSeparator();
    menu_View->addAction(menuSorting->menuAction());
    menu_View->addAction(actionFilter);
    menuSorting->addAction(actionByFileName);
    menuSorting->addAction(actionByMTime);
    menuSorting->addAction(actionByFileSize);
    menuSorting->addAction(actionByFileType);
    menuSorting->addAction(actionByOwner);
    menuSorting->addSeparator();
    menuSorting->addAction(actionAscending);
    menuSorting->addAction(actionDescending);
    menuSorting->addSeparator();
    menuSorting->addAction(actionFolderFirst);
    menuSorting->addAction(actionCaseSensitive);
    menu_Editw->addAction(actionCut);
    menu_Editw->addAction(actionCopy);
    menu_Editw->addAction(actionPaste);
    menu_Editw->addAction(actionRename);
    menu_Editw->addAction(actionDelete);
    menu_Editw->addSeparator();
    menu_Editw->addAction(actionSelectAll);
    menu_Editw->addAction(actionInvertSelection);
    menu_Editw->addSeparator();
    menu_Editw->addAction(actionPreferences);
    menu_Bookmarks->addAction(actionAddToBookmarks);
    menu_Bookmarks->addAction(actionEditBookmarks);
    menu_Go->addAction(actionHome);
    menu_Go->addAction(actionDesktop);
    menu_Go->addAction(actionComputer);
    menu_Go->addAction(actionTrash);
    menu_Go->addAction(actionNetwork);
    menu_Go->addAction(actionApplications);
    menu_Go->addSeparator();
    menu_Go->addAction(actionGoBack);
    menu_Go->addAction(actionGoForward);
    menu_Go->addAction(actionGoUp);
    menu_Tool->addAction(actionOpenTerminal);
    menu_Tool->addAction(actionOpenAsRoot);
    menu_Tool->addAction(actionFindFiles);

    QActionGroup* group = new QActionGroup(menu_View);
    group->setExclusive(true);
    group->addAction(actionIconView);
    group->addAction(actionCompactView);
    group->addAction(actionThumbnailView);
    group->addAction(actionDetailedList);

    group = new QActionGroup(menuSorting);
    group->setExclusive(true);
    group->addAction(actionByFileName);
    group->addAction(actionByMTime);
    group->addAction(actionByFileSize);
    group->addAction(actionByFileType);
    group->addAction(actionByOwner);

    group = new QActionGroup(menuSorting);
    group->setExclusive(true);
    group->addAction(actionAscending);
    group->addAction(actionDescending);

    addAction(menu_File->menuAction());
    addAction(menu_Editw->menuAction());
    addAction(menu_View->menuAction());
    addAction(menu_Go->menuAction());
    addAction(menu_Bookmarks->menuAction());
    addAction(menu_Tool->menuAction());
    addAction(menu_Help->menuAction());

    retranslateUi();
}

void MenuBar::retranslateUi() noexcept
{
    Settings& settings = static_cast<Application*>(qApp)->settings();

    actionGoUp->setText(QCoreApplication::translate("MenuBar", "Go &Up", nullptr));
#if QT_CONFIG(tooltip)
    actionGoUp->setToolTip(QCoreApplication::translate("MenuBar", "Go Up", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    actionGoUp->setShortcut(QCoreApplication::translate("MenuBar", "Alt+Up", nullptr));
#endif // QT_CONFIG(shortcut)
    actionHome->setText(QCoreApplication::translate("MenuBar", "&Home", nullptr));
#if QT_CONFIG(shortcut)
    actionHome->setShortcut(QCoreApplication::translate("MenuBar", "Alt+Home", nullptr));
#endif // QT_CONFIG(shortcut)
    actionReload->setText(QCoreApplication::translate("MenuBar", "&Reload", nullptr));
#if QT_CONFIG(shortcut)
    actionReload->setShortcut(QCoreApplication::translate("MenuBar", "F5", nullptr));
#endif // QT_CONFIG(shortcut)
    actionGo->setText(QCoreApplication::translate("MenuBar", "Go", nullptr));
    actionQuit->setText(QCoreApplication::translate("MenuBar", "Quit", nullptr));
    actionAbout->setText(QCoreApplication::translate("MenuBar", "&About", nullptr));
    actionNewWin->setText(QCoreApplication::translate("MenuBar", "&New Window", nullptr));
#if QT_CONFIG(tooltip)
    actionNewWin->setToolTip(QCoreApplication::translate("MenuBar", "New Window", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    actionNewWin->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
    actionShowHidden->setText(QCoreApplication::translate("MenuBar", "Show &Hidden", nullptr));
#if QT_CONFIG(shortcut)
    actionShowHidden->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+H", nullptr));
#endif // QT_CONFIG(shortcut)
    actionComputer->setText(QCoreApplication::translate("MenuBar", "&Computer", nullptr));
    actionTrash->setText(QCoreApplication::translate("MenuBar", "&Trash", nullptr));
    actionNetwork->setText(QCoreApplication::translate("MenuBar", "&Network", nullptr));
    actionDesktop->setText(QCoreApplication::translate("MenuBar", "&Desktop", nullptr));
    actionAddToBookmarks->setText(QCoreApplication::translate("MenuBar", "&Add to Bookmarks", nullptr));
    actionApplications->setText(QCoreApplication::translate("MenuBar", "&Applications", nullptr));
    actionReload_2->setText(QCoreApplication::translate("MenuBar", "Reload", nullptr));
    actionIconView->setText(QCoreApplication::translate("MenuBar", "&Icon View", nullptr));
    actionCompactView->setText(QCoreApplication::translate("MenuBar", "&Compact View", nullptr));
    actionDetailedList->setText(QCoreApplication::translate("MenuBar", "&Detailed List", nullptr));
    actionThumbnailView->setText(QCoreApplication::translate("MenuBar", "&Thumbnail View", nullptr));
    actionCut->setText(QCoreApplication::translate("MenuBar", "Cu&t", nullptr));
#if QT_CONFIG(shortcut)
    actionCut->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+X", nullptr));
#endif // QT_CONFIG(shortcut)
    actionCopy->setText(QCoreApplication::translate("MenuBar", "&Copy", nullptr));
#if QT_CONFIG(shortcut)
    actionCopy->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+C", nullptr));
#endif // QT_CONFIG(shortcut)
    actionPaste->setText(QCoreApplication::translate("MenuBar", "&Paste", nullptr));
#if QT_CONFIG(shortcut)
    actionPaste->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+V", nullptr));
#endif // QT_CONFIG(shortcut)
    actionSelectAll->setText(QCoreApplication::translate("MenuBar", "Select &All", nullptr));
#if QT_CONFIG(shortcut)
    actionSelectAll->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
    actionPreferences->setText(QCoreApplication::translate("MenuBar", "Pr&eferences", nullptr));
    actionAscending->setText(QCoreApplication::translate("MenuBar", "&Ascending", nullptr));
    actionDescending->setText(QCoreApplication::translate("MenuBar", "&Descending", nullptr));
    actionByFileName->setText(QCoreApplication::translate("MenuBar", "&By File Name", nullptr));
    actionByMTime->setText(QCoreApplication::translate("MenuBar", "By &Modification Time", nullptr));
    actionByFileType->setText(QCoreApplication::translate("MenuBar", "By File &Type", nullptr));
    actionByOwner->setText(QCoreApplication::translate("MenuBar", "By &Owner", nullptr));
    actionFolderFirst->setText(QCoreApplication::translate("MenuBar", "&Folder First", nullptr));
    actionNewTab->setText(QCoreApplication::translate("MenuBar", "New &Tab", nullptr));
#if QT_CONFIG(tooltip)
    actionNewTab->setToolTip(QCoreApplication::translate("MenuBar", "New Tab", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    actionNewTab->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+T", nullptr));
#endif // QT_CONFIG(shortcut)
    actionGoBack->setText(QCoreApplication::translate("MenuBar", "Go &Back", nullptr));
#if QT_CONFIG(tooltip)
    actionGoBack->setToolTip(QCoreApplication::translate("MenuBar", "Go Back", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    actionGoBack->setShortcut(QCoreApplication::translate("MenuBar", "Alt+Left", nullptr));
#endif // QT_CONFIG(shortcut)
    actionGoForward->setText(QCoreApplication::translate("MenuBar", "Go &Forward", nullptr));
#if QT_CONFIG(tooltip)
    actionGoForward->setToolTip(QCoreApplication::translate("MenuBar", "Go Forward", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    actionGoForward->setShortcut(QCoreApplication::translate("MenuBar", "Alt+Right", nullptr));
#endif // QT_CONFIG(shortcut)
    actionInvertSelection->setText(QCoreApplication::translate("MenuBar", "&Invert Selection", nullptr));
    actionDelete->setText(settings.useTrash() ?
                              QCoreApplication::translate("MenuBar", "&Move to Trash", nullptr) :
                              QCoreApplication::translate("MenuBar", "&Delete", nullptr));
#if QT_CONFIG(shortcut)
    actionDelete->setShortcut(QCoreApplication::translate("MenuBar", "Del", nullptr));
#endif // QT_CONFIG(shortcut)
    actionRename->setText(QCoreApplication::translate("MenuBar", "&Rename", nullptr));
#if QT_CONFIG(shortcut)
    actionRename->setShortcut(QCoreApplication::translate("MenuBar", "F2", nullptr));
#endif // QT_CONFIG(shortcut)
    actionCloseTab->setText(QCoreApplication::translate("MenuBar", "C&lose Tab", nullptr));
#if QT_CONFIG(shortcut)
    actionCloseTab->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+W", nullptr));
#endif // QT_CONFIG(shortcut)
    actionFileProperties->setText(QCoreApplication::translate("MenuBar", "File &Properties", nullptr));
#if QT_CONFIG(shortcut)
    actionFileProperties->setShortcut(QCoreApplication::translate("MenuBar", "Alt+Return", nullptr));
#endif // QT_CONFIG(shortcut)
    actionFolderProperties->setText(QCoreApplication::translate("MenuBar", "&Folder Properties", nullptr));
    actionCaseSensitive->setText(QCoreApplication::translate("MenuBar", "&Case Sensitive", nullptr));
    actionByFileSize->setText(QCoreApplication::translate("MenuBar", "By File &Size", nullptr));
    actionCloseWindow->setText(QCoreApplication::translate("MenuBar", "&Close Window", nullptr));
    actionEdit_Bookmarks->setText(QCoreApplication::translate("MenuBar", "Edit Bookmarks", nullptr));
    actionOpenTerminal->setText(QCoreApplication::translate("MenuBar", "Open &Terminal", nullptr));
#if QT_CONFIG(shortcut)
    actionOpenTerminal->setShortcut(QCoreApplication::translate("MenuBar", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
    actionOpenAsRoot->setText(QCoreApplication::translate("MenuBar", "Open as &Root", nullptr));
    actionEditBookmarks->setText(QCoreApplication::translate("MenuBar", "&Edit Bookmarks", nullptr));
    actionNewFolder->setText(QCoreApplication::translate("MenuBar", "&Folder", nullptr));
#if QT_CONFIG(shortcut)
    actionNewFolder->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+Shift+N", nullptr));
#endif // QT_CONFIG(shortcut)
    actionNewBlankFile->setText(QCoreApplication::translate("MenuBar", "&Blank File", nullptr));
#if QT_CONFIG(shortcut)
    actionNewBlankFile->setShortcut(QCoreApplication::translate("MenuBar", "Ctrl+Alt+N", nullptr));
#endif // QT_CONFIG(shortcut)
    actionFindFiles->setText(QCoreApplication::translate("MenuBar", "&Find Files", nullptr));
#if QT_CONFIG(shortcut)
    actionFindFiles->setShortcut(QCoreApplication::translate("MenuBar", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
    actionFilter->setText(QCoreApplication::translate("MenuBar", "Filter", nullptr));
    menu_File->setTitle(QCoreApplication::translate("MenuBar", "&File", nullptr));
    menuCreateNew->setTitle(QCoreApplication::translate("MenuBar", "C&reate New", nullptr));
    menu_Help->setTitle(QCoreApplication::translate("MenuBar", "&Help", nullptr));
    menu_View->setTitle(QCoreApplication::translate("MenuBar", "&View", nullptr));
    menuSorting->setTitle(QCoreApplication::translate("MenuBar", "&Sorting", nullptr));
    menu_Editw->setTitle(QCoreApplication::translate("MenuBar", "&Edit", nullptr));
    menu_Bookmarks->setTitle(QCoreApplication::translate("MenuBar", "&Bookmarks", nullptr));
    menu_Go->setTitle(QCoreApplication::translate("MenuBar", "&Go", nullptr));
    menu_Tool->setTitle(QCoreApplication::translate("MenuBar", "&Tool", nullptr));
} // retranslateUi

} // namespace Filer
