//-----------------------------------------------------------------------------
// File: SWPropertiesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 21.5.2012
//
// Description:
// Editor for SW properties.
//-----------------------------------------------------------------------------

#include "SWPropertiesEditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComDefinitionEditor/ComPropertyDelegate.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::SWPropertiesEditor()
//-----------------------------------------------------------------------------
SWPropertiesEditor::SWPropertiesEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QWidget *parent): 
ItemEditor(component, handler, parent),
    view_(this),
    filter_(this),
    model_(this)
{
    // Set the model source and other options for the view.
    filter_.setSourceModel(&model_);

    view_.setModel(&filter_);

    // Set case-insensitive sorting.
    filter_.setSortCaseSensitivity(Qt::CaseInsensitive);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/swPropertiesListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);
    view_.setSortingEnabled(true);
    view_.setItemsDraggable(false);
    view_.setItemDelegate(new ComPropertyDelegate(this));

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Software properties"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

    refresh();

    connect(&model_, SIGNAL(contentChanged()), this, SLOT(onPropertyChange()), Qt::UniqueConnection);
    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
            &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::~SWPropertiesEditor()
//-----------------------------------------------------------------------------
SWPropertiesEditor::~SWPropertiesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::isValid()
//-----------------------------------------------------------------------------
bool SWPropertiesEditor::isValid() const
{
    return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::refresh()
//-----------------------------------------------------------------------------
void SWPropertiesEditor::refresh()
{
	 model_.setProperties(component()->getSWProperties());
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::onPropertyChange()
//-----------------------------------------------------------------------------
void SWPropertiesEditor::onPropertyChange()
{
	QList< QSharedPointer<ComProperty> > properties;

	foreach (QSharedPointer<ComProperty> prop, model_.getProperties())
	{
		properties.append(QSharedPointer<ComProperty>(new ComProperty(*prop.data())));
	}

	component()->setSWProperties(properties);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::showEvent()
//-----------------------------------------------------------------------------
void SWPropertiesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/swproperties.html");
}
