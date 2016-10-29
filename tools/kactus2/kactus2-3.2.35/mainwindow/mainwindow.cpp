//-----------------------------------------------------------------------------
// File: mainwindow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 24.2.2011
//
// Description:
// The main window of Kactus2.
//-----------------------------------------------------------------------------

#include "mainwindow.h"

#include "SplashScreen.h"
#include "ExitScreen.h"
#include "VersionHelper.h"

#include "NewWorkspaceDialog.h"
#include "DeleteWorkspaceDialog.h"

#include <mainwindow/Ribbon/Ribbon.h>
#include <mainwindow/Ribbon/RibbonGroup.h>
#include <mainwindow/MessageConsole/messageconsole.h>
#include <mainwindow/NewPages/NewComponentPage.h>
#include <mainwindow/NewPages/NewDesignPage.h>
#include <mainwindow/NewPages/NewSWComponentPage.h>
#include <mainwindow/NewPages/NewSWDesignPage.h>
#include <mainwindow/NewPages/NewSystemPage.h>
#include <mainwindow/NewPages/NewComDefinitionPage.h>
#include <mainwindow/NewPages/NewApiDefinitionPage.h>
#include <mainwindow/NewPages/NewBusDefinitionPage.h>
#include <mainwindow/DrawingBoard/DrawingBoard.h>
#include <mainwindow/SaveHierarchy/DocumentTreeBuilder.h>
#include <mainwindow/SaveHierarchy/SaveHierarchyDialog.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>
#include <common/dialogs/NewDesignDialog/NewDesignDialog.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/listSelectDialog/ListSelectDialog.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>
#include <common/dialogs/propertyPageDialog/PropertyPageDialog.h>

#include <designEditors/MemoryDesigner/MemoryDesignDocument.h>

#include <designEditors/common/ComponentInstanceEditor/componentinstanceeditor.h>
#include <designEditors/common/ConfigurationEditor/configurationeditor.h>
#include <designEditors/common/InterfaceEditor/interfaceeditor.h>
#include <designEditors/common/ConnectionEditor/connectioneditor.h>
#include <designEditors/HWDesign/HWDesignWidget.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/BusPortItem.h>
#include <designEditors/HWDesign/BusInterfaceItem.h>
#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/AdHocVisibilityEditor/AdHocVisibilityEditor.h>
#include <designEditors/HWDesign/AdhocEditor/AdhocEditor.h>
//#include <designEditors/HWDesign/AddressEditor/AddressEditor.h>

#include <designEditors/SystemDesign/SystemDetailsEditor/SystemDetailsEditor.h>
#include <designEditors/SystemDesign/SystemDesignWidget.h>
#include <designEditors/SystemDesign/SystemDesignDiagram.h>
#include <designEditors/common/DesignWidgetFactoryImplementation.h>

#include <editors/ApiDefinitionEditor/ApiDefinitionEditor.h>
#include <editors/BusDefinitionEditor/BusDefinitionEditor.h>
#include <editors/ComDefinitionEditor/ComDefinitionEditor.h>
#include <editors/ComponentEditor/componenteditor.h>
#include <editors/CSourceEditor/CSourceWidget.h>
#include <editors/CSourceEditor/CSourceContentMatcher.h>
#include <editors/ComponentEditor/common/ExpressionFormatterFactoryImplementation.h>

#include <editors/ConfigurationTools/ViewConfigurer.h>

#include <Help/HelpSystem/ContextHelpBrowser.h>
#include <Help/HelpSystem/HelpWindow.h>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>

#include <library/LibraryManager/libraryhandler.h>
#include <library/LibraryManager/LibraryUtils.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <IPXACTmodels/kactusExtensions/SWInstance.h>

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>
#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <settings/SettingsDialog.h>
#include <settings/SettingsUpdater.h>

#include <wizards/ComponentWizard/ComponentWizard.h>
#include <wizards/ImportWizard/ImportWizard.h>

#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QStringList>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDockWidget>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QDesktopServices>
#include <QCursor>
#include <QDateTime>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QPainter>
#include <QProcess>

class LibraryItem;

//-----------------------------------------------------------------------------
// Function: mainwindow::MainWindow()
//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent),
libraryHandler_(0),
libraryDock_(0),
designTabs_(0),
dialer_(0),
previewBox_(0),
previewDock_(0),
console_(0),
consoleDock_(0),
contextHelpBrowser_(0),
contextHelpDock_(0),
instanceEditor_(0),
instanceDock_(0),
adHocVisibilityEditor_(0),
adHocVisibilityDock_(0),
adhocEditor_(0),
adhocDock_(0),
//addressEditor_(0),
//addressDock_(0),
configurationEditor_(0),
configurationDock_(0),
systemDetailsEditor_(0),
systemDetailsDock_(0),
interfaceEditor_(0),
interfaceDock_(0),
connectionEditor_(0),
connectionDock_(0),
ribbon_(0),
actNew_(0),
actSave_(0),
actSaveAs_(0),
actSaveHierarchy_(0),
actPrint_(0),
editGroup_(0),
actUndo_(0),
actRedo_(0),
actLibraryLocations_(0),
actLibrarySearch_(0),
actCheckIntegrity_(0),
generationGroup_(0),
pluginActionGroup_(0),
actGenVHDL_(0),
actGenDocumentation_(0),
actRunImport_(0),
diagramToolsGroup_(0),
actAddColumn_(0),
actToolSelect_(0),
actToolConnect_(0),
actToolInterface_(0),
actToolDraft_(0),
actToolToggleOffPage_(0),
actToolLabel_(0),
actZoomIn_(0),
actZoomOut_(0),
actZoomOriginal_(0),
actFitInView_(0),
actVisibleDocks_(0),
actVisibilityControl_(0),
openMemoryDesignerAction_(0),
actWorkspaces_(0),
protectGroup_(0),
actRefresh_(0),
actProtect_(0),
actSettings_(0),
actAbout_(0),
actHelp_(0),
actExit_(0),
configurationToolsGroup_(0),
actionConfigureViews_(0),
windowsMenu_(this),
visibilityMenu_(this),
workspaceMenu_(this),
curWorkspaceName_("Default"),
pluginMgr_(),
helpWnd_(0),
visibilities_()
{
    qRegisterMetaTypeStreamOperators<HighlightStyleDesc>("HighlightStyleDesc");

    // set the identification tags for the application
	QCoreApplication::setOrganizationDomain(tr("tut.fi"));
	QCoreApplication::setOrganizationName(tr("TUT"));
	QCoreApplication::setApplicationName(tr("Kactus2"));
	QCoreApplication::setApplicationVersion("2.0");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    SettingsUpdater::runUpgrade(this);

	setWindowTitle(QCoreApplication::applicationName());
	setWindowIcon(QIcon(":icons/common/graphics/appicon.png"));

	// By default, the window is 1024x768 and set to maximized state.
	resize(1024, 768);
	setWindowState(Qt::WindowMaximized);

    // Setup windows.
    setupMessageConsole();
    setupContextHelp();
	setupDrawBoard();
    setupLibraryDock();
    setupInstanceEditor();
    setupAdHocVisibilityEditor();
    setupAdHocEditor();
    //setupAddressEditor();
	setupConfigurationEditor();
    setupSystemDetailsEditor();
	setupInterfaceEditor();
	setupConnectionEditor();

    // Load plugins.
    QSettings settings;
    QStringList pluginsPath = settings.value("Platform/PluginsPath", QStringList("Plugins")).toStringList();
 
    pluginMgr_ = QSharedPointer<PluginManager>(new PluginManager(pluginsPath));

	// some actions need the editors so set them up before the actions
	setupActions();

	// Restore program settings.
	restoreSettings();

	// don't display empty editors
    updateWindows();

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

//-----------------------------------------------------------------------------
// Function: ~MainWindow()
//-----------------------------------------------------------------------------
MainWindow::~MainWindow() 
{
	saveSettings();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onLibrarySearch()
//-----------------------------------------------------------------------------
void MainWindow::onLibrarySearch() 
{
	if (libraryHandler_)
    {
		libraryHandler_->searchForIPXactFiles();
	}
}

//-----------------------------------------------------------------------------
// Function: restoreSettings()
//-----------------------------------------------------------------------------
void MainWindow::restoreSettings()
{
    QSettings settings;

    // Load the active workspace.
    curWorkspaceName_ = settings.value("Workspaces/CurrentWorkspace", QString("Default")).toString();
    loadWorkspace(curWorkspaceName_);

    // Create default workspaces if the workspaces registry group is not found.
    settings.beginGroup("Workspaces");

    if (settings.childGroups().empty())
    {
        settings.endGroup();
 
		FilterWidget::FilterOptions defaultOptions;
		defaultOptions.type.advanced_ = false;
		dialer_->setFilters(defaultOptions);

		createNewWorkspace("Default");
		createNewWorkspace("Design");
    }
    else
    {
        settings.endGroup();
    }

    // Update the workspace menu.
    updateWorkspaceMenu();
}

//-----------------------------------------------------------------------------
// Function: saveSettings()
//-----------------------------------------------------------------------------
void MainWindow::saveSettings()
{
	QSettings settings;

    // Save the active workspace.
    settings.setValue("Workspaces/CurrentWorkspace", curWorkspaceName_);
    saveWorkspace(curWorkspaceName_);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::copyComponentEditorSettings()
//-----------------------------------------------------------------------------
void MainWindow::copyComponentEditorSettings(QString workspaceName)
{
	QSettings settings;

	QString activeWorkspacePath = "Workspaces/" + curWorkspaceName_ + "/ComponentEditorFilters/";
	QString newWorkspacePath = "Workspaces/" + workspaceName + "/ComponentEditorFilters/";
	
	for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);
		QString hwHierarchyName(KactusAttribute::hierarchyToString(val));

		hwHierarchyName = "HW/" + hwHierarchyName;

		foreach (QString name, ComponentEditor::getHwItemNames())
		{
			settings.setValue(newWorkspacePath + hwHierarchyName + "/" + name, 
				settings.value(activeWorkspacePath + hwHierarchyName + "/" + name).toBool());
		}
	}

	activeWorkspacePath = activeWorkspacePath + "SW/";
	newWorkspacePath = newWorkspacePath + "SW/";

	foreach (QString itemName, ComponentEditor::getSwItemNames())
	{
		settings.setValue(newWorkspacePath + itemName, settings.value(activeWorkspacePath + itemName).toBool());
	}
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createNewWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::createNewWorkspace(QString workspaceName)
{
	QSettings settings;

	QString workspacePath = "Workspaces/" + workspaceName;

	FilterWidget::FilterOptions filters = dialer_->getFilters();

	// Save the geometry and state of windows.
	settings.beginGroup(workspacePath);

	settings.setValue("WindowState", saveState());
	settings.setValue("Geometry", saveGeometry());
	settings.setValue("WindowPosition", pos());
	settings.setValue("ConfigurationVisibility", visibilities_.value(TabDocument::CONFIGURATIONWINDOW));
	settings.setValue("SystemDetailsVisibility", visibilities_.value(TabDocument::SYSTEM_DETAILS_WINDOW));
	settings.setValue("ConnectionVisibility", visibilities_.value(TabDocument::CONNECTIONWINDOW));
	settings.setValue("InstanceVisibility", visibilities_.value(TabDocument::INSTANCEWINDOW));
	settings.setValue("AdHocVisibility", visibilities_.value(TabDocument::ADHOCVISIBILITY_WINDOW));
    settings.setValue("AdHocEditorVisibility", visibilities_.value(TabDocument::ADHOC_WINDOW));
	settings.setValue("AddressVisibility", visibilities_.value(TabDocument::ADDRESS_WINDOW));
	settings.setValue("InterfaceVisibility", visibilities_.value(TabDocument::INTERFACEWINDOW));
	settings.setValue("LibraryVisibility", visibilities_.value(TabDocument::LIBRARYWINDOW));
	settings.setValue("OutputVisibility", visibilities_.value(TabDocument::OUTPUTWINDOW));
	settings.setValue("ContextHelpVisibility", visibilities_.value(TabDocument::CONTEXT_HELP_WINDOW));
	settings.setValue("PreviewVisibility", visibilities_.value(TabDocument::PREVIEWWINDOW));
    // 	settings.setValue("NotesVisibility", visibilities_.value(TabDocument::NOTES_WINDOW));

	// Save filters.
	settings.beginGroup("LibraryFilters");
	settings.beginGroup("Type");
	settings.setValue("ShowComponents", filters.type.components_);
	settings.setValue("ShowBuses", filters.type.buses_);
	settings.setValue("ShowAdvanced", filters.type.advanced_);
	settings.endGroup();
	settings.beginGroup("Implementation");
	settings.setValue("ShowHW", filters.implementation.hw_);
	settings.setValue("ShowSW", filters.implementation.sw_);
	settings.setValue("ShowSystem", filters.implementation.system_);
	settings.endGroup();
	settings.beginGroup("Hierarchy");
	settings.setValue("ShowGlobal", filters.hierarchy.flat_);
	settings.setValue("ShowProduct", filters.hierarchy.product_);
	settings.setValue("ShowBoard", filters.hierarchy.board_);
	settings.setValue("ShowChip", filters.hierarchy.chip_);
	settings.setValue("ShowSoC", filters.hierarchy.soc_);
	settings.setValue("ShowIP", filters.hierarchy.ip_);
	settings.endGroup();
	settings.beginGroup("Firmness");
	settings.setValue("ShowTemplates", filters.firmness.templates_);
	settings.setValue("ShowMutable", filters.firmness.mutable_);
	settings.setValue("ShowFixed", filters.firmness.fixed_);
	settings.endGroup(); // Firmness
	settings.endGroup(); // LibraryFilters

	// Create the component editor filters.
	settings.beginGroup("ComponentEditorFilters");
	settings.beginGroup("HW");

	for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);
		QString hwHierarchyName(KactusAttribute::hierarchyToString(val));
		settings.beginGroup(hwHierarchyName);

		foreach (QString itemName, ComponentEditor::getHwItemNames())
		{
			settings.setValue(itemName, true);
		}

		settings.endGroup(); // hwHierarchyName
	}

	settings.endGroup(); // HW
	settings.beginGroup("SW");
	
	foreach (QString itemName, ComponentEditor::getSwItemNames())
	{
		settings.setValue(itemName, true);
	}

	settings.endGroup(); // SW
	settings.endGroup(); // ComponentEditorFilters
	settings.endGroup(); // Workspace/workspaceName
}

//-----------------------------------------------------------------------------
// Function: updateWorkspaceMenu()
//-----------------------------------------------------------------------------
void MainWindow::updateWorkspaceMenu()
{
    // Create the workspace menu based on the settings.
    workspaceMenu_.clear();

    QSettings settings;
    settings.beginGroup("Workspaces");

    QStringList workspaceIDs = settings.childGroups();
    QActionGroup* workspaceGroup = new QActionGroup(this);
    workspaceGroup->setExclusive(true);

    foreach (QString const& workspaceID, workspaceIDs)
    {
        QString workspaceName = workspaceID;

        QAction* action = new QAction(workspaceName, this);
        action->setCheckable(true);
        action->setChecked(curWorkspaceName_ == workspaceName);

        workspaceGroup->addAction(action);
        workspaceMenu_.addAction(action);
    }

    settings.endGroup();

    connect(workspaceGroup, SIGNAL(triggered(QAction *)), this, SLOT(onWorkspaceChanged(QAction *)));

    // Add actions for creating and deleting new workspaces.
    QAction* addAction = new QAction(tr("New Workspace..."), this);
    connect(addAction, SIGNAL(triggered()), this, SLOT(onNewWorkspace()), Qt::UniqueConnection);

    QAction* deleteAction = new QAction(tr("Delete Workspace..."), this);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(onDeleteWorkspace()), Qt::UniqueConnection);

    workspaceMenu_.addSeparator();
    workspaceMenu_.addAction(addAction);
    workspaceMenu_.addAction(deleteAction);

	actWorkspaces_->setText(curWorkspaceName_);
}

//-----------------------------------------------------------------------------
// Function: loadWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::loadWorkspace(QString const& workspaceName)
{
    QSettings settings;

    // Create the registry key name.
    QString keyName = "Workspaces/" + workspaceName;
    settings.beginGroup(keyName);

    // Set the window to normal state (this fixed a weird error with window state).
    setWindowState(Qt::WindowNoState);

    if (settings.contains("WindowPosition"))
    {
        move(settings.value("WindowPosition").toPoint());
    }

	if (settings.contains("Geometry"))
	{
	    restoreGeometry(settings.value("Geometry").toByteArray());
	}

    if (settings.contains("WindowState")) {
        restoreState(settings.value("WindowState").toByteArray());
    }

    const bool configurationVisible = settings.value("ConfigurationVisibility", true).toBool();
    visibilities_.insert(TabDocument::CONFIGURATIONWINDOW, configurationVisible);
    configurationDock_->toggleViewAction()->setChecked(configurationVisible);

    const bool systemDetailsVisible = settings.value("SystemDetailsVisibility", true).toBool();
    visibilities_.insert(TabDocument::SYSTEM_DETAILS_WINDOW, systemDetailsVisible);
    systemDetailsDock_->toggleViewAction()->setChecked(systemDetailsVisible);

    const bool connectionVisible = settings.value("ConnectionVisibility", true).toBool();
    visibilities_.insert(TabDocument::CONNECTIONWINDOW, connectionVisible);    
    connectionDock_->toggleViewAction()->setChecked(connectionVisible);

    const bool instanceVisible = settings.value("InstanceVisibility", true).toBool();
    visibilities_.insert(TabDocument::INSTANCEWINDOW, instanceVisible); 
    instanceDock_->toggleViewAction()->setChecked(instanceVisible);

    const bool adHocVisibilityVisible = settings.value("AdHocVisibility", true).toBool();
    visibilities_.insert(TabDocument::ADHOCVISIBILITY_WINDOW, adHocVisibilityVisible);
    adHocVisibilityDock_->toggleViewAction()->setChecked(adHocVisibilityVisible);

    const bool adHocEditorVisible = settings.value("AdHocEditorVisibility", true).toBool();
    visibilities_.insert(TabDocument::ADHOC_WINDOW, adHocEditorVisible);
    adhocDock_->toggleViewAction()->setChecked(adHocEditorVisible);

    /*const bool addressVisible = settings.value("AddressVisibility", false).toBool();
    visibilities_.insert(TabDocument::ADDRESS_WINDOW, addressVisible);
    addressDock_->toggleViewAction()->setChecked(addressVisible);*/

    const bool interfaceVisible = settings.value("InterfaceVisibility", true).toBool();
    visibilities_.insert(TabDocument::INTERFACEWINDOW, interfaceVisible);
    interfaceDock_->toggleViewAction()->setChecked(interfaceVisible);

    const bool libraryVisible = settings.value("LibraryVisibility", true).toBool();
    visibilities_.insert(TabDocument::LIBRARYWINDOW, libraryVisible);
    libraryDock_->toggleViewAction()->setChecked(libraryVisible);

    const bool outputVisible = settings.value("OutputVisibility", true).toBool();
    visibilities_.insert(TabDocument::OUTPUTWINDOW, outputVisible);
    consoleDock_->toggleViewAction()->setChecked(outputVisible);

    const bool contextHelpVisible = settings.value("ContextHelpVisibility", false).toBool();
    visibilities_.insert(TabDocument::CONTEXT_HELP_WINDOW, contextHelpVisible);
    contextHelpDock_->toggleViewAction()->setChecked(contextHelpVisible);

    const bool previewVisible = settings.value("PreviewVisibility", true).toBool();
    visibilities_.insert(TabDocument::PREVIEWWINDOW, previewVisible);
    previewDock_->toggleViewAction()->setChecked(previewVisible);

    updateWindows();
    

    // Load filter settings.
    FilterWidget::FilterOptions filters;
    settings.beginGroup("LibraryFilters");
    settings.beginGroup("Type");
    filters.type.components_ = settings.value("ShowComponents", true).toBool();
    filters.type.buses_ = settings.value("ShowBuses", true).toBool();
    filters.type.advanced_ = settings.value("ShowAdvanced", false).toBool();
    settings.endGroup();
    settings.beginGroup("Implementation");
    filters.implementation.hw_ = settings.value("ShowHW", true).toBool();
    filters.implementation.sw_ = settings.value("ShowSW", true).toBool();
    filters.implementation.system_ = settings.value("ShowSystem", true).toBool();
    settings.endGroup();
    settings.beginGroup("Hierarchy");
    filters.hierarchy.flat_ = settings.value("ShowGlobal", true).toBool();
    filters.hierarchy.product_ = settings.value("ShowProduct", true).toBool();
    filters.hierarchy.board_ = settings.value("ShowBoard", true).toBool();
    filters.hierarchy.chip_ = settings.value("ShowChip", true).toBool();
    filters.hierarchy.soc_ = settings.value("ShowSoC", true).toBool();
    filters.hierarchy.ip_ = settings.value("ShowIP", true).toBool();
    settings.endGroup();
    settings.beginGroup("Firmness");
    filters.firmness.templates_ = settings.value("ShowTemplates", true).toBool();
    filters.firmness.mutable_ =settings.value("ShowMutable", true).toBool();
    filters.firmness.fixed_ =settings.value("ShowFixed", true).toBool();
    settings.endGroup();
    dialer_->setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::applySettingsToOpenDocuments()
//-----------------------------------------------------------------------------
void MainWindow::applySettingsToOpenDocuments()
{
	QSettings settings;

	// Apply the settings to the open documents.
	for (int i = 0; i < designTabs_->count(); ++i)
	{
		TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(i));
		Q_ASSERT(doc != 0);

		doc->applySettings(settings);
	}
}

//-----------------------------------------------------------------------------
// Function: saveWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::saveWorkspace(QString const& workspaceName)
{
    // Instance to save the settings.
    QSettings settings;

    // Create the registry group name.
    QString keyName = "Workspaces/" + workspaceName;


    FilterWidget::FilterOptions filters = dialer_->getFilters();

    // Save the geometry and state of windows.
    settings.beginGroup(keyName);

    settings.setValue("WindowState", saveState());
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowPosition", pos());
    settings.setValue("ConfigurationVisibility", visibilities_.value(TabDocument::CONFIGURATIONWINDOW));
    settings.setValue("SystemDetailsVisibility", visibilities_.value(TabDocument::SYSTEM_DETAILS_WINDOW));
    settings.setValue("ConnectionVisibility", visibilities_.value(TabDocument::CONNECTIONWINDOW));
    settings.setValue("InstanceVisibility", visibilities_.value(TabDocument::INSTANCEWINDOW));
    settings.setValue("AdHocVisibility", visibilities_.value(TabDocument::ADHOCVISIBILITY_WINDOW));
    settings.setValue("AdHocEditorVisibility", visibilities_.value(TabDocument::ADHOC_WINDOW));
    settings.setValue("AddressVisibility", visibilities_.value(TabDocument::ADDRESS_WINDOW));
    settings.setValue("InterfaceVisibility", visibilities_.value(TabDocument::INTERFACEWINDOW));
    settings.setValue("LibraryVisibility", visibilities_.value(TabDocument::LIBRARYWINDOW));
    settings.setValue("OutputVisibility", visibilities_.value(TabDocument::OUTPUTWINDOW));
    settings.setValue("ContextHelpVisibility", visibilities_.value(TabDocument::CONTEXT_HELP_WINDOW));
    settings.setValue("PreviewVisibility", visibilities_.value(TabDocument::PREVIEWWINDOW));
//     settings.setValue("NotesVisibility", visibilities_.value(TabDocument::NOTES_WINDOW));

    // Save filters.
    settings.beginGroup("LibraryFilters");
    settings.beginGroup("Type");
    settings.setValue("ShowComponents", filters.type.components_);
    settings.setValue("ShowBuses", filters.type.buses_);
    settings.setValue("ShowAdvanced", filters.type.advanced_);
    settings.endGroup();
    settings.beginGroup("Implementation");
    settings.setValue("ShowHW", filters.implementation.hw_);
    settings.setValue("ShowSW", filters.implementation.sw_);
    settings.setValue("ShowSystem", filters.implementation.system_);
    settings.endGroup();
    settings.beginGroup("Hierarchy");
    settings.setValue("ShowGlobal", filters.hierarchy.flat_);
    settings.setValue("ShowProduct", filters.hierarchy.product_);
    settings.setValue("ShowBoard", filters.hierarchy.board_);
    settings.setValue("ShowChip", filters.hierarchy.chip_);
    settings.setValue("ShowSoC", filters.hierarchy.soc_);
    settings.setValue("ShowIP", filters.hierarchy.ip_);
    settings.endGroup();
    settings.beginGroup("Firmness");
    settings.setValue("ShowTemplates", filters.firmness.templates_);
    settings.setValue("ShowMutable", filters.firmness.mutable_);
    settings.setValue("ShowFixed", filters.firmness.fixed_);
    settings.endGroup(); // Firmness
    settings.endGroup(); // LibraryFilters

	settings.endGroup(); // Workspaces/keyName   
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupActions()
//-----------------------------------------------------------------------------
void MainWindow::setupActions() 
{
	// the action to create a new hierarchical component
	actNew_ = new QAction(QIcon(":/icons/common/graphics/file-new.png"), tr("New"), this);
	actNew_->setShortcut(QKeySequence::New);
	connect(actNew_, SIGNAL(triggered()), this, SLOT(createNew()));

	actSave_ = new QAction(QIcon(":/icons/common/graphics/file-save.png"), tr("Save"), this);
	actSave_->setShortcut(QKeySequence::Save);
	actSave_->setEnabled(false);
	connect(actSave_, SIGNAL(triggered()), designTabs_, SLOT(saveCurrentDocument()));
    connect(designTabs_, SIGNAL(documentModifiedChanged(bool)), 
        actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

	actSaveAs_ = new QAction(QIcon(":/icons/common/graphics/file-save-as.png"), tr("Save As"), this);
	actSaveAs_->setShortcut(QKeySequence::SaveAs);
	actSaveAs_->setEnabled(false);
	connect(actSaveAs_, SIGNAL(triggered()), designTabs_, SLOT(saveCurrentDocumentAs()));

    actSaveHierarchy_ = new QAction(QIcon(":/icons/common/graphics/file-save-hierarchical.png"), 
        tr("Save Hierarchy"), this);
    actSaveHierarchy_->setEnabled(false);
    connect(actSaveHierarchy_, SIGNAL(triggered()), this, SLOT(saveCurrentDocumentHierarchy()));

	actSaveAll_ = new QAction(QIcon(":/icons/common/graphics/file-save_all.png"), tr("Save All"), this);
	actSaveAll_->setShortcut(QKeySequence("Ctrl+Shift+S"));
	connect(actSaveAll_, SIGNAL(triggered()), this, SLOT(saveAll()));

	actPrint_ = new QAction(QIcon(":/icons/common/graphics/file-print.png"), tr("Print"), this);
	actPrint_->setShortcut(QKeySequence::Print);
	actPrint_->setEnabled(false);
	connect(actPrint_, SIGNAL(triggered()), designTabs_, SLOT(printCurrentDocument()));

	actUndo_ = new QAction(QIcon(":/icons/common/graphics/edit-undo.png"), tr("Undo"), this);
	actUndo_->setShortcut(QKeySequence::Undo);
	connect(actUndo_, SIGNAL(triggered()), this, SLOT(undo()));

	actRedo_ = new QAction(QIcon(":/icons/common/graphics/edit-redo.png"), tr("Redo"), this);
	actRedo_->setShortcut(QKeySequence::Redo);
	connect(actRedo_, SIGNAL(triggered()), this, SLOT(redo()));

	actLibraryLocations_ = new QAction(QIcon(":/icons/common/graphics/library-config.png"),
		tr("Configure library"), this);
	connect(actLibraryLocations_, SIGNAL(triggered()), this, SLOT(setLibraryLocations()), Qt::UniqueConnection);

	// the action to search for IP-Xact documents in file system
	actLibrarySearch_ = new QAction(QIcon(":/icons/common/graphics/library-refresh.png"),
		tr("Refresh library"), this);
	connect(actLibrarySearch_, SIGNAL(triggered()),	this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

	// Check the library integrity
	actCheckIntegrity_ = new QAction(QIcon(":/icons/common/graphics/checkIntegrity.png"),
		tr("Check Integrity"), this);
	connect(actCheckIntegrity_, SIGNAL(triggered()),
		libraryHandler_, SLOT(onCheckLibraryIntegrity()), Qt::UniqueConnection);

	// Initialize the action to generate VHDL.
	actGenVHDL_ = new QAction(QIcon(":/icons/common/graphics/vhdl_gen.png"),
		tr("Generate Top-VHDL"), this);
	connect(actGenVHDL_, SIGNAL(triggered()), this, SLOT(generateVHDL()), Qt::UniqueConnection);

	// initialize the action to generate documentation for the component/design
	actGenDocumentation_ = new QAction(QIcon(":icons/common/graphics/documentation.png"),
		tr("Generate Documentation"), this);
	connect(actGenDocumentation_, SIGNAL(triggered()), this, SLOT(generateDoc()), Qt::UniqueConnection);

    // initialize the action to run import wizard for component.
    actRunImport_ = new QAction(QIcon(":icons/common/graphics/import.png"), tr("Import File to Component"), this);
    connect(actRunImport_, SIGNAL(triggered()), this, SLOT(onRunImportWizard()), Qt::UniqueConnection);

	// Initialize the action to add a new column.
	actAddColumn_ = new QAction(QIcon(":/icons/common/graphics/diagram-add-column.png"), tr("Add Column"), this);
	connect(actAddColumn_, SIGNAL(triggered()), this, SLOT(addColumn()), Qt::UniqueConnection);

	// Initialize the action to set draw mode to selection mode.
	actToolSelect_ = new QAction(QIcon(":/icons/common/graphics/tool-select.png"), tr("Select Tool"), this);
	actToolSelect_->setCheckable(true);
	actToolSelect_->setChecked(true);

	// Initialize the action to set draw mode to connection mode.
	actToolConnect_ = new QAction(QIcon(":/icons/common/graphics/tool-interconnection.png"),
		tr("Interconnection Tool"), this);
	actToolConnect_->setCheckable(true);

	// Initialize the action to set draw mode to interface mode.
	actToolInterface_ = new QAction(QIcon(":/icons/common/graphics/tool-interface.png"), tr("Interface Tool"), this);
	actToolInterface_->setCheckable(true);

	actToolDraft_ = new QAction(QIcon(":/icons/common/graphics/tool-drafting.png"), tr("Drafting Tool"), this);
	actToolDraft_->setCheckable(true);

	actToolToggleOffPage_ = new QAction(QIcon(":/icons/common/graphics/tool-toggle_offpage.png"),
		tr("Toggle Off-Page Tool"), this);
	actToolToggleOffPage_->setCheckable(true);

    actToolLabel_ = new QAction(QIcon(":/icons/common/graphics/balloon.png"), tr("Sticky Note Tool"), this);
    actToolLabel_->setCheckable(true);

	modeActionGroup_ = new QActionGroup(this);
	modeActionGroup_->setExclusive(true);
	modeActionGroup_->addAction(actToolSelect_);
	modeActionGroup_->addAction(actToolConnect_);
	modeActionGroup_->addAction(actToolInterface_);
	modeActionGroup_->addAction(actToolDraft_);
	modeActionGroup_->addAction(actToolToggleOffPage_);
    modeActionGroup_->addAction(actToolLabel_);
	connect(modeActionGroup_, SIGNAL(triggered(QAction *)), this, SLOT(drawModeChange(QAction *)));

	// Initialize the action to zoom in.
	actZoomIn_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_in.png"), tr("Zoom In"), this);
	actZoomIn_->setEnabled(false);
	actZoomIn_->setVisible(false);
	connect(actZoomIn_, SIGNAL(triggered()), this, SLOT(zoomIn()));

	// Initialize the action to zoom out.
	actZoomOut_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_out.png"), tr("Zoom Out"), this);
	actZoomOut_->setEnabled(false);
	actZoomOut_->setVisible(false);
	connect(actZoomOut_, SIGNAL(triggered()), this, SLOT(zoomOut()));

	// Initialize the action to reset the zoom to original 1:1 ratio.
	actZoomOriginal_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_original.png"),
		tr("Original 1:1 Zoom"), this);
	actZoomOriginal_->setEnabled(false);
	actZoomOriginal_->setVisible(false);
	connect(actZoomOriginal_, SIGNAL(triggered()), this, SLOT(zoomOriginal()));

	// Initialize the action to fit the document into the view.
	actFitInView_ = new QAction(QIcon(":/icons/common/graphics/view-fit_best.png"),
		tr("Fit Document to View"), this);
	actFitInView_->setEnabled(false);
	actFitInView_->setVisible(false);
	connect(actFitInView_, SIGNAL(triggered()), this, SLOT(fitInView()));

	// the action for user to select the visible docks
	actVisibleDocks_ = new QAction(QIcon(":icons/common/graphics/dockSelect.png"), tr("Visible Windows"), this);
    actVisibleDocks_->setMenu(&windowsMenu_);
    connect(actVisibleDocks_, SIGNAL(triggered()), this, SLOT(selectVisibleDocks()), Qt::UniqueConnection);

	// Initialize the action to manage visibility control.
	actVisibilityControl_ = new QAction(QIcon(":icons/common/graphics/visibility.png"), tr("Visibility Control"), this);
	actVisibilityControl_->setEnabled(false);
	actVisibilityControl_->setVisible(false);
    actVisibilityControl_->setMenu(&visibilityMenu_);
    connect(actVisibilityControl_, SIGNAL(triggered()), this, SLOT(openVisibilityControlMenu()), Qt::UniqueConnection);    
    connect(&visibilityMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onVisibilityControlToggled(QAction*)));

    // Initialize the action for opening memory designer.
    openMemoryDesignerAction_ = new QAction(QIcon(":icons/common/graphics/memoryDesigner.png"), tr("Open Memory Designer"), this);
    openMemoryDesignerAction_->setVisible(false);
    connect(openMemoryDesignerAction_, SIGNAL(triggered()), this, SLOT(openMemoryDesign()), Qt::UniqueConnection);   

	// Initialize the action to manage workspaces.
	actWorkspaces_ = new QAction(QIcon(":icons/common/graphics/workspace.png"),	tr("Workspaces"), this);
	connect(actWorkspaces_, SIGNAL(triggered()), this, SLOT(openWorkspaceMenu()), Qt::UniqueConnection);
    actWorkspaces_->setMenu(&workspaceMenu_);

	actRefresh_ = new QAction(QIcon(":/icons/common/graphics/refresh.png"), tr("Refresh"), this);
	actRefresh_->setShortcut(QKeySequence("F5"));
	connect(actRefresh_, SIGNAL(triggered(bool)), designTabs_, SLOT(refreshCurrentDocument()));

	actProtect_ = new QAction(QIcon(":/icons/common/graphics/protection-unlocked.png"), tr("Unlocked"), this);
	actProtect_->setCheckable(true);
	actProtect_->setEnabled(false);
	actProtect_->setShortcut(QKeySequence("Ctrl+L"));
	connect(actProtect_, SIGNAL(triggered(bool)), this, SLOT(changeProtection(bool)));

	// Initialize the action to open Kactus2 settings.
	actSettings_ = new QAction(QIcon(":/icons/common/graphics/system-settings.png"), tr("Settings"), this);
	connect(actSettings_, SIGNAL(triggered()), this, SLOT(openSettings()));

	// Initialize the action to open the about box.
	actAbout_= new QAction(QIcon(":/icons/common/graphics/system-about.png"), tr("About"), this);
	connect(actAbout_, SIGNAL(triggered()), this, SLOT(showAbout()), Qt::UniqueConnection);

	// Initialize the action to open the help window.
	actHelp_= new QAction(QIcon(":/icons/common/graphics/system-help.png"), tr("Help"), this);
	connect(actHelp_, SIGNAL(triggered()), this, SLOT(showHelp()), Qt::UniqueConnection);

	// Initialize the action to exit the program.
	actExit_ = new QAction(QIcon(":/icons/common/graphics/system-exit.png"), tr("Exit"), this);
	connect(actExit_, SIGNAL(triggered()), this, SLOT(close()), Qt::UniqueConnection);

    actionConfigureViews_ = new QAction(QIcon(":/icons/common/graphics/viewConfiguration.png"),
        tr("View Configuration"), this);
    connect(actionConfigureViews_, SIGNAL(triggered()), this, SLOT(onConfigureViews()), Qt::UniqueConnection);

    connectVisibilityControls();

	setupMenus();
}

//-----------------------------------------------------------------------------
// Function: setupMenus()
//-----------------------------------------------------------------------------
void MainWindow::setupMenus()
{
	QDockWidget* menuDock = new QDockWidget(tr("Menu"), this);
	menuDock->setObjectName(tr("Menu"));
	menuDock->setTitleBarWidget(new QWidget(this));
    menuDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	menuDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    ribbon_ = new Ribbon(menuDock);
    menuDock->setWidget(ribbon_);
    addDockWidget(Qt::BottomDockWidgetArea, menuDock);
	
	// The "File" group.
	RibbonGroup* fileGroup = ribbon_->addGroup(tr("File"));
	fileGroup->addAction(actNew_);
	fileGroup->addAction(actSave_);
	fileGroup->addAction(actSaveAs_);
	fileGroup->addAction(actSaveAll_);
    fileGroup->addAction(actSaveHierarchy_);
	fileGroup->addAction(actPrint_);

	fileGroup->widgetForAction(actNew_)->installEventFilter(ribbon_);
	fileGroup->widgetForAction(actSave_)->installEventFilter(ribbon_);
	fileGroup->widgetForAction(actSaveAs_)->installEventFilter(ribbon_);
    fileGroup->widgetForAction(actSaveHierarchy_)->installEventFilter(ribbon_);
	fileGroup->widgetForAction(actSaveAll_)->installEventFilter(ribbon_);
	fileGroup->widgetForAction(actPrint_)->installEventFilter(ribbon_);

	// The "Library" group.
	RibbonGroup* libGroup = ribbon_->addGroup(tr("Library"));
	libGroup->addAction(actLibraryLocations_);
	libGroup->addAction(actLibrarySearch_);
	libGroup->addAction(actCheckIntegrity_);

	libGroup->widgetForAction(actLibraryLocations_)->installEventFilter(ribbon_);
	libGroup->widgetForAction(actLibrarySearch_)->installEventFilter(ribbon_);
	libGroup->widgetForAction(actCheckIntegrity_)->installEventFilter(ribbon_);

	// The "Edit" group.
	editGroup_ = ribbon_->addGroup(tr("Edit"));
	editGroup_->addAction(actUndo_);
	editGroup_->addAction(actRedo_);
	editGroup_->addAction(actRefresh_);
	editGroup_->setVisible(false);
	editGroup_->setEnabled(false);

	editGroup_->widgetForAction(actUndo_)->installEventFilter(ribbon_);
	editGroup_->widgetForAction(actRedo_)->installEventFilter(ribbon_);
	editGroup_->widgetForAction(actRefresh_)->installEventFilter(ribbon_);

    // The "Generation" group.
    generationGroup_ = ribbon_->addGroup(tr("Generation"));
    generationGroup_->addAction(actGenDocumentation_);
    generationGroup_->addAction(actRunImport_);
    generationGroup_->addAction(actGenVHDL_);
    generationGroup_->setVisible(false);
    generationGroup_->setEnabled(false);

	generationGroup_->widgetForAction(actGenDocumentation_)->installEventFilter(ribbon_);
	generationGroup_->widgetForAction(actRunImport_)->installEventFilter(ribbon_);
	generationGroup_->widgetForAction(actGenVHDL_)->installEventFilter(ribbon_);
	
    createGeneratorPluginActions();
    
	//! The "Diagram Tools" group.
	diagramToolsGroup_ = ribbon_->addGroup(tr("Diagram Tools"));
	diagramToolsGroup_->addAction(actAddColumn_);
	diagramToolsGroup_->addAction(actToolSelect_);
	diagramToolsGroup_->addAction(actToolConnect_);
	diagramToolsGroup_->addAction(actToolInterface_);
	diagramToolsGroup_->addAction(actToolDraft_);
    diagramToolsGroup_->addAction(actToolToggleOffPage_);
    diagramToolsGroup_->addAction(actToolLabel_);
	diagramToolsGroup_->setVisible(false);

	diagramToolsGroup_->widgetForAction(actAddColumn_)->installEventFilter(ribbon_);
	diagramToolsGroup_->widgetForAction(actToolSelect_)->installEventFilter(ribbon_);
	diagramToolsGroup_->widgetForAction(actToolConnect_)->installEventFilter(ribbon_);
	diagramToolsGroup_->widgetForAction(actToolInterface_)->installEventFilter(ribbon_);
	diagramToolsGroup_->widgetForAction(actToolDraft_)->installEventFilter(ribbon_);
	diagramToolsGroup_->widgetForAction(actToolToggleOffPage_)->installEventFilter(ribbon_);
	diagramToolsGroup_->widgetForAction(actToolLabel_)->installEventFilter(ribbon_);

	//! The "View" group.
	RibbonGroup* viewGroup = ribbon_->addGroup(tr("View"));
	viewGroup->addAction(actZoomIn_);
	viewGroup->addAction(actZoomOut_);
	viewGroup->addAction(actZoomOriginal_);
	viewGroup->addAction(actFitInView_);
	viewGroup->addAction(actVisibleDocks_);
    viewGroup->addAction(actVisibilityControl_);
    viewGroup->addAction(openMemoryDesignerAction_);

	viewGroup->widgetForAction(actZoomIn_)->installEventFilter(ribbon_);
	viewGroup->widgetForAction(actZoomOut_)->installEventFilter(ribbon_);
	viewGroup->widgetForAction(actZoomOriginal_)->installEventFilter(ribbon_);
	viewGroup->widgetForAction(actFitInView_)->installEventFilter(ribbon_);
	viewGroup->widgetForAction(actVisibleDocks_)->installEventFilter(ribbon_);
	viewGroup->widgetForAction(actVisibilityControl_)->installEventFilter(ribbon_);
	viewGroup->widgetForAction(openMemoryDesignerAction_)->installEventFilter(ribbon_);

    //! The "Configuration tools" group.
    configurationToolsGroup_ = ribbon_->addGroup(tr("Configuration Tools"));
    configurationToolsGroup_->addAction(actionConfigureViews_);
    configurationToolsGroup_->setVisible(false);
    configurationToolsGroup_->setEnabled(false);

    configurationToolsGroup_->widgetForAction(actionConfigureViews_)->installEventFilter(ribbon_);

	//! The "Workspace" group.
	RibbonGroup* workspacesGroup = ribbon_->addGroup(tr("Workspace"));
	workspacesGroup->addAction(actWorkspaces_);
	workspacesGroup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	workspacesGroup->widgetForAction(actWorkspaces_)->installEventFilter(ribbon_);

	//! The Protection group.
	protectGroup_ = ribbon_->addGroup(tr("Protection"));
	protectGroup_->addAction(actProtect_);
	protectGroup_->setVisible(false);

	protectGroup_->widgetForAction(actProtect_)->installEventFilter(ribbon_);

	//! The "System" group.
	RibbonGroup* sysGroup = ribbon_->addGroup(tr("System"));
	sysGroup->addAction(actSettings_);
    sysGroup->addAction(actHelp_);
	sysGroup->addAction(actAbout_);
	sysGroup->addAction(actExit_);

	sysGroup->widgetForAction(actSettings_)->installEventFilter(ribbon_);
	sysGroup->widgetForAction(actHelp_)->installEventFilter(ribbon_);
	sysGroup->widgetForAction(actAbout_)->installEventFilter(ribbon_);
	sysGroup->widgetForAction(actExit_)->installEventFilter(ribbon_);

	// the menu to display the dock widgets
    //windowsMenu_.addAction(addressDock_->toggleViewAction());	
    windowsMenu_.addAction(adHocVisibilityDock_->toggleViewAction());
    windowsMenu_.addAction(adhocDock_->toggleViewAction());
    windowsMenu_.addAction(connectionDock_->toggleViewAction());
    windowsMenu_.addAction(contextHelpDock_->toggleViewAction());    
    windowsMenu_.addAction(instanceDock_->toggleViewAction());
    windowsMenu_.addAction(previewDock_->toggleViewAction());
    windowsMenu_.addAction(configurationDock_->toggleViewAction());
    windowsMenu_.addAction(systemDetailsDock_->toggleViewAction());
    windowsMenu_.addAction(libraryDock_->toggleViewAction());       
    windowsMenu_.addAction(interfaceDock_->toggleViewAction());
    windowsMenu_.addAction(consoleDock_->toggleViewAction());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setupDrawBoard()
//-----------------------------------------------------------------------------
void MainWindow::setupDrawBoard()
{
	designTabs_ = new DrawingBoard();
	designTabs_->setMovable(true);
	designTabs_->setTabsClosable(true);

	setCentralWidget(designTabs_);

	connect(designTabs_, SIGNAL(lastDocumentClosed()), this, SLOT(onLastDocumentClosed()), Qt::UniqueConnection);
	connect(designTabs_, SIGNAL(currentChanged(int)), this, SLOT(onDocumentChanged(int)), Qt::UniqueConnection);

    connect(designTabs_, SIGNAL(documentContentChanged()), this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
    connect(designTabs_, SIGNAL(documentEditStateChanged()), this, SLOT(updateMenuStrip()));
    
    connect(designTabs_, SIGNAL(helpUrlRequested(QString const&)),
        this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    connect(designTabs_, SIGNAL(errorMessage(const QString&)),
        console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(designTabs_, SIGNAL(noticeMessage(const QString&)),
        console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupLibraryDock()
//-----------------------------------------------------------------------------
void MainWindow::setupLibraryDock()
{
    // set up the dock widget for the library
	libraryDock_ = new QDockWidget(tr("IP-XACT Library"), this);
	libraryDock_->setObjectName(tr("Library"));
	libraryDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	libraryDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	dialer_ = new VLNVDialer(libraryDock_);

    libraryHandler_ = new LibraryHandler(dialer_, this);

    // pass the library root to dialer.
	const LibraryItem* root = libraryHandler_->getTreeRoot();
	dialer_->setRootItem(root);

	libraryDock_->setWidget(libraryHandler_);

	// create a container widget for dialer and library display
	QWidget* container = new QWidget(this);
	QVBoxLayout* containerLayout = new QVBoxLayout(container);
	containerLayout->addWidget(dialer_, 0);
	containerLayout->addWidget(libraryHandler_, 1);
	containerLayout->setSpacing(0);
	containerLayout->setContentsMargins(0, 0, 0, 0);
	libraryDock_->setWidget(container);

	addDockWidget(Qt::LeftDockWidgetArea, libraryDock_);

	// set up the preview box to display the component preview
	previewDock_ = new QDockWidget(tr("Component Preview"), this);
	previewDock_->setObjectName(tr("ComponentPreview"));
	previewDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea 
		| Qt::BottomDockWidgetArea);
	previewDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	previewBox_ = new ComponentPreviewBox(libraryHandler_);
	previewDock_->setWidget(previewBox_);

	addDockWidget(Qt::LeftDockWidgetArea, previewDock_);

    connect(libraryHandler_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(libraryHandler_, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SLOT(openDesign(const VLNV&, const QString&)));
    connect(libraryHandler_, SIGNAL(openMemoryDesign(const VLNV&, const QString&)),
        this, SLOT(openMemoryDesign(const VLNV&, const QString&)));

	connect(libraryHandler_, SIGNAL(createBus(const VLNV&, const QString&)),
		this, SLOT(createBus(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(createAbsDef(const VLNV&, const QString&, bool)),
		this, SLOT(createAbsDef(const VLNV&, const QString&, bool)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createComDef(const VLNV&, const QString&)),
        this, SLOT(createComDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createApiDef(const VLNV&, const QString&)),
        this, SLOT(createApiDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createSWDesign(const VLNV&)),
        this, SLOT(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createSystemDesign(const VLNV&)),
        this, SLOT(createSystemDesign(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createDesignForExistingComponent(const VLNV&)),
        this, SLOT(createDesignForExistingComponent(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openSWDesign(const VLNV&, QString const&)),
		this, SLOT(openSWDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openSystemDesign(const VLNV&, QString const&)),
        this, SLOT(openSystemDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openBus(const VLNV&, const VLNV&, bool)),
		this, SLOT(openBus(const VLNV&, const VLNV&, bool)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openComDefinition(const VLNV&)),
        this, SLOT(openComDefinition(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openApiDefinition(const VLNV&)),
        this, SLOT(openApiDefinition(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(refreshDialer()),
		dialer_, SLOT(refreshLibrary()), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(itemSelected(const VLNV&)),
		previewBox_, SLOT(setComponent(const VLNV&)), Qt::UniqueConnection);
	
	connect(libraryHandler_, SIGNAL(createComponent(KactusAttribute::ProductHierarchy,KactusAttribute::Firmness,
		const VLNV&, const QString&)),
		this, SLOT(createComponent(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness,
		const VLNV&, const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupMessageConsole()
//-----------------------------------------------------------------------------
void MainWindow::setupMessageConsole() 
{
	consoleDock_ = new QDockWidget(tr("Output"), this);
	consoleDock_->setObjectName(tr("Output"));
	consoleDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	consoleDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	console_ = new MessageConsole(consoleDock_);
	consoleDock_->setWidget(console_);
	addDockWidget(Qt::BottomDockWidgetArea, consoleDock_);

	connect(this, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
    // Force console visible on new messages.
    connect(this, SIGNAL(errorMessage(const QString&)),
        consoleDock_, SLOT(show()), Qt::UniqueConnection);
    connect(this, SIGNAL(noticeMessage(const QString&)),
        consoleDock_, SLOT(show()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setupContextHelp()
//-----------------------------------------------------------------------------
void MainWindow::setupContextHelp()
{
    // Create the dock widget for the context help.
    contextHelpDock_ = new QDockWidget(tr("Context Help"), this);
    contextHelpDock_->setObjectName(tr("Context Help"));
    contextHelpDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    contextHelpDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    // Initialize the help engine.
    QSettings settings;
    QString helpPath = settings.value("Platform/HelpPath", "Help").toString();

    QHelpEngine* helpEngine = new QHelpEngine(helpPath + "/Kactus2Help.qhc", this);
    helpEngine->setupData();

    // Create the help window.
    helpWnd_ = new HelpWindow(helpEngine, this);

    // Create the context help browser.
    contextHelpBrowser_ = new ContextHelpBrowser(helpEngine, "qthelp://com.tut.kactus2.2.0/doc",
                                                 contextHelpDock_);

    QColor bgColor = QApplication::palette().color(QPalette::Window);
    QString style = QString("QTextBrowser { border: 5px solid transparent; "
                            "background-color: #%1}").arg(bgColor.rgb() & 0x00FFFFFF, 0, 16);
    contextHelpBrowser_->setStyleSheet(style);

    contextHelpDock_->setWidget(contextHelpBrowser_);
    addDockWidget(Qt::BottomDockWidgetArea, contextHelpDock_);

    connect(this, SIGNAL(helpUrlRequested(QString const&)),
            contextHelpBrowser_, SLOT(onHelpRequested(QString const&)), Qt::UniqueConnection);
    contextHelpBrowser_->onHelpRequested("index.html");
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupConfigurationEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupConfigurationEditor()
{
	configurationDock_ = new QDockWidget(tr("Design Configuration Details"), this);
	configurationDock_->setObjectName(tr("Configuration editor"));
	configurationDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	configurationDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	configurationEditor_ = new ConfigurationEditor(libraryHandler_, configurationDock_);
	configurationDock_->setWidget(configurationEditor_);
	addDockWidget(Qt::RightDockWidgetArea, configurationDock_);

	connect(configurationEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
    //connect(configurationEditor_, SIGNAL(configurationChanged(QString const&)),
    //    instanceEditor_, SLOT(setTopComponentActiveView(QString const&)), Qt::UniqueConnection);
    connect(configurationEditor_, SIGNAL(designConfigurationChanged(QSharedPointer<DesignConfiguration>)),
        instanceEditor_, SLOT(changeDesignConfiguration(QSharedPointer<DesignConfiguration>)),
        Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: setupSystemDetailsEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupSystemDetailsEditor()
{
    systemDetailsDock_ = new QDockWidget(tr("HW Mapping Details"), this);
    systemDetailsDock_->setObjectName(tr("HW Mapping Details Editor"));
    systemDetailsDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    systemDetailsDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    systemDetailsEditor_ = new SystemDetailsEditor(libraryHandler_, systemDetailsDock_);
    systemDetailsDock_->setWidget(systemDetailsEditor_);
    addDockWidget(Qt::RightDockWidgetArea, systemDetailsDock_);

    connect(systemDetailsEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupInstanceEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupInstanceEditor()
{
	instanceDock_ = new QDockWidget(tr("Component Instance Details"), this);
	instanceDock_->setObjectName(tr("Instance Editor"));
	instanceDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	instanceDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	instanceEditor_ = new ComponentInstanceEditor(instanceDock_);
	instanceDock_->setWidget(instanceEditor_);
	addDockWidget(Qt::RightDockWidgetArea, instanceDock_);

	connect(instanceEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupAdHocVisibilityEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupAdHocVisibilityEditor()
{
    adHocVisibilityDock_ = new QDockWidget(tr("Ad hoc Visibility"), this);
    adHocVisibilityDock_->setObjectName(tr("Ad-hoc Visibility"));
    adHocVisibilityDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    adHocVisibilityDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    adHocVisibilityEditor_ = new AdHocVisibilityEditor(adHocVisibilityDock_);
    adHocVisibilityDock_->setWidget(adHocVisibilityEditor_);
    addDockWidget(Qt::RightDockWidgetArea, adHocVisibilityDock_);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupAdHocEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupAdHocEditor()
{
    adhocDock_ = new QDockWidget(tr("Ad hoc port details"));
    adhocDock_->setObjectName(tr("Ad hoc port details"));
    adhocDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    adhocDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    adhocEditor_ = new AdHocEditor(adhocDock_);
    adhocDock_->setWidget(adhocEditor_);
    addDockWidget(Qt::RightDockWidgetArea, adhocDock_);

    connect(adhocEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupAddressEditor()
//-----------------------------------------------------------------------------
/*void MainWindow::setupAddressEditor()
{
    addressDock_ = new QDockWidget(tr("Address Editor"), this);
    addressDock_->setObjectName(tr("Address Editor"));
    addressDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    addressDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    addressEditor_ = new AddressEditor(addressDock_);
    addressDock_->setWidget(addressEditor_);
    addDockWidget(Qt::BottomDockWidgetArea, addressDock_);

    connect(addressEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}*/

//-----------------------------------------------------------------------------
// Function: mainwindow::setupInterfaceEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupInterfaceEditor()
{
	interfaceDock_ = new QDockWidget(tr("Interface Editor"), this);
	interfaceDock_->setObjectName(tr("Interface Editor"));
	interfaceDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	interfaceDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	interfaceEditor_ = new InterfaceEditor(interfaceDock_, libraryHandler_);
	interfaceDock_->setWidget(interfaceEditor_);
	addDockWidget(Qt::RightDockWidgetArea, interfaceDock_);
    
    connect(interfaceEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupConnectionEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupConnectionEditor()
{
	connectionDock_ = new QDockWidget(tr("Connection Editor"), this);
	connectionDock_->setObjectName(tr("Connection Editor"));
	connectionDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	connectionDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	connectionEditor_ = new ConnectionEditor(connectionDock_, libraryHandler_);
	connectionDock_->setWidget(connectionEditor_);
	addDockWidget(Qt::RightDockWidgetArea, connectionDock_);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onDesignChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDesignChanged() 
{
	// find the currently open editor
	QWidget* widget = designTabs_->currentWidget();

	// if editor was found
	if (widget) 
    {
		DesignWidget* editor = dynamic_cast<DesignWidget*>(widget);

		// if editor is design widget then set it to be modified.
		if (editor) 
        {
			editor->setModified(true);
			updateMenuStrip();
		}
	}
}

//-----------------------------------------------------------------------------
// Function: MainWindow::saveCurrentDocumentHierarchy()
//-----------------------------------------------------------------------------
void MainWindow::saveCurrentDocumentHierarchy()
{
    TabDocument* currentDocument = dynamic_cast<TabDocument*>(designTabs_->currentWidget());

    if (currentDocument)
    {
        DocumentTreeBuilder documentTreeBuilder(libraryHandler_);
        QObject* documentRoot = documentTreeBuilder.createFrom(currentDocument->getDocumentVLNV());

        SaveHierarchyDialog saveDialog(documentRoot, libraryHandler_, this);
        saveDialog.exec();

        delete documentRoot;
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onClearItemSelection()
//-----------------------------------------------------------------------------
void MainWindow::onClearItemSelection()
{
    HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());

    if (designWidget != 0)
    {
        QSharedPointer<Design> containedDesign = designWidget->getDiagram()->getDesign();

        adHocVisibilityEditor_->setDataSource(designWidget->getDiagram(), containedDesign,
            designWidget->getEditProvider(), designWidget->isProtected());
    }
    else
    {
        adHocVisibilityEditor_->clear();
    }

    //addressEditor_->clear();
	instanceEditor_->clear();
	interfaceEditor_->clear();
	connectionEditor_->clear();
    adhocEditor_->clear();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onComponentSelected()
//-----------------------------------------------------------------------------
void MainWindow::onComponentSelected(ComponentItem* component)
{
	Q_ASSERT(component);

	connectionEditor_->clear();
	interfaceEditor_->clear();
    adhocEditor_->clear();
    
    DesignWidget* designWidget(0);

    HWDesignWidget* hwDesignWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());
    if (hwDesignWidget)
    {
        designWidget = hwDesignWidget;
    }
    else
    {
        SystemDesignWidget* systemDesignWidget = dynamic_cast<SystemDesignWidget*>(designTabs_->currentWidget());
        designWidget = systemDesignWidget;
    }

    QSharedPointer<Design> containingDesign = designWidget->getDiagram()->getDesign();
	
    // Update the instance and ad-hoc visibility editors.
	instanceEditor_->setComponentInstance(component, designWidget->getEditProvider(), containingDesign,
        designWidget->getDiagram()->getDesignConfiguration(), designWidget->getOpenViewName());

    HWComponentItem* hwComponent = dynamic_cast<HWComponentItem*>(component);
    if (hwComponent != 0)
    {
        adHocVisibilityEditor_->setDataSource(hwComponent, containingDesign, designWidget->getEditProvider(),
            designWidget->isProtected());
        //addressEditor_->setComponent(component);
    }
    else
    {
        adHocVisibilityEditor_->clear();
    }

	if (component->componentModel()->getVlnv().isValid())
	{
		libraryHandler_->onSelectionChanged(component->componentModel()->getVlnv());
		previewBox_->setComponent(component->componentModel()->getVlnv());
	}
	else
	{
		libraryHandler_->onClearSelection();
	}
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInterfaceSelected()
//-----------------------------------------------------------------------------
void MainWindow::onInterfaceSelected(ConnectionEndpoint* interface)
{
	Q_ASSERT(interface);

    // if the port has an encompassing component then it is selected
    ComponentItem* component = interface->encompassingComp();

    if (component != 0 && component->componentModel()->getVlnv().isValid())
    {
        libraryHandler_->onSelectionChanged(component->componentModel()->getVlnv());
        previewBox_->setComponent(component->componentModel()->getVlnv());
    }
    // if no component can be identified
    else
    {
        libraryHandler_->onClearSelection();
    }

    adHocVisibilityEditor_->clear();
    connectionEditor_->clear();
	instanceEditor_->clear();
    
    // TODO: Address editor bus interface select.

    if (!interface->isAdHoc())
    {
        DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->currentWidget());
        if (designWidget)
        {
            DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(designWidget->getDiagram());
            if (diagram)
            {
                interfaceEditor_->setInterface(interface, diagram->getDesign());
            }
        }

        adhocEditor_->clear();
    }
    else
    {
        AdHocItem* adhocEndPoint = dynamic_cast<AdHocItem*>(interface);
        if (adhocEndPoint)
        {
            HWDesignWidget* hwDesignWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());
            if (hwDesignWidget)
            {
                HWDesignDiagram* hwDiagram = dynamic_cast<HWDesignDiagram*>(hwDesignWidget->getDiagram());
                if (hwDiagram)
                {
                    adhocEditor_->setAdhocPort(adhocEndPoint, hwDiagram, hwDesignWidget->getEditProvider());
                }
            }
        }
        else
        {
            adhocEditor_->clear();
        }

        interfaceEditor_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: onConnectionSelected()
//-----------------------------------------------------------------------------
void MainWindow::onConnectionSelected(GraphicsConnection* connection)
{
    DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->currentWidget());

	Q_ASSERT(connection);
    adHocVisibilityEditor_->clear();
    adhocEditor_->clear();
    //addressEditor_->clear();
	instanceEditor_->clear();
	interfaceEditor_->clear();
	connectionEditor_->setConnection(connection, designWidget->getDiagram());
}

//-----------------------------------------------------------------------------
// Function: updateMenuStrip()
//-----------------------------------------------------------------------------
void MainWindow::updateMenuStrip()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	bool unlocked = doc != 0 && (!(doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) || !doc->isProtected());

	bool isHWComp = false;
	ComponentEditor* componentEditor = dynamic_cast<ComponentEditor*>(doc);
	if (componentEditor)
    {
		isHWComp = componentEditor->isHWImplementation();
	}

	bool isHWDesign = dynamic_cast<HWDesignWidget*>(doc) != 0;
    bool isSystemDesign = dynamic_cast<SystemDesignWidget*>(doc) != 0;

	actSave_->setEnabled(doc != 0 && doc->isModified());
	actSaveAs_->setEnabled(doc != 0);
    actSaveHierarchy_->setEnabled(componentEditor || dynamic_cast<DesignWidget*>(doc));
	actPrint_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PRINT_SUPPORT));

	// generation group is always visible when there is open editor but disabled when locked
	generationGroup_->setEnabled(unlocked);
	if (doc != 0 && (componentEditor != 0 || isHWDesign || isSystemDesign))
    {
		generationGroup_->setVisible(true);
	}
	else
    {
		generationGroup_->setVisible(false);
	}

	// if is hardware design then set all actions enabled
	if (isHWDesign)
    {
		actGenVHDL_->setEnabled(unlocked);
		actGenVHDL_->setVisible(true);

		actGenDocumentation_->setEnabled(unlocked);
		actGenDocumentation_->setVisible(true);
		
		actRunImport_->setEnabled(false);
        actRunImport_->setVisible(false);

        openMemoryDesignerAction_->setVisible(true);
	}
	// if is hardware component then set only documentation, modelsim and vhdl enabled
	else if (isHWComp)
    {
		actGenVHDL_->setEnabled(unlocked);
		actGenVHDL_->setVisible(true);
		
		actGenDocumentation_->setEnabled(unlocked);
		actGenDocumentation_->setVisible(true);

        actRunImport_->setEnabled(unlocked);
        actRunImport_->setVisible(true);

        openMemoryDesignerAction_->setVisible(false);
	}
	else
    {
		actGenVHDL_->setVisible(false);
		actGenDocumentation_->setVisible(false);
        actRunImport_->setVisible(false);
        openMemoryDesignerAction_->setVisible(false);
	}

    configurationToolsGroup_->setEnabled(unlocked);
    configurationToolsGroup_->setVisible(doc != 0 && (isHWComp || isHWDesign));
    actionConfigureViews_->setEnabled(unlocked);
    actionConfigureViews_->setVisible(isHWDesign || isHWComp);

	editGroup_->setVisible(doc != 0);
	editGroup_->setEnabled(doc != 0 && unlocked);
	actUndo_->setVisible(doc != 0 && doc->getEditProvider() != 0);
	actRedo_->setVisible(doc != 0 && doc->getEditProvider() != 0);
	actUndo_->setEnabled(doc != 0 && doc->getEditProvider() != 0 && doc->getEditProvider()->canUndo());
	actRedo_->setEnabled(doc != 0 && doc->getEditProvider() != 0 && doc->getEditProvider()->canRedo());
	
	diagramToolsGroup_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT));
	diagramToolsGroup_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT) &&
		!doc->isProtected());
	actToolSelect_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_SELECT));
	actToolConnect_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_CONNECT));
	actToolInterface_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_INTERFACE));
	actToolDraft_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_DRAFT));
    actToolToggleOffPage_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_TOGGLE_OFFPAGE));
    actToolLabel_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_LABEL));

	bool oldProtectionState = actProtect_->isChecked();

	protectGroup_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
	actProtect_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
	actProtect_->setChecked(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) &&
		doc->isProtected());

	if (oldProtectionState != actProtect_->isChecked())
		onProtectionChanged(actProtect_->isChecked());

    actVisibilityControl_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_VISIBILITY_CONTROL_SUPPORT));
	actVisibilityControl_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_VISIBILITY_CONTROL_SUPPORT));

	setPluginVisibilities();

	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: saveAll()
//-----------------------------------------------------------------------------
void MainWindow::saveAll()
{
	designTabs_->saveAll();

	if (designTabs_->currentWidget() != 0)
	{
		TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
		actSave_->setEnabled(doc->isModified());
	}
	else
	{
		actSave_->setEnabled(false);
	}
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void MainWindow::addColumn()
{
	QWidget* curWidget = designTabs_->currentWidget();
	DesignWidget* designWidget = dynamic_cast<DesignWidget*>(curWidget);

	if (designWidget != 0)
	{
		designWidget->addColumn();
	}
}

//-----------------------------------------------------------------------------
// Function: generateVHDL()
//-----------------------------------------------------------------------------
void MainWindow::generateVHDL()
{
	HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());
	ComponentEditor* compEditor = dynamic_cast<ComponentEditor*>(designTabs_->currentWidget());

	if (designWidget != 0)
	{
		designWidget->onVhdlGenerate();
	}
	else if (compEditor)
    {
		// if user changed the metadata then the editor must be reopened
		if (compEditor->onVhdlGenerate())
        {
			int index = designTabs_->currentIndex();
			VLNV compVLNV = compEditor->getDocumentVLNV();

			designTabs_->removeTab(index);
			delete compEditor;

			openComponent(compVLNV, true);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: MainWindow::runGeneratorPlugin()
//-----------------------------------------------------------------------------
void MainWindow::runGeneratorPlugin(QAction* action)
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    // Inform user that unsaved changes must be saved before continuing.
    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            "The document " + doc->getDocumentName() + " has unsaved changes and needs to be "
            "saved before generators can be run. Save and continue?",
            QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    // Retrieve the library component.
	 VLNV compVLNV = doc->getDocumentVLNV();
	 VLNV desVLNV = doc->getIdentifyingVLNV();
	 QSharedPointer<Document> libComp = libraryHandler_->getModel(compVLNV);
	 QSharedPointer<Document> libDes;
	 QSharedPointer<Document> libDesConf;

	 // if the design is supported by the document type
	 DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
	 if (desWidget)
     {
		 // the vlnvs must be for different objects
		 Q_ASSERT(compVLNV != desVLNV);

		 // design is the object that identifies the editor
		 libDes = libraryHandler_->getModel(desVLNV);

		 // find the design config is one exists
		 QString viewName = desWidget->getOpenViewName();

		 QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
		 VLNV desConfVLNV;

		 // the implementation defines where to search for the hierarchy ref
		 switch (desWidget->getImplementation()) {
		 case KactusAttribute::HW: {
			 desConfVLNV = comp->getHierRef(viewName);
			 break;
												 }
		 case KactusAttribute::SW: {
			 desConfVLNV = comp->getHierSWRef(viewName);
			 break;
												 }
		 case KactusAttribute::SYSTEM: {
			 desConfVLNV = comp->getHierSystemRef(viewName);
			 break;
												  }
		 default: {
			 Q_ASSERT(false);
			 return;
					 }
		 }

		 // the hierarchy reference must be valid
		 Q_ASSERT(desConfVLNV.isValid());

		 // if the hierarchy ref is not directly to the design but design config is in between
		 if (desConfVLNV != desVLNV)
         {
			 libDesConf = libraryHandler_->getModel(desConfVLNV);
		 }
	 }

    // Retrieve the plugin pointer from the action.
    IGeneratorPlugin* plugin = reinterpret_cast<IGeneratorPlugin*>(action->data().value<void*>());
	Q_ASSERT(plugin != 0);

	PluginUtilityAdapter adapter(libraryHandler_, this, VersionHelper::createVersionString());

    connect(&adapter, SIGNAL(errorMessage(QString const&)), 
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(&adapter, SIGNAL(infoMessage(QString const&)), 
        this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

    // Run the generator and refresh the document.
    plugin->runGenerator(&adapter, libComp, libDesConf, libDes);
    doc->refresh();
}

//-----------------------------------------------------------------------------
// Function: generateDoc()
//-----------------------------------------------------------------------------
void MainWindow::generateDoc()
{
	// get the vlnv of the current component
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc);
	VLNV vlnv = doc->getDocumentVLNV();
	Q_ASSERT(vlnv.isValid());

	// if changes have been made to the component
	if (doc->isModified())
    {
		QMessageBox::information(this, QCoreApplication::applicationName(), 
			tr("Changes have been made to the component. Save the changes before generating documentation."));
		return;
	}

    const QString XMLPath = libraryHandler_->getPath(vlnv);
    QFileInfo xmlInfo(XMLPath);

    QString targetPath = QFileDialog::getSaveFileName(NULL, tr("Save the documentation to..."),
        xmlInfo.absolutePath(), tr("web pages (*.html)"));

    if (targetPath.isEmpty())
    {
        return;
    }

    QFile targetFile(targetPath);
    if (!targetFile.open(QFile::WriteOnly))
    {
        emit errorMessage(tr("Could not open file %1 for writing.").arg(targetPath));
        return;
    }
    QTextStream stream(&targetFile);

    DesignWidgetFactoryImplementation designWidgetFactory(libraryHandler_);

    ExpressionFormatterFactoryImplementation expressionFormatterFactory;

    DocumentGenerator generator(libraryHandler_, vlnv, &designWidgetFactory, &expressionFormatterFactory, this);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

    generator.writeDocumentation(stream, targetPath);

    targetFile.close();

	// open the generated document in user's default browser
    QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath));

	// if the editor was component editor then it must be refreshed to make the changes to metadata visible
	ComponentEditor* compEditor = dynamic_cast<ComponentEditor*>(doc);

	if (compEditor)
    {
        designTabs_->refreshCurrentDocument();
	}
}

//-----------------------------------------------------------------------------
// Function: zoomIn()
//-----------------------------------------------------------------------------
void MainWindow::zoomIn()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->setZoomLevel(doc->getZoomLevel() + 10);
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: zoomOut()
//-----------------------------------------------------------------------------
void MainWindow::zoomOut()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->setZoomLevel(doc->getZoomLevel() - 10);
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: zoomOriginal()
//-----------------------------------------------------------------------------
void MainWindow::zoomOriginal()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->setZoomLevel(100);
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: fitInView()
//-----------------------------------------------------------------------------
void MainWindow::fitInView()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->fitInView();
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: drawModeChange()
//-----------------------------------------------------------------------------
void MainWindow::drawModeChange(QAction *action)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (action == actToolSelect_)
	{
		doc->setMode(MODE_SELECT);
	}
	else if (action == actToolConnect_)
	{
		doc->setMode(MODE_CONNECT);
	}
	else if (action == actToolInterface_)
	{
		doc->setMode(MODE_INTERFACE);
	}
	else if (action == actToolDraft_)
	{
		doc->setMode(MODE_DRAFT);
	}
    else if (action == actToolToggleOffPage_)
    {
        doc->setMode(MODE_TOGGLE_OFFPAGE);
    }
    else if (action == actToolLabel_)
    {
        doc->setMode(MODE_LABEL);
    }
}

//-----------------------------------------------------------------------------
// Function: onDrawModeChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDrawModeChanged(DrawMode mode)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT))
    {
        if (mode == MODE_SELECT)
        {
            doc->setCursor(Qt::ArrowCursor);
        }
        else
        {
            doc->setCursor(Qt::CrossCursor);
        }
    }	

    actToolSelect_->setChecked(mode == MODE_SELECT);
    actToolConnect_->setChecked(mode == MODE_CONNECT);
    actToolInterface_->setChecked(mode == MODE_INTERFACE);
    actToolDraft_->setChecked(mode == MODE_DRAFT);
    actToolToggleOffPage_->setChecked(mode == MODE_TOGGLE_OFFPAGE);
    actToolLabel_->setChecked(mode == MODE_LABEL);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onLastDocumentClosed()
//-----------------------------------------------------------------------------
void MainWindow::onLastDocumentClosed()
{	
    updateWindows();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onDocumentChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDocumentChanged(int index)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(index));

	// update the menu 
	if (doc) 
    {
		updateWindows();
        updateVisibilityControlMenu(doc);
	}

	// if the new tab is designWidget
	DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
	// set the configuration manager to edit the active design
	if (designwidget)
    {
		configurationEditor_->setConfiguration(designwidget);
        
        QSharedPointer<Document> topItem = libraryHandler_->getModel(doc->getDocumentVLNV());
        QSharedPointer<Component> topComponent = topItem.dynamicCast<Component>();
        if (topComponent)
        {
            instanceEditor_->setContext(topComponent, designwidget->getDiagram()->getDesignConfiguration(),
                designwidget->getDiagram()->getEditProvider());
            instanceEditor_->setTopComponentActiveView(topComponent->getModel()->findView(designwidget->getOpenViewName()));
            instanceEditor_->setProtection(designwidget->isProtected());
        }

        if (doc->getSupportedWindows() & TabDocument::SYSTEM_DETAILS_WINDOW)
        {
            systemDetailsEditor_->setSystem(designwidget);
        }
        else
        {
            systemDetailsEditor_->clear();
        }
	}
	// active tab is not design widget so clear the editors associated with design widget
	else
    {
		configurationEditor_->clear();
        systemDetailsEditor_->clear();
        //addressEditor_->clear();
		instanceEditor_->clear();
        adHocVisibilityEditor_->clear();
        adhocEditor_->clear();
		interfaceEditor_->clear();
		connectionEditor_->clear();
	}

	// Reset the draw mode to select.
	if (doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT))
	{
		doc->setMode(MODE_SELECT);
	}
	else
	{
		onDrawModeChanged(MODE_SELECT);
	}

	// Update the menu strip.
	updateMenuStrip();
}

//-----------------------------------------------------------------------------
// Function: closeEvent()
//-----------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent* event)
{
	// disconnect the signals that would otherwise change the window states to be saved.
	disconnectVisibilityControls();

	// Go through all tab documents and ask the user what to do if they are not saved.
    int tabsToClose = designTabs_->count();
    for (int i = 0; i < tabsToClose; i++)
    {
        int openTabCountBeforeClosingCurrent = designTabs_->count();

        designTabs_->closeAndRemoveDocument(designTabs_->currentIndex());
        
        if (openTabCountBeforeClosingCurrent == designTabs_->count())
        {
            event->ignore();
            return;
        }
    }

    QSettings settings;
    bool showExitScreen = settings.value("General/showExitScreen", true).toBool();
    if (showExitScreen)
    {
        hide();
        ExitScreen exitScreen(this);
        exitScreen.exec();
    }
}

//-----------------------------------------------------------------------------
// Function: updateZoomTools()
//-----------------------------------------------------------------------------
void MainWindow::updateZoomTools()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	bool canZoom = doc != 0 && (doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);

	actZoomIn_->setVisible(canZoom);
	actZoomOut_->setVisible(canZoom);
	actZoomOriginal_->setVisible(canZoom);
	actFitInView_->setVisible(canZoom);

	actZoomIn_->setEnabled(canZoom && doc->getZoomLevel() < doc->getMaxZoomLevel());
	actZoomOut_->setEnabled(canZoom && doc->getZoomLevel() > doc->getMinZoomLevel());
	actZoomOriginal_->setEnabled(canZoom);
	actFitInView_->setEnabled(canZoom);
}

//-----------------------------------------------------------------------------
// Function: createNew()
//-----------------------------------------------------------------------------
void MainWindow::createNew()
{
    // Create a property page dialog to work as a "New" dialog.
	PropertyPageDialog dialog(QSize(48, 48), 1, PropertyPageDialog::VIEW_ICONS,
                              PropertyPageDialog::APPLY_CURRENT, this);
	dialog.setFixedWidth(620);
	dialog.setWindowTitle(tr("New"));

    // Add pages to the dialog.
	NewComponentPage* compPage = new NewComponentPage(libraryHandler_, &dialog);
	connect(compPage, SIGNAL(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)),
		this, SLOT(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/hw-component.png"), tr("HW Component"), compPage);

	NewDesignPage* designPage = new NewDesignPage(libraryHandler_, &dialog);
	connect(designPage, SIGNAL(createDesign(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)),
		this, SLOT(createDesign(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/hw-design.png"), tr("HW Design"), designPage);

	NewSWComponentPage* swCompPage = new NewSWComponentPage(libraryHandler_, &dialog);
	connect(swCompPage, SIGNAL(createSWComponent(VLNV const&, QString const&)),
		this, SLOT(createSWComponent(VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/sw-component48x48.png"), tr("SW Component"), swCompPage);

	NewSWDesignPage* swDesignPage = new NewSWDesignPage(libraryHandler_, &dialog);
	connect(swDesignPage, SIGNAL(createSWDesign(VLNV const&, QString const&)),
		    this, SLOT(createSWDesign(VLNV const&, QString const&)), Qt::UniqueConnection);
	dialog.addPage(QIcon(":icons/common/graphics/sw-design48x48.png"), tr("SW Design"), swDesignPage);

	NewSystemPage* sysPage = new NewSystemPage(libraryHandler_, &dialog);
	connect(sysPage, SIGNAL(createSystem(VLNV const&, QString const&, VLNV const&, QString const&)),
		this, SLOT(createSystem(VLNV const&, QString const&, VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/system-component.png"), tr("System"), sysPage);

	NewBusDefinitionPage* busPage = new NewBusDefinitionPage(libraryHandler_, &dialog);
	connect(busPage, SIGNAL(createBus(VLNV const&, QString const&)),
		this, SLOT(createBus(VLNV const&, QString const&)), Qt::UniqueConnection);
	dialog.addPage(QIcon(":icons/common/graphics/new-bus.png"), tr("Bus Definition"), busPage);

    NewComDefinitionPage* comDefPage = new NewComDefinitionPage(libraryHandler_, &dialog);
    connect(comDefPage, SIGNAL(createComDefinition(VLNV const&, QString const&)),
            this, SLOT(createComDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/new-com_definition.png"), tr("COM Definition"), comDefPage);

    NewApiDefinitionPage* apiDefPage = new NewApiDefinitionPage(libraryHandler_, &dialog);
    connect(apiDefPage, SIGNAL(createApiDefinition(VLNV const&, QString const&)),
        this, SLOT(createApiDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/new-api_definition.png"), tr("API Definition"), apiDefPage);

    dialog.finalizePages();

	dialog.exec();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createComponent()
//-----------------------------------------------------------------------------
void MainWindow::createComponent(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
								 VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	// Create a component.
	QSharedPointer<Component> component = QSharedPointer<Component>(new Component(vlnv));

	// Set Kactus attributes.
    component->setHierarchy(prodHier);
    component->setFirmness(firmness);
    component->setImplementation(KactusAttribute::HW);

    QSharedPointer<View> emptyFlatView (new View());
    if (component->getHierarchy() == KactusAttribute::IP || component->getHierarchy() == KactusAttribute::SOC)
    {
        emptyFlatView->setName("rtl");
    }
    else
    {
        emptyFlatView->setName("flat");
    }

    component->getViews()->append(emptyFlatView);

    component->setVersion(VersionHelper::versionFileStr());

    // Create the file.
    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        emit errorMessage("Error saving file to disk.");
        return;
    }

    runComponentWizard(component, directory);    

	// Open the component editor.
	openComponent(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createDesign()
//-----------------------------------------------------------------------------
void MainWindow::createDesign(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
							  VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

	// Create a component and a hierarchical view .
	QSharedPointer<Component> component(new Component(vlnv));

	// Set Kactus attributes.
    component->setHierarchy(prodHier);
    component->setFirmness(firmness);
    component->setImplementation(KactusAttribute::HW);
    component->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<View> newHierarchicalView(new View());
    if (component->getHierarchy() == KactusAttribute::IP || component->getHierarchy() == KactusAttribute::SOC)
    {
        newHierarchicalView->setName("structural");
    }
    else
    {
        newHierarchicalView->setName("hierarchical");
    }

    QSharedPointer<ConfigurableVLNVReference> tempReference (new ConfigurableVLNVReference(desConfVLNV));

    QSharedPointer<DesignConfigurationInstantiation> hierarchicalInstantiation
        (new DesignConfigurationInstantiation(desConfVLNV.getName() + "_" + desConfVLNV.getVersion()));
    hierarchicalInstantiation->setDesignConfigurationReference(tempReference);

    newHierarchicalView->setDesignConfigurationInstantiationRef(hierarchicalInstantiation->name());

    component->getDesignConfigurationInstantiations()->append(hierarchicalInstantiation);
    component->getViews()->append(newHierarchicalView);

	QStringList viewNames = component->getHierViews();
	Q_ASSERT(!viewNames.isEmpty());

	// Create the design and design configuration.
	QSharedPointer<Design> design(new Design(designVLNV));
    design->setDesignImplementation(KactusAttribute::HW);
    design->setVersion(VersionHelper::versionFileStr());

	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::HW);
    designConf->setVersion(VersionHelper::versionFileStr());

	// Create the files.
	libraryHandler_->beginSave();

    bool success = true;

	if (!libraryHandler_->writeModelToFile(directory, designConf))
    {
        success = false;
    }

	if (!libraryHandler_->writeModelToFile(directory, design))
    {
        success = false;
    }

	if (!libraryHandler_->writeModelToFile(directory, component))
    {
        success = false;
    }

	libraryHandler_->endSave();

    if (success)
    {
	    // Open the design.
	    openDesign(vlnv, viewNames.first(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createDesignForExistingComponent()
//-----------------------------------------------------------------------------
void MainWindow::createDesignForExistingComponent(VLNV const& vlnv)
{
    Q_ASSERT(libraryHandler_->contains(vlnv));
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    // Retrieve the component to which the design will be created.
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::HW, this);
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv().getVendor(), component->getVlnv().getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/HWViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);
    component->setVersion(VersionHelper::versionFileStr());

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        foreach (QSharedPointer<View> view, *component->getViews())
        {
            if (view->name() == viewName)
            {
                ++runningNumber;
                viewName = baseViewName + QString::number(runningNumber);
            }
        }
    }

    dialog.setViewName(viewName);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    QSharedPointer<View> view (new View(dialog.getViewName()));

    VLNV designConfigVLNV = dialog.getDesignConfVLNV();
    QSharedPointer<DesignConfigurationInstantiation> hierarchyInstantiation
        (new DesignConfigurationInstantiation(designConfigVLNV.getName() + "_" + designConfigVLNV.getVersion()));
	hierarchyInstantiation->setDesignConfigurationReference(
		QSharedPointer<ConfigurableVLNVReference>( new ConfigurableVLNVReference( designConfigVLNV ) ) );

    view->setDesignConfigurationInstantiationRef(hierarchyInstantiation->name());

    component->getViews()->append(view);
    component->getDesignConfigurationInstantiations()->append(hierarchyInstantiation);

    // Create the design and design configuration objects.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::HW);
    designConf->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::HW);
    newDesign->setVersion(VersionHelper::versionFileStr());
    
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("IO", ColumnTypes::IO, 0, HWDesignDiagram::IO_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Buses", ColumnTypes::BUSES, 0, HWDesignDiagram::COMPONENT_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Components", ColumnTypes::COMPONENTS, 0, HWDesignDiagram::COMPONENT_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("IO", ColumnTypes::IO, 0, HWDesignDiagram::IO_COLUMN_WIDTH)));

    libraryHandler_->beginSave();

    bool success = true;

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), newDesign))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        openDesign(vlnv, view->name(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSWDesign(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".swdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".swdesigncfg", vlnv.getVersion());

    // Create a component and a hierarchical view .
    QSharedPointer<Component> component(new Component(vlnv));

    // Set Kactus attributes.
    component->setImplementation(KactusAttribute::SW);
    component->setVersion(VersionHelper::versionFileStr());

    // Create the view.
    QSharedPointer<SWView> view (new SWView("software"));
    view->setHierarchyRef(desConfVLNV);

    QList<QSharedPointer<SWView> > swViews = component->getSWViews();
    swViews.append(view);
    component->setSWViews(swViews);

    // Create the design and design configuration.
    QSharedPointer<Design> design(new Design(designVLNV));
    design->setDesignImplementation(KactusAttribute::SW);
    design->setVersion(VersionHelper::versionFileStr());
    
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
    designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::SW);
    designConf->setVersion(VersionHelper::versionFileStr());

    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Low-level", ColumnTypes::COMPONENTS, 0,
        SystemDesignDiagram::SW_COLUMN_WIDTH)));
    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Middle-level", ColumnTypes::COMPONENTS, 0, 
        SystemDesignDiagram::SW_COLUMN_WIDTH)));
    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("High-level", ColumnTypes::COMPONENTS, 0, 
        SystemDesignDiagram::SW_COLUMN_WIDTH)));
    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Out", ColumnTypes::IO, 0,
        SystemDesignDiagram::IO_COLUMN_WIDTH)));

    // Create the files.
    libraryHandler_->beginSave();

    bool success = true;

    if (!libraryHandler_->writeModelToFile(directory, designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(directory, design))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        openSWDesign(vlnv, component->getSWViewNames().first(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");

    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSWDesign(VLNV const& vlnv)
{
    Q_ASSERT(libraryHandler_->contains(vlnv));
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    // Retrieve the component to which the SW design will be created.
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::SW, this);
    dialog.setWindowTitle(tr("New SW Design"));
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv().getVendor(), component->getVlnv().getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/SWViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        foreach (QSharedPointer<SWView> swView, component->getSWViews())
        {
            if (swView->name() == viewName)
            {
                ++runningNumber;
                viewName = baseViewName + QString::number(runningNumber);
            }
        }
    }

    dialog.setViewName(viewName);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    QSharedPointer<SWView> view (new SWView(dialog.getViewName()));
    view->setHierarchyRef(dialog.getDesignConfVLNV());   

    QList<QSharedPointer<SWView> > swViews = component->getSWViews();
    swViews.append(view);
    component->setSWViews(swViews);

    component->setVersion(VersionHelper::versionFileStr());

    // Create the design and design configuration objects.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::SW);
    designConf->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<Design> newDesign(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::SW);
    newDesign->setVersion(VersionHelper::versionFileStr());

    if (component->getImplementation() == KactusAttribute::SW)
    {
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Low-level", ColumnTypes::COMPONENTS, 0, 
            SystemDesignDiagram::SW_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Middle-level", ColumnTypes::COMPONENTS, 0,
            SystemDesignDiagram::SW_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("High-level", ColumnTypes::COMPONENTS, 0, 
            SystemDesignDiagram::SW_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Out", ColumnTypes::IO, 0,
            SystemDesignDiagram::IO_COLUMN_WIDTH)));
    }
    else
    {
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Pre-mapped SW", ColumnTypes::COMPONENTS, 0,
            SystemDesignDiagram::SW_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Pre-mapped SW", ColumnTypes::COMPONENTS, 0,
            SystemDesignDiagram::SW_COLUMN_WIDTH)));
    }

    libraryHandler_->beginSave();

    bool success = true;

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), newDesign))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        openSWDesign(vlnv, view->name(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: openSettings()
//-----------------------------------------------------------------------------
void MainWindow::openSettings()
{
	SettingsDialog dialog(*pluginMgr_, this);

	connect(&dialog, SIGNAL(scanLibrary()), this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

	if (dialog.exec() == QDialog::Accepted)
	{
		applySettingsToOpenDocuments();
        updateGeneratorPluginActions();
	}
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createSystem()
//-----------------------------------------------------------------------------
void MainWindow::createSystem(VLNV const& compVLNV, QString const& viewName, VLNV const& sysVLNV,
                              QString const& directory)
{
	Q_ASSERT(sysVLNV.isValid());

	libraryHandler_->beginSave();

	VLNV designVLNV(VLNV::DESIGN, sysVLNV.getVendor(), sysVLNV.getLibrary(),
		sysVLNV.getName().remove(".comp") + ".sysdesign", sysVLNV.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, sysVLNV.getVendor(), sysVLNV.getLibrary(),
		sysVLNV.getName().remove(".comp") + ".sysdesigncfg", sysVLNV.getVersion());

    QSharedPointer<Component> parentComp;

    // Check if mapping information was given.
    if (compVLNV.isValid())
    {
        // Retrieve the component to which the system design will be based on.
        parentComp = libraryHandler_->getModel(compVLNV).staticCast<Component>();
        Q_ASSERT(parentComp != 0);
    }
    else
    {
        // Otherwise create a system component to encapsulate the system design.
        parentComp = QSharedPointer<Component>(new Component(sysVLNV));
        parentComp->setHierarchy(KactusAttribute::PRODUCT);
        parentComp->setFirmness(KactusAttribute::FIXED);
        parentComp->setImplementation(KactusAttribute::SYSTEM);
        parentComp->setVersion(VersionHelper::versionFileStr());
    }

    // Create the system view to the system design.
    QString sysViewName = "system";
    unsigned int runningNumber = 1;

    while (parentComp->findSystemView(sysViewName) != 0)
    {
        ++runningNumber;
        sysViewName = "system" + QString::number(runningNumber);
    }

    QSharedPointer<SystemView> systemView (new SystemView(sysViewName));
    systemView->setHierarchyRef(desConfVLNV);
    systemView->setHWViewRef(viewName);

    QList<QSharedPointer<SystemView> > systemViews = parentComp->getSystemViews();
    systemViews.append(systemView);
    parentComp->setSystemViews(systemViews);

	// Flat-out the hierarchy to form the system design.
	QSharedPointer<Design> sysDesign(new Design(designVLNV));
    sysDesign->setDesignImplementation(KactusAttribute::SYSTEM);
    sysDesign->setVersion(VersionHelper::versionFileStr());

    sysDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0, 
        SystemDesignDiagram::SYSTEM_COLUMN_WIDTH)));
    sysDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0, 
        SystemDesignDiagram::SYSTEM_COLUMN_WIDTH)));
    
	generateSystemDesignV2(libraryHandler_, parentComp->getHierRef(viewName), *sysDesign);

	// Create the design configuration.
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::SYSTEM);
    designConf->setVersion(VersionHelper::versionFileStr());

	// Create the files.
    bool success = true;

	if (!libraryHandler_->writeModelToFile(directory, designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(directory, sysDesign))
    {
        success = false;
    }

    if (compVLNV.isValid())
    {
        if (!libraryHandler_->writeModelToFile(parentComp))
        {
            success = false;
        }
    }
    else
    {
        if (!libraryHandler_->writeModelToFile(directory, parentComp))
        {
            success = false;
        }
    }

	libraryHandler_->endSave();

    if (success)
    {
	    openSystemDesign(parentComp->getVlnv(), sysViewName, true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSystemDesign(VLNV const& vlnv)
{
    Q_ASSERT(libraryHandler_->contains(vlnv));
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    libraryHandler_->beginSave();

    // Retrieve the component to which the system design will be created.
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();
    component->setVersion(VersionHelper::versionFileStr());

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::SYSTEM, this);
    dialog.setWindowTitle(tr("New System Design"));
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv().getVendor(), component->getVlnv().getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/SysViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        while (component->findSystemView(viewName) != 0)
        {
            ++runningNumber;
            viewName = baseViewName + QString::number(runningNumber);
        }
    }

    dialog.setViewName(viewName);
    
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    QSharedPointer<SystemView> view (new SystemView(dialog.getViewName()));
    view->setHierarchyRef(dialog.getDesignConfVLNV());   
    view->setHWViewRef(component->getHierViews().first());

    QList<QSharedPointer<SystemView> > systemViews = component->getSystemViews();
    systemViews.append(view);
    component->setSystemViews(systemViews);

    // Create the design and design configuration objects to the same folder as the component.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::SYSTEM);
    designConf->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::SYSTEM);
    newDesign->setVersion(VersionHelper::versionFileStr());

    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0,
        SystemDesignDiagram::SYSTEM_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0,
        SystemDesignDiagram::SYSTEM_COLUMN_WIDTH)));

    generateSystemDesignV2(libraryHandler_, component->getHierRef(), *newDesign);

    bool success = true;

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), newDesign))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        QString viewName = view->name();
        openSystemDesign(vlnv, viewName, true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createBus()
//-----------------------------------------------------------------------------
void MainWindow::createBus( VLNV const& vlnv, QString const& directory )
{
	Q_ASSERT(vlnv.isValid());
	Q_ASSERT(!directory.isEmpty());

	// create the vlnvs that identify the bus definition and abstraction definition
	VLNV busVLNV = vlnv;
	busVLNV.setType(VLNV::BUSDEFINITION);

	Q_ASSERT(!libraryHandler_->contains(busVLNV));

	VLNV absVLNV = busVLNV;

	// remove the possible .busDef from the end of the name field
	QString absDefName = busVLNV.getName();
	absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

	absVLNV.setName(absDefName + ".absDef");
	absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

	// by default the abs def and bus def are saved to same directory
	QString absDirectory = directory;

	if (libraryHandler_->contains(absVLNV))
    {
		VLNV newAbsDefVLNV;

		if (!NewObjectDialog::saveAsDialog(this, libraryHandler_, absVLNV, newAbsDefVLNV, absDirectory,
			"Set VLNV for abstraction definition"))
        {
            // if user canceled
            return;
		}
		// save the created abstraction definition vlnv
		absVLNV = newAbsDefVLNV;
	}

	// Create a bus definition.
	QSharedPointer<BusDefinition> busDef = QSharedPointer<BusDefinition>(new BusDefinition());
	busDef->setVlnv(busVLNV);
    busDef->setVersion(VersionHelper::versionFileStr());

	// Create the file for the bus definition.
    bool success = true;

    if (!libraryHandler_->writeModelToFile(directory, busDef))
    {
        success = false;
    }

	// create an abstraction definition
	QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
	absDef->setVlnv(absVLNV);
    absDef->setVersion(VersionHelper::versionFileStr());

	// set reference from abstraction definition to bus definition
	absDef->setBusType(busVLNV);

	// create the file for the abstraction definition
    if (!libraryHandler_->writeModelToFile(absDirectory, absDef))
    {
        success = false;
    }

    if (success)
    {
	    // Open the bus editor.
	    openBus(busVLNV, absVLNV, false, true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createAbsDef()
//-----------------------------------------------------------------------------
void MainWindow::createAbsDef( const VLNV& busDefVLNV, const QString& directory, bool disableBusDef )
{
	Q_ASSERT(busDefVLNV.isValid());
	Q_ASSERT(!directory.isEmpty());

	VLNV absVLNV = busDefVLNV;

	// remove the possible .busDef from the end of the name field
	QString absDefName = busDefVLNV.getName();
	absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

	absVLNV.setName(absDefName + ".absDef");
	absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

	// by default the abs def and bus def are saved to same directory
	QString absDirectory = directory;

	if (libraryHandler_->contains(absVLNV))
    {
		VLNV newAbsDefVLNV;

		if (!NewObjectDialog::saveAsDialog(this, libraryHandler_, absVLNV, newAbsDefVLNV, absDirectory,
			"Set VLNV for abstraction definition"))
        {
            // if user canceled
            return;
		}
		// save the created abstraction definition vlnv
		absVLNV = newAbsDefVLNV;
	}

	// create an abstraction definition
	QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
	absDef->setVlnv(absVLNV);
    absDef->setVersion(VersionHelper::versionFileStr());

	// set reference from abstraction definition to bus definition
	absDef->setBusType(busDefVLNV);

	// create the file for the abstraction definition
    if (!libraryHandler_->writeModelToFile(absDirectory, absDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

	// Open the bus editor.
	openBus(busDefVLNV, absVLNV, disableBusDef, true);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createComDefinition()
//-----------------------------------------------------------------------------
void MainWindow::createComDefinition(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());
    Q_ASSERT(!directory.isEmpty());

    // Create an empty COM definition and save it.
    QSharedPointer<ComDefinition> comDef(new ComDefinition(vlnv));
    comDef->setVersion(VersionHelper::versionFileStr());

    if (!libraryHandler_->writeModelToFile(directory, comDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the COM definition.
    openComDefinition(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createApiDefinition()
//-----------------------------------------------------------------------------
void MainWindow::createApiDefinition(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());
    Q_ASSERT(!directory.isEmpty());

    VLNV apiDefVLNV = vlnv;
    //apiDefVLNV.setName(vlnv.getName() + ".apiDef");

    // Create an empty API definition and save it.
    QSharedPointer<ApiDefinition> apiDef(new ApiDefinition(apiDefVLNV));
    apiDef->setVersion(VersionHelper::versionFileStr());

    if (!libraryHandler_->writeModelToFile(directory, apiDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the API definition.
    openApiDefinition(apiDefVLNV, true);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::openBus()
//-----------------------------------------------------------------------------
void MainWindow::openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV, bool disableBusDef, bool forceUnlocked )
{
	if (isOpen(absDefVLNV))
    {
		return;
	}
	else if (isOpen(busDefVLNV))
    {
		return;
	}

	// Check if the bus editor is already open and activate it.
	if (busDefVLNV.isValid())
    {
		for (int i = 0; i < designTabs_->count(); i++)
        {
			BusDefinitionEditor* editor = dynamic_cast<BusDefinitionEditor*>(designTabs_->widget(i));

			// if the abstraction definition matches
			if (editor && absDefVLNV.isValid() && editor->getDocumentVLNV() == absDefVLNV)
            {
				designTabs_->setCurrentIndex(i);
				return;
			}

			// if bus definition matches
			if (editor && editor->getDocumentVLNV() == busDefVLNV)
            {
				designTabs_->setCurrentIndex(i);
				return;
			}
		}
	}
	// at least the bus definition vlnv must be valid
	else
    {
		return;
	}

	// Editor for given vlnv was not yet open so create one for it
	QSharedPointer<BusDefinition> busDef;
	QSharedPointer<AbstractionDefinition> absDef;

	if (libraryHandler_->contains(busDefVLNV) && 
		libraryHandler_->getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION)
    {
        QSharedPointer<Document> libComp = libraryHandler_->getModel(busDefVLNV);
        busDef = libComp.dynamicCast<BusDefinition>();
	}
	else
    {
		emit errorMessage(tr("Bus definition %1 was not found in the library").arg(busDefVLNV.toString()));
		return;
	}

	// pointer to the bus editor
	BusDefinitionEditor* editor = 0;

	// if abstraction definition has been specified
	if (absDefVLNV.isValid())
    {
		// if library contains the abstraction definition
		if (libraryHandler_->contains(absDefVLNV) && 
			libraryHandler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION)
        {
				QSharedPointer<Document> libComp = libraryHandler_->getModel(absDefVLNV);
				absDef = libComp.staticCast<AbstractionDefinition>();

				editor = new BusDefinitionEditor(this, libraryHandler_, busDef, absDef, disableBusDef);
		}
		else
        {
			emit errorMessage(tr("Abstraction definition %1 was not found in the library").arg(
                absDefVLNV.toString()));
			return;
		}
	}

	// if no abstraction definition has been specified.
	else
    {
		editor = new BusDefinitionEditor(this, libraryHandler_, busDef);
	}

    designTabs_->addAndOpenDocument(editor, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: openDesign()
//-----------------------------------------------------------------------------
void MainWindow::openDesign(VLNV const& vlnv, QString const& viewName , bool forceUnlocked)
{
	// The vlnv must always be for a component.
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// Parse the referenced component.
	QSharedPointer<Component> comp = libraryHandler_->getModel(vlnv).staticCast<Component>();
	
	// Check if the design is already open.
	VLNV refVLNV = comp->getHierRef(viewName);
	VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
	if (isOpen(designVLNV) || hasInvalidReferences(comp->getHierRefs(), vlnv))
    {
		return;
	}

    DesignWidgetFactoryImplementation factory(libraryHandler_);
	DesignWidget* designWidget = factory.makeHWDesignWidget(this);
	
    // open the design in the designWidget
	designWidget->setDesign(vlnv, viewName);

	// if the design could not be opened
	if (designWidget->getOpenDocument().isEmpty())
    {
		delete designWidget;
		return;
	}

    connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modeChanged(DrawMode)),
        this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SLOT(openDesign(const VLNV&, const QString&)));
    connect(designWidget, SIGNAL(openComponent(const VLNV&)),
        this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(openBus(VLNV const&, VLNV const&, bool)),
        this, SLOT(openBus(VLNV const&, VLNV const&, bool)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
        this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
        this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
        this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(refreshed()), this, SLOT(onDesignDocumentRefreshed()), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(designWidget, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openMemoryDesign()
//-----------------------------------------------------------------------------
void MainWindow::openMemoryDesign()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
		
	// if there is no currently selected tab
	if (!doc)
    {
		return;
	}

    libraryHandler_->onOpenMemoryDesign(doc->getDocumentVLNV());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openMemoryDesign()
//-----------------------------------------------------------------------------
void MainWindow::openMemoryDesign(const VLNV& vlnv, const QString& viewName, bool forceUnlocked)
{
    // the vlnv must always be for a component
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    // parse the referenced component
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> comp = libComp.staticCast<Component>();

    // check if the design is already open
    VLNV refVLNV = comp->getHierRef(viewName);
    VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);

    QString memoryDesignName = designVLNV.getName();

    int extensionBegin = memoryDesignName.size() - 7;
    memoryDesignName.replace(extensionBegin, 7, ".memoryDesign");
    designVLNV.setName(memoryDesignName);

    if (isOpen(designVLNV))
    {
        return;
    }

    // make sure that all component's hierarchy refs are valid
    // if there was at least one invalid reference then do not open the design
    if (hasInvalidReferences(comp->getHierRefs(), vlnv))
    {
        return;
    }

    MemoryDesignDocument* memoryDesignWidget = new MemoryDesignDocument(libraryHandler_, this);
    
    memoryDesignWidget->getDocumentName();

    connect(memoryDesignWidget, SIGNAL(errorMessage(const QString&)),
        console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);

    if (!memoryDesignWidget->setDesign(vlnv, viewName))
    {
        delete memoryDesignWidget;
        return;
    }

    connect(memoryDesignWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);

    connect(memoryDesignWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(memoryDesignWidget, SIGNAL(clearItemSelection()),
        libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);

    connect(memoryDesignWidget, SIGNAL(clearItemSelection()), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(memoryDesignWidget, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: openSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSWDesign(const VLNV& vlnv, QString const& viewName, bool forceUnlocked)
{
	// the vlnv must always be for a component
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// parse the referenced component
	QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();

	// check if the design is already open
	VLNV refVLNV = comp->getHierSWRef(viewName);
	VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
	if (isOpen(designVLNV))
    {
		return;
	}

	SystemDesignWidget* designWidget = new SystemDesignWidget(true, libraryHandler_, this);

    connect(designWidget, SIGNAL(errorMessage(const QString&)),
        console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(noticeMessage(const QString&)),
        console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

	if (!designWidget->setDesign(vlnv, viewName))
	{
		delete designWidget;
		return;
	}

    disconnect(designWidget, SIGNAL(errorMessage(const QString&)), console_, SLOT(onErrorMessage(const QString&)));
    disconnect(designWidget, SIGNAL(noticeMessage(const QString&)), console_, SLOT(onNoticeMessage(const QString&)));

	connect(designWidget, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SLOT(openSWDesign(const VLNV&, const QString&)));
	connect(designWidget, SIGNAL(openCSource(ComponentItem*)),
		this, SLOT(openCSource(ComponentItem*)), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
		this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
		this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
		this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(refreshed()), this, SLOT(onDesignDocumentRefreshed()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modeChanged(DrawMode)),
		this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modifiedChanged(bool)), actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(designWidget, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: openSystemDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSystemDesign(VLNV const& vlnv, QString const& viewName, bool forceUnlocked)
{
	// the vlnv must always be for a component
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// parse the referenced component
	QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();

	// check if the design is already open
	VLNV refVLNV = comp->getHierSystemRef(viewName);
	VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
	if (isOpen(designVLNV))
    {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	SystemDesignWidget* designWidget = new SystemDesignWidget(false, libraryHandler_, this);

    connect(designWidget, SIGNAL(errorMessage(const QString&)),
        console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(noticeMessage(const QString&)),
        console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

	if (!designWidget->setDesign(vlnv, viewName))
	{
		delete designWidget;
		QApplication::restoreOverrideCursor();
		return;
	}

    disconnect(designWidget, SIGNAL(errorMessage(const QString&)), console_, SLOT(onErrorMessage(const QString&)));
    disconnect(designWidget, SIGNAL(noticeMessage(const QString&)), console_, SLOT(onNoticeMessage(const QString&)));

	connect(designWidget, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SLOT(openSWDesign(const VLNV&, const QString&)));
	connect(designWidget, SIGNAL(openCSource(ComponentItem*)),
		this, SLOT(openCSource(ComponentItem*)), Qt::UniqueConnection);
    
	connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
		this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
        this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
        this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(destroyed(QObject*)),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(refreshed()), this, SLOT(onDesignDocumentRefreshed()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modeChanged(DrawMode)),
		this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(designWidget, forceUnlocked);

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::openComponent()
//-----------------------------------------------------------------------------
void MainWindow::openComponent( const VLNV& vlnv, bool forceUnlocked )
{
	if (isOpen(vlnv))
    {
		return;
	}

	// component editor was not yet open so create it

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QSharedPointer<Component> component;

	if (libraryHandler_->contains(vlnv))
    {
		QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
		component = libComp.dynamicCast<Component>();
	}
	else
    {
		emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
		QApplication::restoreOverrideCursor();
		return;
	}

	if (!component)
    {
		emit errorMessage(tr("Document type did not match Component"));
		QApplication::restoreOverrideCursor();
		return;
	}

	ComponentEditor* editor = new ComponentEditor(libraryHandler_, *pluginMgr_, component, this);
    
	QString styleSheet("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }");
	editor->setStyleSheet(styleSheet);

	connect(editor, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
            this , SLOT(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SLOT(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openBus(const VLNV&, const VLNV&)),
		this, SLOT(openBus(const VLNV&, const VLNV&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openComDefinition(const VLNV&)),
		this, SLOT(openComDefinition(const VLNV&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SLOT(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
		this, SLOT(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(editor, forceUnlocked);

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openComDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openComDefinition(VLNV const& vlnv, bool forceUnlocked /*= false*/)
{
	if (isOpen(vlnv))
    {
		return;
	}

    // Editor was not yet open so create it.
    QSharedPointer<ComDefinition> comDef;

    if (libraryHandler_->contains(vlnv))
    {
        QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
        comDef = libComp.dynamicCast<ComDefinition>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        return;
    }

    if (comDef == 0)
    {
        emit errorMessage(tr("Document type did not match COM Definition"));
        return;
    }

    ComDefinitionEditor* editor = new ComDefinitionEditor(this, libraryHandler_, comDef);
    designTabs_->addAndOpenDocument(editor, forceUnlocked);
}


//-----------------------------------------------------------------------------
// Function: MainWindow::openApiDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openApiDefinition(VLNV const& vlnv, bool forceUnlocked /*= false*/)
{
	if (isOpen(vlnv))
    {
		return;
	}

	// Editor was not yet open so create it.
    QSharedPointer<ApiDefinition> apiDef;

    if (libraryHandler_->contains(vlnv))
    {
        QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
        apiDef = libComp.dynamicCast<ApiDefinition>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        return;
    }

    if (apiDef == 0)
    {
        emit errorMessage(tr("Document type did not match API Definition"));
        return;
    }

    ApiDefinitionEditor* editor = new ApiDefinitionEditor(this, libraryHandler_, apiDef);
    designTabs_->addAndOpenDocument(editor, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isOpen()
//-----------------------------------------------------------------------------
bool MainWindow::isOpen( const VLNV& vlnv ) const
{
	if (!vlnv.isValid())
    {
		return false;
	}

	for (int i = 0; i < designTabs_->count(); i++)
    {
		TabDocument* document = dynamic_cast<TabDocument*>(designTabs_->widget(i));

		// if the document is already open is some tab
		if (document && document->getIdentifyingVLNV() == vlnv)
        {
			designTabs_->setCurrentIndex(i);
			return true;
		}
	}
	// document was not open
	return false;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setWindowVisibilityForSupportedWindow()
//-----------------------------------------------------------------------------
void MainWindow::setWindowVisibilityForSupportedWindow(TabDocument::SupportedWindows type, bool show)
{
    if (isSupportedWindowType(type))
    {
        setWindowVisibility(type, show);
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setWindowVisibility()
//-----------------------------------------------------------------------------
void MainWindow::setWindowVisibility(TabDocument::SupportedWindows windowType, bool show)
{
    visibilities_.insert(windowType, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::updateWindows()
//-----------------------------------------------------------------------------
void MainWindow::updateWindows()
{
    updateWindowAndControlVisibility(TabDocument::LIBRARYWINDOW, libraryDock_);
    updateWindowAndControlVisibility(TabDocument::OUTPUTWINDOW, consoleDock_);
    updateWindowAndControlVisibility(TabDocument::CONTEXT_HELP_WINDOW, contextHelpDock_);
    updateWindowAndControlVisibility(TabDocument::PREVIEWWINDOW, previewDock_);
    updateWindowAndControlVisibility(TabDocument::CONFIGURATIONWINDOW, configurationDock_);
    updateWindowAndControlVisibility(TabDocument::SYSTEM_DETAILS_WINDOW, systemDetailsDock_);
    updateWindowAndControlVisibility(TabDocument::CONNECTIONWINDOW, connectionDock_);
    updateWindowAndControlVisibility(TabDocument::INTERFACEWINDOW, interfaceDock_);
    updateWindowAndControlVisibility(TabDocument::INSTANCEWINDOW, instanceDock_);
    updateWindowAndControlVisibility(TabDocument::ADHOCVISIBILITY_WINDOW, adHocVisibilityDock_);
    updateWindowAndControlVisibility(TabDocument::ADHOC_WINDOW, adhocDock_);
    //updateWindowAndControlVisibility(TabDocument::ADDRESS_WINDOW, addressDock_);   
}

//-----------------------------------------------------------------------------
// Function: MainWindow::updateWindowControlVisibility()
//-----------------------------------------------------------------------------
void MainWindow::updateWindowAndControlVisibility(TabDocument::SupportedWindows windowType, QDockWidget* dock)
{
    QAction* showAction = dock->toggleViewAction(); 
   
    if (isSupportedWindowType(windowType))
    {
        showAction->setVisible(true);
        dock->setVisible(visibilities_.value(windowType));
    }
    else
    {
        showAction->setVisible(false);
        dock->hide();
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isSupportedWindowType()
//-----------------------------------------------------------------------------
bool MainWindow::isSupportedWindowType(TabDocument::SupportedWindows windowType)
{
    return currentlySupportedWindows() & windowType;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::supportedWindows()
//-----------------------------------------------------------------------------
unsigned int MainWindow::currentlySupportedWindows()
{
    unsigned int windows = defaultWindows();

    if (designTabs_->count() != 0)
    {
        TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
        windows = doc->getSupportedWindows();
    }

    return windows;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::defaultWindows()
//-----------------------------------------------------------------------------
unsigned int MainWindow::defaultWindows()
{
    return TabDocument::OUTPUTWINDOW | TabDocument::LIBRARYWINDOW | 
        TabDocument::PREVIEWWINDOW | TabDocument::CONTEXT_HELP_WINDOW;        
}

//-----------------------------------------------------------------------------
// Function: MainWindow::updateVisibilityControlMenu()
//-----------------------------------------------------------------------------
void MainWindow::updateVisibilityControlMenu(TabDocument* document)
{
    visibilityMenu_.clear();

    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(false);

    QMapIterator<QString, bool> iter(document->getVisibilityControls());
    while (iter.hasNext())
    {
        iter.next();

        QAction* action = new QAction(tr(qPrintable(iter.key())), this);
        action->setCheckable(true);
        action->setChecked(iter.value());

        group->addAction(action);
        visibilityMenu_.addAction(action);
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::connectVisibilityControls()
//-----------------------------------------------------------------------------
void MainWindow::connectVisibilityControls()
{
    // Action to show/hide the output window.
    connect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onOutputAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the context help window.
    connect(contextHelpDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onContextHelpAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the preview box.
    connect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onPreviewAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the library window.
    connect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onLibraryAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the configuration window.
    connect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onConfigurationAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the system detail window.
    connect(systemDetailsDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onSystemDetailsAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the connection editor.
    connect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onConnectionAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the interface editor.
    connect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onInterfaceAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the instance editor.
    connect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onInstanceAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the ad-hoc visibility editor.
    connect(adHocVisibilityDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onAdHocVisibilityAction(bool)), Qt::UniqueConnection);

    connect(adhocDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onAdHocEditorAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the address editor.
    /*connect(addressDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onAddressAction(bool)), Qt::UniqueConnection);*/
}

//-----------------------------------------------------------------------------
// Function: MainWindow::disconnectVisibilityControls()
//-----------------------------------------------------------------------------
void MainWindow::disconnectVisibilityControls()
{
    disconnect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onOutputAction(bool)));

    disconnect(contextHelpDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onContextHelpAction(bool)));

    disconnect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onPreviewAction(bool)));

    disconnect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onLibraryAction(bool)));	

    disconnect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onConfigurationAction(bool)));

    disconnect(systemDetailsDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onSystemDetailsAction(bool)));

    disconnect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onConnectionAction(bool)));

    disconnect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onInterfaceAction(bool)));

    disconnect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onInstanceAction(bool)));

    disconnect(adHocVisibilityDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onAdHocVisibilityAction(bool)));

    disconnect(adhocDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onAdHocEditorAction(bool)));

    //disconnect(addressDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onAddressAction(bool)));
}

//-----------------------------------------------------------------------------
// Function: openCSource()
//-----------------------------------------------------------------------------
void MainWindow::openCSource(ComponentItem* compItem)
{
	Q_ASSERT(compItem != 0);

	QSharedPointer<FileSet> fileSet = compItem->componentModel()->getFileSet("cSources");

	if (fileSet == 0 || fileSet->getFiles()->count() == 0)
	{
		return;
	}

	QString filename;

	if (fileSet->getFiles()->count() > 1)
	{
		// Show a dialog for selecting what source file to open.
		ListSelectDialog saveDialog(this);
		saveDialog.setWindowTitle(tr("Open Source"));
		saveDialog.setDescription(tr("Select C source file to open:"));

		foreach (QSharedPointer<File> file, *fileSet->getFiles())
		{
			saveDialog.addItem(new QListWidgetItem(file->name()));
		}

		if (saveDialog.exec() == QDialog::Rejected)
		{
			return;
		}

		filename = saveDialog.getSelectedItem()->text();
	}
	else
	{
		// Otherwise there is only one possibility.
		filename = fileSet->getFiles()->first()->name();
	}

	if (compItem->componentModel()->getVlnv().isValid())
	{
		filename = General::getAbsolutePath(libraryHandler_->getPath(compItem->componentModel()->getVlnv()),
			filename);
	}

	openCSource(filename, compItem->componentModel());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openCSource()
//-----------------------------------------------------------------------------
void MainWindow::openCSource(QString const& filename, QSharedPointer<Component> component)
{
	// Check if the source is already open and activate it.
	for (int i = 0; i < designTabs_->count(); i++)
	{
		CSourceWidget* sourceWidget = dynamic_cast<CSourceWidget*>(designTabs_->widget(i));

		if (sourceWidget != 0 && sourceWidget->getSourceFile() == filename)
		{
			designTabs_->setCurrentIndex(i);
			return;
		}
	}

	// Otherwise make sure that the file exists.
	if (!QFile::exists(filename))
	{
		QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
			"The source file " + filename + " is not found!", QMessageBox::Ok, this);
		msgBox.exec();
		return;
	}

	// Open the source to a view.
	CSourceWidget* sourceWidget = new CSourceWidget(filename, component, libraryHandler_, this, this);
    designTabs_->addAndOpenDocument(sourceWidget, true);
}

//-----------------------------------------------------------------------------
// Function: changeProtection()
//-----------------------------------------------------------------------------
void MainWindow::changeProtection(bool locked)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
		
	// if there is no currently selected tab
	if (!doc)
    {
		return;
	}

	VLNV docVLNV = doc->getDocumentVLNV();
    Q_ASSERT(docVLNV.isValid());
	
	// if user set the document to be locked and it has been modified
	if (locked)
	{
		if (doc->isModified())
		{
            // ask user if he wants to save the changes made to document
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("The document has been modified. The changes need to be saved before the "
                "document can be locked. Save changes and continue?"), QMessageBox::Yes | QMessageBox::No, this);

			// if user does not want to save or save can't be done
			if (msgBox.exec() == QMessageBox::No || !doc->save())
			{
				actProtect_->setChecked(false);
				return;
			}
		}
		// lock the document
		doc->setProtection(locked);
    }

	//If user wants to unlock the document.
    else
    {
        // The list containing tabs for the same document.
        QVector<TabDocument*> otherDocs;

        // If the same document has been edited in another editor.
        TabDocument* docToSave = 0;

		for (int i = 0; i < designTabs_->count(); i++)
        {
			TabDocument* otherDoc = static_cast<TabDocument*>(designTabs_->widget(i));
			Q_ASSERT(otherDoc);

			// if the other document is for the same component
			if (otherDoc != doc && otherDoc->getDocumentVLNV() == docVLNV)
            {
				otherDocs.append(otherDoc);

                // If there is a document that is not locked and has been modified.
                if (!otherDoc->isProtected() && otherDoc->isModified())
                {
                    // there should only be one doc at a time that can be modified
                    Q_ASSERT(!docToSave);

                    // this was the document that was modified
                    docToSave = otherDoc;
                }
			}
		}

        // if there was document that was unlocked and modified
        if (docToSave)
        {
            // Ask the user if he wants to save and switch locks.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("The document is being edited in another tab and has unsaved changes. Changes need to be saved "
                   "before this tab can be unlocked. Save changes and switch locks?"),
                QMessageBox::Yes | QMessageBox::No, this);

            // Restore the lock if the user canceled.
            if (msgBox.exec() == QMessageBox::No)
            {
                actProtect_->setChecked(true);
                return;
            }

            // if the document could not be saved
            if (!docToSave->save())
            {
                return;
            }
        }

        bool previouslyUnlocked = doc->isPreviouslyUnlocked();

        // User wanted to unlock this doc so go through other documents to see if they were unlocked.
        foreach (TabDocument* otherDoc, otherDocs)
        {
            otherDoc->setProtection(true);

            if (otherDoc->isPreviouslyUnlocked())
            {
                previouslyUnlocked = true;
            }
        }

        if (!previouslyUnlocked)
        {
            QString message = tr("Are you sure you want to unlock the document?\n");
            QString detailMsg = "";

            // If edited document was component and has been instantiated in other components, print them.
            if (docVLNV.isValid() && libraryHandler_->getDocumentType(docVLNV) == VLNV::COMPONENT)
            {
                QList<VLNV> list;
                int refCount = libraryHandler_->getOwners(list, docVLNV);
                if (refCount > 0)
                {
                    message += tr("Changes to the document can affect %1 other documents.").arg(refCount);

                    detailMsg += tr("The component has been instantiated in the following %1 component(s):\n").arg(
                        refCount);
                    foreach (VLNV const& owner, list)
                    {
                        detailMsg += "* " + owner.toString() + "\n"; 
                    }
                }
                else
                {
                    message += tr("Changes to the document can affect other documents. ");
                }
            }
            else
            {
                message += tr("Changes to the document can affect other documents. ");
            }

            message += "If you choose yes, this will not be asked next time for this document.";

            // Ask verification from the user.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(), message,
                QMessageBox::Yes | QMessageBox::No, this);
            
            msgBox.setDetailedText(detailMsg);

            // if user did not want to unlock the document
            if (msgBox.exec() == QMessageBox::No)
            {
                actProtect_->setChecked(true);
                return;
            }
        }

        // mark the other documents also unlocked to keep the unlock history in sync.
        foreach (TabDocument* otherDoc, otherDocs)
        {
            otherDoc->setPreviouslyUnlocked();
        }

        // Unlock the document.
        doc->setProtection(locked);
	}

	DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
	if (designwidget)
    {
		// update the editors to match the locked state
		configurationEditor_->setLocked(locked);
        instanceEditor_->setProtection(locked);

        if (designwidget->getSupportedWindows() & TabDocument::SYSTEM_DETAILS_WINDOW)
        {
            systemDetailsEditor_->setLocked(locked);
        }
    }
	else
    {
		configurationEditor_->setLocked(true);
        systemDetailsEditor_->setLocked(true);
    }

	// Clear the item selection since the current instance is no longer valid.
    onClearItemSelection();

	onProtectionChanged(locked);

	updateMenuStrip();
}

//-----------------------------------------------------------------------------
// Function: onProtectionChanged()
//-----------------------------------------------------------------------------
void MainWindow::onProtectionChanged(bool locked)
{
	if (locked)
	{
		actProtect_->setIcon(QIcon(":icons/common/graphics/protection-locked.png"));
		actProtect_->setText(tr("Locked"));
	}
	else
	{
		actProtect_->setIcon(QIcon(":icons/common/graphics/protection-unlocked.png"));
		actProtect_->setText(tr("Unlocked"));
	}
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void MainWindow::undo()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
		doc->getEditProvider()->undo();
	}
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void MainWindow::redo()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
		doc->getEditProvider()->redo();
	}
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createSWComponent()
//-----------------------------------------------------------------------------
void MainWindow::createSWComponent(VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	// Create a component.
	QSharedPointer<Component> component = QSharedPointer<Component>(new Component());
	component->setVlnv(vlnv);
    component->setVersion(VersionHelper::versionFileStr());

	// Set Kactus attributes.
    component->setImplementation(KactusAttribute::SW);

    // Create the file.
    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    runComponentWizard(component, directory);    

	openComponent(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: showAbout()
//-----------------------------------------------------------------------------
void MainWindow::showAbout()
{
	SplashScreen* splash = new SplashScreen(VersionHelper::createVersionString(), this);
	splash->setAttribute(Qt::WA_DeleteOnClose);
	splash->setWindowFlags(splash->windowFlags() & ~(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint));
	splash->move(this->mapToGlobal(this->rect().center() - splash->rect().center()));
	splash->setWindowModality(Qt::ApplicationModal);
	splash->show();
	splash->showMessage("");
}

//-----------------------------------------------------------------------------
// Function: showAbout()
//-----------------------------------------------------------------------------
void MainWindow::showHelp()
{
    helpWnd_->show();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::selectVisibleDocks()
//-----------------------------------------------------------------------------
void MainWindow::selectVisibleDocks()
{
	windowsMenu_.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
// Function: mainwindow::hideEvent()
//-----------------------------------------------------------------------------
void MainWindow::hideEvent( QHideEvent* event ) 
{
    disconnectVisibilityControls();
	QMainWindow::hideEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::showEvent()
//-----------------------------------------------------------------------------
void MainWindow::showEvent( QShowEvent* event ) 
{
    connectVisibilityControls();
	QMainWindow::showEvent(event);
}

//-----------------------------------------------------------------------------
// Function: openWorkspaceMenu()
//-----------------------------------------------------------------------------
void MainWindow::openWorkspaceMenu()
{
    workspaceMenu_.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onWorkspaceChanged()
//-----------------------------------------------------------------------------
void MainWindow::onWorkspaceChanged(QAction* action)
{
    saveWorkspace(curWorkspaceName_);

    curWorkspaceName_ = action->text();

	QSettings settings;
	settings.setValue("Workspaces/CurrentWorkspace", curWorkspaceName_);

    loadWorkspace(curWorkspaceName_);

	actWorkspaces_->setText(curWorkspaceName_);

	applySettingsToOpenDocuments();
}

//-----------------------------------------------------------------------------
// Function: onNewWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::onNewWorkspace()
{
    NewWorkspaceDialog saveDialog(this);

    if (saveDialog.exec() == QDialog::Accepted)
    {
		saveWorkspace(curWorkspaceName_);

		createNewWorkspace(saveDialog.name());
		copyComponentEditorSettings(saveDialog.name());

        saveWorkspace(saveDialog.name());
        curWorkspaceName_ = saveDialog.name();

		QSettings settings;
		settings.setValue("Workspaces/CurrentWorkspace", curWorkspaceName_);

        updateWorkspaceMenu();
    }
}

//-----------------------------------------------------------------------------
// Function: onDeleteWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::onDeleteWorkspace()
{
    DeleteWorkspaceDialog saveDialog(this);

    // Fill in the dialog with existing workspace names.
    QSettings settings;
    settings.beginGroup("Workspaces");

    QStringList workspaces = settings.childGroups();

    foreach (QString const& workspace, workspaces)
    {
        if (workspace != "Default" && workspace != curWorkspaceName_)
        {        
            saveDialog.addWorkspaceName(workspace);
        }
    }

    settings.endGroup();

    // Execute the dialog.
    if (saveDialog.exec() == QDialog::Accepted)
    {
        // Remove the workspace from the settings and update the workspace menu.
        settings.remove("Workspaces/" + saveDialog.name());
        updateWorkspaceMenu();
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onOutputAction()
//-----------------------------------------------------------------------------
void MainWindow::onOutputAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::OUTPUTWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onContextHelpAction()
//-----------------------------------------------------------------------------
void MainWindow::onContextHelpAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::CONTEXT_HELP_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onPreviewAction()
//-----------------------------------------------------------------------------
void MainWindow::onPreviewAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::PREVIEWWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onLibraryAction()
//-----------------------------------------------------------------------------
void MainWindow::onLibraryAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::LIBRARYWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onConfigurationAction()
//-----------------------------------------------------------------------------
void MainWindow::onConfigurationAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::CONFIGURATIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onSystemDetailsAction()
//-----------------------------------------------------------------------------
void MainWindow::onSystemDetailsAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::SYSTEM_DETAILS_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onConnectionAction()
//-----------------------------------------------------------------------------
void MainWindow::onConnectionAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::CONNECTIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInterfaceAction()
//-----------------------------------------------------------------------------
void MainWindow::onInterfaceAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::INTERFACEWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onAdHocAction()
//-----------------------------------------------------------------------------
void MainWindow::onAdHocVisibilityAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::ADHOCVISIBILITY_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onAdHocEditorAction()
//-----------------------------------------------------------------------------
void MainWindow::onAdHocEditorAction(bool show)
{
    setWindowVisibilityForSupportedWindow(TabDocument::ADHOC_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onAddressAction()
//-----------------------------------------------------------------------------
void MainWindow::onAddressAction(bool show)
{
    setWindowVisibilityForSupportedWindow(TabDocument::ADDRESS_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInstanceAction()
//-----------------------------------------------------------------------------
void MainWindow::onInstanceAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::INSTANCEWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: openVisibilityControlMenu()
//-----------------------------------------------------------------------------
void MainWindow::openVisibilityControlMenu()
{
    visibilityMenu_.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
// Function: onVisibilityControlToggled()
//-----------------------------------------------------------------------------
void MainWindow::onVisibilityControlToggled(QAction* action)
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    Q_ASSERT(doc != 0);

    doc->setVisibilityControlState(action->text(), !doc->getVisibilityControls().value(action->text()));
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isDesignOpen()
//-----------------------------------------------------------------------------
bool MainWindow::isDesignOpen(VLNV const& vlnv, KactusAttribute::Implementation implementation) const
{
    if (vlnv.isValid())
    {
        for (int i = 0; i < designTabs_->count(); i++)
        {
            DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->widget(i));

            if (designWidget != 0 && designWidget->getOpenDocument() == vlnv &&
                designWidget->getImplementation() == implementation)
            {
                designTabs_->setCurrentIndex(i);
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::hasInvalidReferences()
//-----------------------------------------------------------------------------
bool MainWindow::hasInvalidReferences(QList<VLNV> hierRefs, VLNV const& referencingVlnv)
{
    bool invalidReferences = false;
    foreach (VLNV ref, hierRefs)
    {
        // if the hierarchy referenced object is not found in library
        if (!libraryHandler_->contains(ref))
        {
            emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                " which is not found in library. Component is not valid and can not "
                "be opened in design view. Edit component with component editor to "
                "remove invalid references.").arg(referencingVlnv.toString(":")).arg(ref.toString(":")));
            invalidReferences = true;
            continue;
        }

        // if the reference is to a wrong object type
        else if (libraryHandler_->getDocumentType(ref) != VLNV::DESIGN &&
            libraryHandler_->getDocumentType(ref) != VLNV::DESIGNCONFIGURATION)
        {
            emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                " which is not design or design configuration. Component is not valid and"
                " can not be opened in design view. Edit component with component editor to"
                " remove invalid references.").arg(referencingVlnv.toString(":")).arg(ref.toString(":")));
            invalidReferences = true;
            continue;
        }

        // if the reference is for a design configuration then check that also
        // the design is found
        else if (libraryHandler_->getDocumentType(ref) == VLNV::DESIGNCONFIGURATION)
        {
            VLNV designVLNV = libraryHandler_->getDesignVLNV(ref);

            QSharedPointer<Document> libComp2 = libraryHandler_->getModel(ref);
            QSharedPointer<DesignConfiguration> desConf = libComp2.staticCast<DesignConfiguration>();
            VLNV refToDesign = desConf->getDesignRef();

            // if the referenced design was not found in the library
            if (!designVLNV.isValid())
            {
                emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                    " which is design configuration and references to design %3. This "
                    "design is not found in library so component can not be opened in "
                    "design view. Edit component with component editor to remove "
                    "invalid references").arg(
                    referencingVlnv.toString(":")).arg(
                    ref.toString(":")).arg(
                    refToDesign.toString(":")));
                invalidReferences = true;
                continue;
            }
        }
    }

    return invalidReferences;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setLibraryLocations()
//-----------------------------------------------------------------------------
void MainWindow::setLibraryLocations()
{
    QSettings settings;
    LibrarySettingsDialog dialog(settings, this);
    connect(&dialog, SIGNAL(scanLibrary()), this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

    dialog.exec();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onRunImportWizard()
//-----------------------------------------------------------------------------
void MainWindow::onRunImportWizard()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (!doc || doc->isProtected())
    {
        return;
    }

    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("The document has been modified. The changes need to be saved before the "
            "import wizard can be run. Save changes and continue?"),
            QMessageBox::Yes | QMessageBox::No, this);

        // if user does not want to save or save can't be done
        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    VLNV vlnv = doc->getIdentifyingVLNV();
    QSharedPointer<const Component> component = libraryHandler_->getModelReadOnly(vlnv).dynamicCast<const Component>();

    if (component)
    {
        ImportWizard wizard(component, *pluginMgr_.data(), libraryHandler_, this);

        if (wizard.exec() == QDialog::Accepted)
        {
            bool succesfulSave = libraryHandler_->writeModelToFile(wizard.getComponent());

            if(succesfulSave)
            {
                doc->refresh();
            }
            else
            {
                emit errorMessage("Error saving file to disk.");
            }        
        }    
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createGeneratorPluginActions()
//-----------------------------------------------------------------------------
void MainWindow::createGeneratorPluginActions()
{
    pluginActionGroup_ = new QActionGroup(this);

    foreach (IPlugin* plugin, pluginMgr_->getActivePlugins())
    {
        IGeneratorPlugin* genPlugin = dynamic_cast<IGeneratorPlugin*>(plugin);

        if (genPlugin != 0)
        {
            // Add a small overlay icon to the plugin icon to visualize that this is a plugin.
            QIcon pluginBaseIcon = genPlugin->getIcon();

			QPixmap icon(32, 32);
            icon.fill(Qt::transparent);

            QPainter painter(&icon);
			painter.drawPixmap(0, 0, pluginBaseIcon.pixmap(32, 32));

            QAction* action = new QAction(icon, genPlugin->getName(), this);
            action->setData(qVariantFromValue((void*)genPlugin));

            generationGroup_->addAction(action);
            pluginActionGroup_->addAction(action);

			generationGroup_->widgetForAction(action)->installEventFilter(ribbon_);
        }
    }

    connect(pluginActionGroup_, SIGNAL(triggered(QAction*)),
            this, SLOT(runGeneratorPlugin(QAction*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::updateGeneratorPluginActions()
//-----------------------------------------------------------------------------
void MainWindow::updateGeneratorPluginActions()
{
    // Remove all plugin actions.
    QList<QAction*> actions = generationGroup_->actions();

    foreach (QAction* action, actions)
    {
        // Check if the action contains the plugin pointer.
        if (action->data().value<void*>() != 0)
        {
            generationGroup_->removeAction(action);
            delete action;
        }
    }

    delete pluginActionGroup_;
    
    // Recreate the plugin actions.
    createGeneratorPluginActions();

	setPluginVisibilities();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::runComponentWizard()
//-----------------------------------------------------------------------------
void MainWindow::runComponentWizard(QSharedPointer<Component> component, QString const& directory)
{
    // Open the component wizard.
    ComponentWizard wizard(component, directory, *pluginMgr_, libraryHandler_, this);

    QString styleSheet("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }");
    wizard.setStyleSheet(styleSheet);

    if(wizard.exec() == QDialog::Accepted)
    {
        // Save wizard changes.
        if (!libraryHandler_->writeModelToFile(wizard.getComponent()))
        {
            emit errorMessage("Error saving file to disk.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setPluginVisibilities()
//-----------------------------------------------------------------------------
void MainWindow::setPluginVisibilities()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	// Enable/disable the plugin generator actions based on the component being edited in the document.
	VLNV compVLNV;
	VLNV desVLNV;
	QSharedPointer<Document const> libComp;
	QSharedPointer<Document const> libDes;
    QSharedPointer<Document const> libDesConf;
    if (doc != 0)
    {
        compVLNV = doc->getDocumentVLNV();
        desVLNV = doc->getIdentifyingVLNV();
        libComp = libraryHandler_->getModelReadOnly(compVLNV);

        // if the design is supported by the document type
        DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
        if (desWidget)
        {
            // design is the object that identifies the editor
            libDes = libraryHandler_->getModelReadOnly(desVLNV);

            // find the design config is one exists
			QString viewName = desWidget->getOpenViewName();

			// The name must exist!
			Q_ASSERT(!viewName.isEmpty());

            QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
            VLNV desConfVLNV;

            // the implementation defines where to search for the hierarchy ref
            switch (desWidget->getImplementation()) {
            case KactusAttribute::HW: {
                desConfVLNV = comp->getHierRef(viewName);
                break;
                                      }
            case KactusAttribute::SW: {
                desConfVLNV = comp->getHierSWRef(viewName);
                break;
                                      }
            case KactusAttribute::SYSTEM: {
                desConfVLNV = comp->getHierSystemRef(viewName);
                break;
                                          }
            default: {
                Q_ASSERT(false);
                return;
                     }
            }

            // if the hierarchy ref is not directly to the design but design config is in between
            if (desConfVLNV.isValid() && desConfVLNV != desVLNV)
            {
                libDesConf = libraryHandler_->getModelReadOnly(desConfVLNV);
            }
        }
    }

    bool isGenerationGroupVisible = false;
	foreach (QAction* action, pluginActionGroup_->actions())
	{
		IGeneratorPlugin* plugin = reinterpret_cast<IGeneratorPlugin*>(action->data().value<void*>());
		Q_ASSERT(plugin != 0);

		action->setVisible(libComp != 0 && plugin->checkGeneratorSupport(libComp, libDesConf, libDes));
        if (action->isVisible())
        {
            isGenerationGroupVisible = true;
        }
	}

    foreach (QAction* action, generationGroup_->actions())
    {
        if (action->isVisible())
        {
            isGenerationGroupVisible = true;
            break;
        }
    }

    generationGroup_->setVisible(isGenerationGroupVisible);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onDesignDocumentRefreshed()
//-----------------------------------------------------------------------------
void MainWindow::onDesignDocumentRefreshed()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    DesignWidget* designWidget = dynamic_cast<DesignWidget*>(doc);

    if (designWidget)
    {
        QSharedPointer<Document> topItem = libraryHandler_->getModel(doc->getDocumentVLNV());
        QSharedPointer<Component> topComponent = topItem.dynamicCast<Component>();

        instanceEditor_->setContext(topComponent, designWidget->getDiagram()->getDesignConfiguration(),
            designWidget->getDiagram()->getEditProvider());
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onConfigureViews()
//-----------------------------------------------------------------------------
void MainWindow::onConfigureViews()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (!doc || doc->isProtected())
    {
        return;
    }

    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(), tr("The document has been "
            "modified. The changes need to be saved before the import wizard can be run. Save changes and "
            "continue?"), QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    VLNV componentVLNV = doc->getDocumentVLNV();
    VLNV designVLNV = doc->getIdentifyingVLNV();

    QSharedPointer<Component> currentComponent = libraryHandler_->getModel(componentVLNV).dynamicCast<Component>();
    QSharedPointer<Design> libraryDesign;
    QSharedPointer<DesignConfiguration> libraryDesignConfiguration;

    DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
    QString viewName ("");

    if (desWidget && componentVLNV != designVLNV)
    {
        libraryDesign = libraryHandler_->getModel(designVLNV).dynamicCast<Design>();

        viewName = desWidget->getOpenViewName();

        VLNV designConfigurationVLNV;

        if (desWidget->getImplementation() == KactusAttribute::HW)
        {
            designConfigurationVLNV = currentComponent->getHierRef(viewName);
        }
        else
        {
            return;
        }

        if (designConfigurationVLNV.isValid() && designConfigurationVLNV != designVLNV)
        {
            libraryDesignConfiguration = libraryHandler_->getModel(designConfigurationVLNV).
                dynamicCast<DesignConfiguration>();
        }
    }

    if (currentComponent)
    {
        ViewConfigurer configurer(currentComponent, libraryDesign, libraryDesignConfiguration, libraryHandler_,
            viewName);

        if (configurer.exec() == QDialog::Accepted)
        {
            doc->refresh();
        }
    }
}
