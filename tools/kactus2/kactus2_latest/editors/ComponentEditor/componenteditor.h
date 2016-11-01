//-----------------------------------------------------------------------------
// File: componenteditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.5.2012
//
// Description:
// The editor to edit/packet IP-Xact components.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITOR_H
#define COMPONENTEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <editors/ComponentEditor/treeStructure/componenttreeview.h>
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/treeStructure/componenteditorgroupslot.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorTreeSortProxyModel.h>
#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/Component/validators/ComponentValidator.h>

#include <QSharedPointer>
#include <QSplitter>
#include <QSettings>

class LibraryInterface;
class PluginManager;
class Component;

class ExpressionParser;
//-----------------------------------------------------------------------------
//! The editor to edit/packet IP-Xact components.
//-----------------------------------------------------------------------------
class ComponentEditor : public TabDocument
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] libHandler  The instance that manages the component library.
     *      @param [in] pluginMgr   The instance that manages the plugins.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      Parent widget of the editor.
	 *
	 */
	ComponentEditor(LibraryInterface* libHandler, 
        PluginManager& pluginMgr,
		QSharedPointer<Component> component, 
		QWidget *parent);

	//! The destructor
	~ComponentEditor();

	/*! Get the VLNV that can be used to identify the component.
	 *
	 *      @return The VLNV of the component.
	*/
	virtual VLNV getIdentifyingVLNV() const;

	/*! Get the vlnv of the current component.
	 *
	 *      @return VLNV of the component being edited.
	*/
	virtual VLNV getDocumentVLNV() const;

	/*! Check if the editor is editing a hardware implementation or not.
	 *
	 *      @return True if the component is a hardware component.
	*/
	virtual bool isHWImplementation() const;

	//! Refreshes the editor to display the changes made.
    virtual void refresh();

	/*!
	 *  Applies the current settings into use.
	 *
	 *      @param [in] settings   The settings file.
	 */
	void applySettings(QSettings& settings);

	/*!
	 *  Gets the names of all component editor items for HW component.
	 *
	 *      @return The names of the items.
	 */
	static QStringList getHwItemNames();

	/*!
	 *  Gets the names of all component editor items for SW component.
	 *
	 *      @return The names of the items.
	 */
	static QStringList getSwItemNames();

public slots:

    /*!
     *  Validates the document against the IP-XACT standard.
     *
     *      @param [in] errorList   Error message list for reporting standard violations.
     *
     *      @return True if the document is valid. False if there were any violations.
     */
    virtual bool validate(QVector<QString>& errorList);

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! Saves the document as new object and resets modifies state
	virtual bool saveAs();

	/*! Run vhdl generator for the component.
	 *
	 *      @return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onVhdlGenerate();

	/*! Set the protection state of the document.
	 *
	 *      @param [in] locked True for locked state, false for unlocked.
	 *
	*/
	virtual void setProtection(bool locked);

	/*!
	 *  Set the visibility of rows in tree view.
	 */
	void setRowVisibility(QSettings& settings);
    
    /*!
     *  Open the reference tree of the selected parameter.
     *
     *      @param [in] parameterId     The id of the parameter.
     */
    void openReferenceTree(QString const& parameterId);

private slots:

	/*! This slot is called when an item is selected in the navigation tree.
	 *
	 *      @param [in] index The index identifying the item.
	 *
	*/
	void onItemActivated(const QModelIndex& index);

	/*! This slot is called when navigation tree sends a selectItem signal.
	 * 
	 * This function tells navigation view to select the index in the tree and then
	 * calls ComponentEditor::onItemActivated() slot.
	 * 
	 *      @param [in] index Model index that identifies the item to select.
	 *
	*/
	void onNavigationTreeSelection(const QModelIndex& index);

	/*! This slot is called when navigation tree sends an expandItem signal.
	 * 
	 * This function tells navigation view to expand the index in the tree.
	 * 
	 *      @param [in] index Model index that identifies the item to expand.
	 *
	*/
    void onExpand(const QModelIndex& index);
    
	/*! \brief Display an error dialog to user with given text.
	 *
	 *      @param [in] message Contains the text to display in the error dialog.
	*/
	void onErrorDialog(const QString& message);

signals:
    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

private:

	//! No copying
	ComponentEditor(const ComponentEditor& other);

	//! No assignment
	ComponentEditor& operator=(const ComponentEditor& other);

    /*!
     *  Creates the root item for the navigation model for the given component.
     *
     *      @param [in] component   The component for which to create the root item.
     *
     *      @return The root item for the navigation model for the component.
     */
    QSharedPointer<ComponentEditorRootItem> createNavigationRootForComponent(QSharedPointer<Component> component);
    
    /*!
     *  Creates the navigation model root item and its children for a HW component.
     *
     *      @param [in] component   The component for which to create the root item.
     *
     *      @return The root item for the navigation model for the component.
     */
    QSharedPointer<ComponentEditorRootItem> createHWRootItem(QSharedPointer<Component> component);
        
    /*!
     *  Creates the navigation model root item and its children for a SW component.
     *
     *      @param [in] component   The component for which to create the root item.
     *
     *      @return The root item for the navigation model for the component.
     */
    QSharedPointer<ComponentEditorRootItem> createSWRootItem(QSharedPointer<Component> component);

    //! Setups the editor layout.
    void setupLayout();

    /*!
     *  Update component files of a target component.
     *
     *      @param [in] targetComponent     The component whose files are being updated.
     *      @param [in] otherComponent      The other component.
     *      @param [in] sourcePath          Source path.
     *      @param [in] targetPath          Target path.
     */
    void updateComponentFiles(QSharedPointer<Component> targetComponent, QSharedPointer<Component> otherComponent,
        QString const& sourcePath, QString const& targetPath);

    /*!
     *  Get a list of the file names of a component.
     *
     *      @param [in] component   The component whose file names are being looked.
     *
     *      @return A list of file names contained within a component.
     */
    QStringList getComponentFileNames(QSharedPointer<Component> component) const;

    /*!
     *  Change the name of a single file.
     *
     *      @param [in] from        The original file name.
     *      @param [in] to          The new file name.
     *      @param [in] component   The component containing the file.
     */
    void changeFileName(QString const& from, QString const& to, QSharedPointer<Component> component) const;

	//-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

    //! The instance managing all the plugins.
    PluginManager& pluginManager_;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! The splitter to contain the navigation tree.
	QSplitter navigationSplitter_;

	//! The splitter to display the editors and visualizers.
	QSplitter editorVisualizerSplitter_;

	//! The model to control the navigation view.
	ComponentEditorTreeModel navigationModel_;

	//! The tree view to navigate in the editor.
	ComponentTreeView navigationView_;

    //! The proxy model for sorting the tree items.
    ComponentEditorTreeProxyModel proxy_;

	//! The slot to display the editors in.
	ComponentEditorGroupSlot editorSlot_;

	//! The slot to display the visualizers in.
	ComponentEditorGroupSlot visualizerSlot_;

    //! Finds the specified parameter inside the component.
    QSharedPointer<ComponentParameterFinder> parameterFinder_;

    //! The counter that increases the amount of references to parameters.
    QSharedPointer<ReferenceCounter> referenceCounter_;

    //! The expression formatter used to change ids to names in expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used component validator.
    ComponentValidator validator_;
};

#endif // COMPONENTEDITOR_H
