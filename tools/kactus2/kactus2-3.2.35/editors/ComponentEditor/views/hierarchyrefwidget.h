//-----------------------------------------------------------------------------
// File: hierarchyrefwidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2011
//
// Description:
// Editor to set the hierarchy reference for a hierarchical view.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYREFWIDGET_H
#define HIERARCHYREFWIDGET_H

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QWidget>
#include <QSharedPointer>
#include <QComboBox>
#include <QSharedPointer>

class ExpressionFormatter;
class LibraryInterface;
class ParameterFinder;

class Choice;
class View;
class DesignInstantiation;
class DesignConfigurationInstantiation;

//-----------------------------------------------------------------------------
//! Editor to set the hierarchy reference for a hierarchical view.
//-----------------------------------------------------------------------------
class HierarchyRefWidget : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *        @param [in] view                  The view being edited.
	 *        @param [in] libHandler            The library handler instance to use.
     *        @param [in] componentChoices      The choices available in the component.
     *        @param [in] parameterFinder       Finder for component parameters.
     *        @param [in] expressionFormatter   The formatter for module parameter expressions.
	 *        @param [in] parent                The owner of this editor.
	 */
	HierarchyRefWidget(QSharedPointer<View> view,
        QSharedPointer<DesignInstantiation> designInstantiation,
        QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation,
        LibraryInterface* libHandler,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent);
	
	//! The destructor.
	virtual ~HierarchyRefWidget();

	/*!
     *  Check for the validity of the edited view.
	 *
	 *      @return True, if the hierarchical reference is valid, otherwise false.
	 */
	bool isValid() const;

    QSharedPointer<DesignInstantiation> getDesignInstantiation();

    QSharedPointer<DesignConfigurationInstantiation> getDesignConfigurationInstantiation();

public slots:

    /*!
     *  Refresh the list of available views.
     *
     *      @param [in] availableViews   The available view names.
     */
    void refresh();

	//! Clear the hierarchy reference and top level view reference.
	void clear();

signals:

	//! Emitted when contents of the editor change.
	void contentChanged();

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(QString const& url);
        
    /*!
     *  Increase the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the selected parameter.
     *
     *      @param [in] id      Id of the selected parameter.
     */
    void openReferenceTree(QString const& id) const;

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in hierarchy reference.
	void onVLNVChanged();


private:

	//! No copying
	HierarchyRefWidget(const HierarchyRefWidget& other);

	//! No assignment
	HierarchyRefWidget& operator=(const HierarchyRefWidget& other);
	
    //! Updates the referenced design according to the view hierarchy reference.
    void updateDesignReference();

    /*!
     *  Updates the error displaying for hierarchy reference.
     */
    void updateErrorSignAndTooltip();

    /*!
     *  Shows the error sign and a tool tip for the cause of an error in hierarchy reference.
     */
    void showErrorSignAndTooltip();
    
    /*!
     *  Hides the error sign and a tool tip for the cause of an error in hierarchy reference.
     */
    void hideErrorSignAndTooltip();

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library in use.
    LibraryInterface* library_;

	//! Pointer to the view being edited.
	QSharedPointer<View> view_;

    QSharedPointer<DesignInstantiation> designInstantiation_;

    QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation_;

	//! The editor to set the design configuration as the hierarchical reference.
	VLNVEditor* designConfigurationEditor_;

    //! The display for referenced design.
    VLNVEditor* designEditor_;
};

#endif // HIERARCHYREFWIDGET_H
