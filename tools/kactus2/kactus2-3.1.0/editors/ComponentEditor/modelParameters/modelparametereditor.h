/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametereditor.h
 */

#ifndef MODELPARAMETEREDITOR_H
#define MODELPARAMETEREDITOR_H

#include "modelparametermodel.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/modelparameter.h>

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <common/views/EditableTableView/ColumnFreezableTable.h>

#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;

/*! \brief Widget to edit component's model parameters.
 *
 */
class ModelParameterEditor : public ItemEditor {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of this widget.
	 */
	ModelParameterEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter, 
        QWidget *parent = 0);

	//! \brief The destructor
	virtual ~ModelParameterEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief Enable/disable the import/export csv functionality for the view.
	 *
	 * Method: 		setAllowImportExport
	 * Full name:	ModelParameterEditor::setAllowImportExport
	 * Access:		public 
	 *
	 * \param allow If true then import/export is enabled.
	 *
	*/
	void setAllowImportExport(bool allow);

    /*!
     *  Sets the edited component.
     *
     *      @param [in] component   The component whose model parameters to edit.
     */
    void setComponent(QSharedPointer<Component> component);

signals:

    //! Emitted when a the value of a model parameter is modified.
    void parameterChanged(QSharedPointer<ModelParameter> changedParameter);

public slots:

    /*!
     *   Emits parameterChanged when detects changes in the model data.
     *
     *      @param [in] index The index of the changed data object.
     */
    void modelDataChanged(QModelIndex const& index);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	ModelParameterEditor(const ModelParameterEditor& other);

	//! No assignment
	ModelParameterEditor& operator=(const ModelParameterEditor& other);
    
	//! \brief The view that displays the parameters.
    ColumnFreezableTable* view_;

	//! \brief The model that holds the data to be displayed to the user
	ModelParameterModel* model_;
};

#endif // MODELPARAMETEREDITOR_H
