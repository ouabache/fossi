//-----------------------------------------------------------------------------
// File: mainwindow.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 24.2.2011
//
// Description:
// The main window of Kactus2.
//-----------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Plugins/PluginSystem/PluginManager.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

#include <designEditors/common/DrawMode.h>
#include <common/widgets/tabDocument/TabDocument.h>

#include <library/LibraryManager/VLNVDialer/vlnvdialer.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>
#include <QShowEvent>
#include <QHideEvent>

class HWDesignWidget;
class LibraryHandler;
class ComponentItem;
class BusPortItem;
class ComponentPreviewBox;
class MessageConsole;
class AdHocPortItem;
class AdHocInterfaceItem;
class ComponentInstanceEditor;
class ConfigurationEditor;
class SystemDetailsEditor;
//class AddressEditor;
class InterfaceEditor;
class ConnectionEditor;
class AdHocVisibilityEditor;
class AdHocEditor;
class HWConnection;
class DrawingBoard;
class ConnectionEndpoint;
class GraphicsConnection;
class ContextHelpBrowser;
class HelpWindow;

class Ribbon;
class RibbonGroup;

//-----------------------------------------------------------------------------
//! The main window of Kactus2.
//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent    The parent widget.
     */
    MainWindow(QWidget *parent = 0);
    
	//! The destructor
	virtual ~MainWindow();

public slots:
    /*!
     *  Opens the "New" dialog.
     */
    void createNew();

    /*!
     *  Opens the source file of the given component.
     *
     *      @param [in] compItem The component item.
     */
    void openCSource(ComponentItem* compItem);

    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

    /*!
     *  Opens a HW design.
     *
     *      @param [in] vlnv           The VLNV of the top-level component.
     *      @param [in] viewName       The name of the view to open.
     *      @param [in] forceUnlocked  Forces the design to be opened in unlocked mode.
     */
    void openDesign(VLNV const& vlnv = VLNV(), QString const& viewName = QString(), bool forceUnlocked = false);

    /*!
     *  Opens the memory design of the current HW design.
     *  
     */
    void openMemoryDesign(); 

    /*!
     *  Opens the memory design of the HW design specified by the component VLNV and view name.
     *  
     *      @param [in] vlnv            The VLNV of the top-level component.
     *      @param [in] viewName        Name of the view to open.
     *      @param [in] forceUnlocked   Forces the design to be opened in unlocked mode.
     */
    void openMemoryDesign(const VLNV& vlnv, const QString& viewName = QString(),
        bool forceUnlocked = true);    

    /*!
     *  Opens a SW design.
     *
     *      @param [in] vlnv           The VLNV of the top-level component.
     *      @param [in] viewName       The name of the view to open.
     *      @param [in] forceUnlocked  Forces the design to be opened in unlocked mode.
     */
	void openSWDesign(const VLNV& vlnv = VLNV(), QString const& viewName = QString(), bool forceUnlocked = false);

    /*!
     *  Opens a system design.
     *
     *      @param [in] vlnv           The VLNV of the system component whose system design to open.
     *      @param [in] viewName       The name of the view to open.
     *      @param [in] forceUnlocked  Forces the system to be opened in unlocked mode.
     */
    void openSystemDesign(VLNV const &vlnv, QString const& viewName = QString(), bool forceUnlocked = false);

    /*!
     *  Open bus to be edited
	 *
	 *      @param [in] vlnv           Identifies the bus definition
     *      @param [in] forceUnlocked  Forces the bus to be opened in unlocked mode.
	 */
	void openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV = VLNV(), bool disableBusDef = false,
        bool forceUnlocked = false);

	/*!
     *  Open component editor to edit a component details.
	 *
	 *      @param [in] vlnv           Identifies the component to edit.
     *      @param [in] forceUnlocked  Forces the component to be opened in unlocked mode.
	 */
	void openComponent(const VLNV& vlnv, bool forceUnlocked = false);

    /*!
     *  Opens a COM definition for editing.
     *
     *      @param [in] vlnv           The VLNV of the COM definition to open.
     *      @param [in] forceUnlocked  Forces the COM definition to be opened in unlocked mode.
     */
    void openComDefinition(VLNV const& vlnv, bool forceUnlocked = false);

    /*!
     *  Opens an API definition for editing.
     *
     *      @param [in] vlnv           The VLNV of the API definition to open.
     *      @param [in] forceUnlocked  Forces the API definition to be opened in unlocked mode.
     */
    void openApiDefinition(VLNV const& vlnv, bool forceUnlocked = false);

    //! Called when user closes the last design tab.
    void onLastDocumentClosed();

    //! Called when the user has selected another tab.
    void onDocumentChanged(int index);

	/*!
     *  Search the file system for new IP-Xact libraries.
	 *
	 *  This slot is called when user clicks to search for IP-Xact files.
	 */
	void onLibrarySearch();

	/*!
     *  Called when user selects a component on the draw board.
	 *
	 *      @param [in] component   Pointer to the DiagramComponent instance that is selected.
	 */
	void onComponentSelected(ComponentItem* component);

	/*!
     *  Called when user selects an interface on the draw board.
	 *
	 *      @param [in] interface   Pointer to the interface instance that is selected.
	 */
	void onInterfaceSelected(ConnectionEndpoint* interface);

	/*!
     *  Called when user selects a connection on the draw board.
	 *
	 *      @param [in] connection  Pointer to the connection that was selected.
	 */
	void onConnectionSelected(GraphicsConnection* connection);

    //! Called when the menu strip needs to be updated.
    void updateMenuStrip();

    //! Called when the zoom has changed.
    void updateZoomTools();

    //! Saves all modified documents.
    void saveAll();

    //! Adds a new column to the current HW/System design.
    void addColumn();

    //! Generates the VHDL from the current design.
    void generateVHDL();

	//! Generates the documentation for the current component/design.
	void generateDoc();

    /*!
     *  Runs the plugin generator specified by the given action.
     *
     *      @param [in] action The action corresponding to the plugin.
     */
    void runGeneratorPlugin(QAction* action);

    //! Zooms in the current document.
    void zoomIn();

    //! Zooms out the current document.
    void zoomOut();

    //! Resets the zoom of the current document to the original 1:1 ratio.
    void zoomOriginal();

    //! Fits the current document fully in view.
    void fitInView();

    //! Undoes one operation in the current document.
    void undo();

    //! Redoes one operation in the current document.
    void redo();

    //! Called when the draw mode has changed.
    void onDrawModeChanged(DrawMode mode);
    void drawModeChange(QAction *action);

    //! Called when the state of the protection button changes.
    void onProtectionChanged(bool locked);

    //! Changes the protection of the current document.
    void changeProtection(bool locked);

    /*!
     *  Creates a new component to the library.
     *
     *      @param [in] prodHier    The product hierarchy attribute.
     *      @param [in] firmness	The firmness attribute.
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The component editor is opened automatically after successful creation.
     */
    void createComponent(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a SW component to the library.
     *
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The component editor is opened automatically after successful creation.
     */
    void createSWComponent(VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a new design.
     *
     *      @param [in] prodHier    The product hierarchy attribute.
     *      @param [in] firmness	The firmness attribute.
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The HW design editor is opened automatically after successful creation.
     */
    void createDesign(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
                      VLNV const& vlnv, QString const& directory);
    /*!
     *  Creates a new design for an existing component.
     *
     *      @param [in] vlnv        The component's VLNV.
     *
     *      @remarks The HW design editor is opened automatically after successful creation.
     */
    void createDesignForExistingComponent(VLNV const& vlnv);

    /*!
     *  Creates a new hierarchical SW component with an SW design.
     *
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The SW design editor is opened automatically after successful creation.
     */
    void createSWDesign(VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a SW design for an existing component.
     *
     *      @param [in] vlnv        The component's VLNV.
     *
     *      @remarks The SW design editor is opened automatically after successful creation.
     */
    void createSWDesign(VLNV const& vlnv);

    /*!
     *  Creates a new system design.
     *
     *      @param [in] compVLNV   The VLNV of an existing component.
     *      @param [in] viewName   The name of the view to which the system design will be based on.
     *      @param [in] sysVLNV    The VLNV of the system to be created.
     *      @param [in] directory  The directory where to save the system.
     */
    void createSystem(VLNV const& compVLNV, QString const& viewName, VLNV const& sysVLNV,
        QString const& directory);

    void createSystemDesign(VLNV const& vlnv);

    /*!
     *  Creates a new bus definition and abstraction definition
	 *
	 *      @param [in] vlnv        The vlnv for the bus definition
	 *      @param [in] directory   The directory where to save the bus definition and abstraction definition.
	 */
	void createBus(VLNV const& vlnv, QString const& directory);

	/*!
     *  Creates a new abstraction definition for given bus definition.
	 *
	 *      @param [in] busDefVLNV      Identifies the bus definition.
	 *      @param [in] directory       The directory where to save the abstraction definition.
	 *      @param [in] disableBusDef   If true then the bus definition editor is disabled when editor is opened.
	 */
	void createAbsDef(const VLNV& busDefVLNV, const QString& directory, bool disableBusDef);

    /*!
     *  Creates a new COM definition.
     *
     *      @param [in] vlnv        VLNV for the COM definition.
     *      @param [in] directory   The directory where to save the design.
     *
     *      @remarks The COM definition editor is opened automatically after successful creation.
     */
    void createComDefinition(VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a new API definition.
     *
     *      @param [in] vlnv        VLNV for the API definition.
     *      @param [in] directory   The directory where to save the design.
     *
     *      @remarks The API definition editor is opened automatically after successful creation.
     */
    void createApiDefinition(VLNV const& vlnv, QString const& directory);

    /*!
     *  Opens the settings dialog.
     */
    void openSettings();

    /*!
     *  Shows the about box.
     */
    void showAbout();

    /*!
     *  Shows the help window.
     */
    void showHelp();

signals:

	//! Print a notice message to the user.
	void noticeMessage(const QString& msg);

	//! Print an error message to the user.
	void errorMessage(const QString& msg);

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

protected:

	//! Called when the user requests to close the program.
    virtual void closeEvent(QCloseEvent* event);

	//! Called when user i.e minimizes the main window.
	virtual void hideEvent(QHideEvent* event);

	//! Called when user i.e maximizes the main window after it has been minimized.
	virtual void showEvent(QShowEvent* event);


private slots:
	//! Handler for design widget's clearItemSelection signal.
	void onClearItemSelection();

	//! Handler for signals that inform that design has changed.
	void onDesignChanged();

    //! Saves the document hierarchy under current document.
    void saveCurrentDocumentHierarchy();

	//! Create a pop up menu to select which dock widgets to display.
	void selectVisibleDocks();

    //! Opens the visibility control menu.
    void openVisibilityControlMenu();

    //! Opens the workspace management menu.
    void openWorkspaceMenu();

    /*!
     *  Handles the situation when a workspace has been changed.
     */
    void onWorkspaceChanged(QAction* action);

    /*!
     *  Creates a new workspace, requesting a name for the workspace from the user using a dialog.
     */
    void onNewWorkspace();

    /*!
     *  Deletes a workspace, asking the user which workspace to delete using a dialog.
     */
    void onDeleteWorkspace();

	//! Handler for output action's trigger.
	void onOutputAction(bool show);

    //! Handler for context help action's trigger.
    void onContextHelpAction(bool show);

	//! Handler for preview box action's trigger.
	void onPreviewAction(bool show);

	//! Handler for library action's trigger.
	void onLibraryAction(bool show);

	//! Handler for configuration action's trigger.
	void onConfigurationAction(bool show);

    //! Handler for system details action's trigger.
    void onSystemDetailsAction(bool show);

	//! Handler for connection action's trigger.
	void onConnectionAction(bool show);

	//! Handler for interface action's trigger.
	void onInterfaceAction(bool show);

	//! Handler for instance action's trigger.
	void onInstanceAction(bool show);

    //! Handler for ad-hoc visibility action's trigger.
    void onAdHocVisibilityAction(bool show);

    //! Handler for ad hoc editor action's trigger.
    void onAdHocEditorAction(bool show);

    //! Handler for address action's trigger.
    void onAddressAction(bool show);

    /*!
     *  Handles the toggling of visibility controls.
     */
    void onVisibilityControlToggled(QAction*);

    //! Opens a dialog for setting library locations.
    void setLibraryLocations();

    //! Runs an import wizard for the currently open component.
    void onRunImportWizard();

    /*!
     *  Update the instance editor, when the design document is refreshed.
     */
    void onDesignDocumentRefreshed();

    /*!
     *  Configure the used views.
     */
    void onConfigureViews();

private:
	// Disable copying.
	MainWindow(MainWindow const& rhs);
    MainWindow& operator=(MainWindow const& rhs);

    /*!
     *  Returns true if a design with the given vlnv is already open.
     *
     *      @remarks If the design is already open, it is displayed.
     */
    bool isDesignOpen(VLNV const& vlnv, KactusAttribute::Implementation implementation) const;

    /*!
     *  Checks if the given list of VLVNs contain invalid references.
     *
     *      @param [in] hierRefs            The list of VLNVs to check
     *      @param [in] referencingVlnv     The referencing VLNV.
     *
     *      @return True, if at least one invalid reference is found, otherwise false.
     */
    bool hasInvalidReferences(QList<VLNV> hierRefs, VLNV const& referencingVlnv);

    /*!
     *  Restores the program's settings.
	 */
	void restoreSettings();

	/*! 
     *  Save the program's settings.
	 */
	void saveSettings();

	/*!
	 *  Copy the component editor settings from the current workspace.
	 *
	 *      @param [in] workspaceName   The name of the new workspace.
	 */
	void copyComponentEditorSettings(QString workspaceName);

	/*!
	 *  Creates a new workspace.
	 *
	 *      @param [in] workspaceName   The name of the new workspace.
	 */
	void createNewWorkspace(QString workspaceName);

    /*!
     *  Updates the workspace menu.
     */
    void updateWorkspaceMenu();

    /*!
     *  Restores the settings for the given workspace.
     *
     *      @param [in] workspaceName The name of the workspace.
     */
    void loadWorkspace(QString const& workspaceName);


	/*!
	 *  Apply the settings to all the documents.
	 */
	void applySettingsToOpenDocuments();

    /*!
     *  Saves the settings for the given workspace.
     *
     *      @param [in] workspaceName The name of the workspace.
     */
    void saveWorkspace(QString const& workspaceName);

	/*!
     *  Set up the actions in the tool bars
	 */
	void setupActions();

	/*!
     *  Set up the menus for the main window.
	 *
	 *  This function is used by the setupActions() after it has created the actions for the menus and should not
     *  be called directly.
	 */
	void setupMenus();

    /*!
     *  Adds generator plugin actions to the generation group.
     */
    void createGeneratorPluginActions();

    /*!
     *  Updates the generation group.
     */
    void updateGeneratorPluginActions();

	/*!
     *  Set up the drawing view where hierarchical components are edited.
	 */
	void setupDrawBoard();

	/*!
     *  Set up the dock for the LibraryHandler
	 */
	void setupLibraryDock();

	//! Set up the message console.
	void setupMessageConsole();

    /*!
     *  Sets up the context help system.
     */
    void setupContextHelp();

	//! Sets up the component instance editor.
	void setupInstanceEditor();

    //! Sets up the ad-hoc visibility editor.
    void setupAdHocVisibilityEditor();

    /*!
     *  Setup the ad hoc editor.
     */
    void setupAdHocEditor();

    //! Sets up the address editor.
    void setupAddressEditor();

	//! Set up the configuration editor.
	void setupConfigurationEditor();

    /*!
     *  Sets up the system details editor.
     */
    void setupSystemDetailsEditor();

	//! Set up the interface editor.
	void setupInterfaceEditor();

	//! Set up the connection editor.
	void setupConnectionEditor();

	/*!
     *  Check if a document with the given vlnv is already open.
	 *
	 *      @param [in] vlnv    The vlnv that identifies the document.
	 * 
	 *      @remark If a document with given vlnv is open then the document is shown.
	 * 
	 *      @return True if the document was already open.
	 */
	bool isOpen(const VLNV& vlnv) const;

    /*!
     *  Sets the visibility for a given window type if it is supported in the current document.
     *
     *      @param [in] windowType   The window type whose visibility to set.
     *      @param [in] show         Should the window be visible.
     */
    void setWindowVisibilityForSupportedWindow(TabDocument::SupportedWindows type, bool show);

    /*!
     *  Sets the window visibility for a given window type.
     *
     *      @param [in] windowType   The window type whose visibility to set.
     *      @param [in] show         Should the window be visible.
     */
    void setWindowVisibility(TabDocument::SupportedWindows windowType, bool show);

	/*!
	 *  Update the windows menu to contain the supported windows and visibility of the windows.
	 */
    void updateWindows();

    /*!
     *  Sets the visibility for the windows visibility control and its associated window.
     *
     *      @param [in] windowType         The type of the window to set.
     *      @param [in] dock               The window whose visibility to update.
     */
    void updateWindowAndControlVisibility(TabDocument::SupportedWindows windowType, QDockWidget* dock);

    /*!
     *  Checks if a given window type is supported in current document or by default.
     *
     *      @param [in] windowType   The window type to check.
     *
     *      @return True, if the type is supported, otherwise false.
     */
    bool isSupportedWindowType(TabDocument::SupportedWindows windowType);

    /*!
     *  Returns the supported windows of the current document. If no document is open, default windows are
     *  returned.
     *     
     *      @return The currently supported window types.
     */
    unsigned int currentlySupportedWindows();

    /*!
     *  Returns the default windows which are visible even if no document is open.
     *
     *      @return The default windows.
     */
    unsigned int defaultWindows();

    /*!
     *  Updates the visibility control menu according to the open document.
     *
     *      @param [in] document   The currently open document.
     */
    void updateVisibilityControlMenu(TabDocument* document);

    /*!
     *  Connects all the visibility controls for windows.     
     */
    void connectVisibilityControls();

    /*!
     *  Disconnects all the visibility controls for windows.     
     */
    void disconnectVisibilityControls();

    /*!
     *  Runs the component editor for the given component.
     *
     *      @param [in] component   The component to run the wizard for.
     *      @param [in] directory   The directory containing the component xml file.
     */
    void runComponentWizard(QSharedPointer<Component> component, QString const& directory);
    

	/*!
	 *  Set the visibilities for the plug ins.
	 */
	void setPluginVisibilities();


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the IP-Xact library
    LibraryHandler *libraryHandler_;

	//! The dock widget that contains the library widgets.
	QDockWidget* libraryDock_;
    
	//! Contains the open documents as each in it's own tab.
	DrawingBoard* designTabs_;

	//! The widget to set the display rules for library items in library handler.
	VLNVDialer* dialer_;

	//! The widget to display a preview of component.
	ComponentPreviewBox* previewBox_;

	//! The dock widget that contains the preview box.
	QDockWidget* previewDock_;

	//! The widget to print errors and notifications to user.
	MessageConsole* console_;

	//! The dock widget that contains the console.
	QDockWidget* consoleDock_;

    //! Context help browser.
    ContextHelpBrowser* contextHelpBrowser_;

    //! The dock widget for the context help browser.
    QDockWidget* contextHelpDock_;

	//! The widget to edit the settings of a component instance.
	ComponentInstanceEditor* instanceEditor_;

	//! The dock widget that contains the instance editor.
	QDockWidget* instanceDock_;

    //! The ad-hoc visibility editor and its dock widget.
    AdHocVisibilityEditor* adHocVisibilityEditor_;
    QDockWidget* adHocVisibilityDock_;

    //! Ad hoc editor and its dock widget.
    AdHocEditor* adhocEditor_;
    QDockWidget* adhocDock_;

    //! Address editor for HW designs and its dock widget.
    //AddressEditor* addressEditor_;
    //QDockWidget* addressDock_;

	//! The widget to edit the configuration of designs.
	ConfigurationEditor* configurationEditor_;

    //! The dock widget that contains the configuration editor.
    QDockWidget* configurationDock_;

    //! The widget to edit system details.
    SystemDetailsEditor* systemDetailsEditor_;

    //! The dock widget for the system details editor.
    QDockWidget* systemDetailsDock_;

	//! The widget to edit the interfaces
	InterfaceEditor* interfaceEditor_;

	//! The dock widget that contains the interface editor.
	QDockWidget* interfaceDock_;

	//! The widget to edit the connection
	ConnectionEditor* connectionEditor_;

	//! The dock widget that contains the connection editor.
	QDockWidget* connectionDock_;

	/*******************************************************************/
	// the actions in the menus

    Ribbon* ribbon_;

	//! Create a new hierarchical component in the IP-Xact library
	QAction *actNew_;

    //! Action to save the current design.
    QAction* actSave_;

	//! Action to save the current design as new object
	QAction* actSaveAs_;

    QAction* actSaveHierarchy_;

    //! Action to save all designs.
    QAction* actSaveAll_;

    //! Action to print the current document.
    QAction* actPrint_;

    //! The Edit group.
    RibbonGroup* editGroup_;

    //! Action to undo an operation.
    QAction* actUndo_;

    //! Action to redo an operation.
    QAction* actRedo_;

    //! Action to set library locations.
    QAction* actLibraryLocations_;

	//! Search for IP-Xact files in the file system
	QAction* actLibrarySearch_;

	//! Check the library integrity
	QAction* actCheckIntegrity_;

    //! The generation group.
    RibbonGroup* generationGroup_;

    //! Action group for plugin generators.
    QActionGroup* pluginActionGroup_;

    //! Action to generate VHDL from a design.
    QAction* actGenVHDL_;

	//! Action to generate the documentation for a component.
	QAction* actGenDocumentation_;

    //! Action to run import wizard.
    QAction* actRunImport_;

    //! The Diagram Tools group.
    RibbonGroup* diagramToolsGroup_;

    //! Action to add a new column to the current diagram.
    QAction* actAddColumn_;

    //! Action to select objects in a diagram.
    QAction* actToolSelect_;

    //! Action to draw connections in a diagram.
    QAction* actToolConnect_;

    //! Action to add interfaces in a diagram.
    QAction* actToolInterface_;

    //! Action to drafting.
    QAction* actToolDraft_;

    //! Action to toggle off-page connection in a diagram.
    QAction* actToolToggleOffPage_;

    //! Action to create labels.
    QAction* actToolLabel_;

    //! Action to zoom in.
    QAction* actZoomIn_;

    //! Action to zoom out.
    QAction* actZoomOut_;

    //! Action to reset the zoom to original 1:1 mapping.
    QAction* actZoomOriginal_;

    //! Action to adjust zoom to fit the document fully in view.
    QAction* actFitInView_;

	//! Action to select which dock widgets are visible.
	QAction* actVisibleDocks_;

    //! Action to manage visibility control.
    QAction* actVisibilityControl_;

    //! Action to open memory designer.
    QAction* openMemoryDesignerAction_;

    //! Action to manage the workspaces.
    QAction* actWorkspaces_;

    //! The protection group.
    RibbonGroup* protectGroup_; 

    //! Action to refresh the current document.
    QAction* actRefresh_;

    //! Action to set protection on/off.
    QAction* actProtect_;

    //! Action to open Kactus2 settings.
    QAction* actSettings_;

    //! Action to show the about box.
    QAction* actAbout_;

    //! Action to show the help window.
    QAction* actHelp_;

    //! Action to exit the program.
    QAction* actExit_;

    QActionGroup* modeActionGroup_;

    //! The group for configuration tools.
    RibbonGroup* configurationToolsGroup_;

    //! The action to configure the views.
    QAction* actionConfigureViews_;

	//! The menu containing the actions to select which windows to display.
	QMenu windowsMenu_;

    //! Menu which contains the actions for visibility control.
    QMenu visibilityMenu_;

    //! Menu which contains the actions for managing workspaces.
    QMenu workspaceMenu_;

    //! The name of the currently active workspace.
    QString curWorkspaceName_;

    //! The plugin manager.
    QSharedPointer<PluginManager> pluginMgr_;

    //! The help window.
    HelpWindow* helpWnd_;

	//! Contains the visibility for the windows.
    //! Used to maintain the visibility information when windows are hidden by change of the active document.
	QMap<TabDocument::SupportedWindows, bool> visibilities_;

};

#endif // MAINWINDOW_H
