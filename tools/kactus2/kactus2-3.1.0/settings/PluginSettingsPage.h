//-----------------------------------------------------------------------------
// File: PluginSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 19.02.2013
//
// Description:
// Settings page for adjusting plugin settings.
//-----------------------------------------------------------------------------

#ifndef PLUGINSETTINGSPAGE_H
#define PLUGINSETTINGSPAGE_H

#include "SettingsPage.h"

#include <common/widgets/DirectoryListSelector/DirectoryListSelector.h>
#include <Plugins/PluginSystem/PluginManager.h>

#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QSettings>
#include <QStackedWidget>
//-----------------------------------------------------------------------------
//! PluginSettingsPage class.
//-----------------------------------------------------------------------------
class PluginSettingsPage : public SettingsPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings   The settings store.
     *      @param [in]     pluginMgr  The plugin manager.
     */
    PluginSettingsPage(QSettings& settings, PluginManager& pluginMgr);

    /*!
     *  Destructor.
     */
    virtual  ~PluginSettingsPage();

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();

private slots:
    /*!
     *  Updates the correct settings in view when the tree item selection has changed.
     */
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev);

    /*!
     *  Updates the plugins tree when plugins directories have changed.
     */
    void onDirectoriesChanged();

private:
    //! Disable copying.
    PluginSettingsPage(PluginSettingsPage const& rhs);

    //! Sets the page layout.
    void setupLayout();

    /*!
     *  Updates the plugins tree.
     */
    void refreshPluginsTree(bool displayChanges = false);
    
    /*!
     *  Resets the general information and settings stacks for plugins.
     *
     */
    void resetStacks();

    PluginSettingsPage& operator=(PluginSettingsPage const& rhs);

    /*!
     *  Creates a new category item to the tree.
     *
     *      @param [in] text  Category title text.
     *      @param [in] icon  Icon for the category.
     */
    QTreeWidgetItem* createCategoryItem(QString const& text, QIcon const& icon);

    /*!
     *  Creates a new plugin item to the tree.
     *
     *      @param [in] plugin   Target plugin for the item.
     *
     *      @return The created tree item.
     */
    QTreeWidgetItem* createPluginItem(IPlugin* plugin);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! The plugin manager.
    PluginManager& pluginMgr_;

    //! The local manager.
    PluginManager localManager_;

    //! The plugins directories editor.
    DirectoryListSelector pluginDirSelector_;

    //! The plugins tree widget.
    QTreeWidget pluginsTree_;

    //! The settings stack.
    QStackedWidget settingsStack_;

    //! The plugin info stack.
    QStackedWidget infoStack_;

    //! Flag for indicating changes in plugin directories.    
    bool directoriesChanged_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINSETTINGSPAGE_H
