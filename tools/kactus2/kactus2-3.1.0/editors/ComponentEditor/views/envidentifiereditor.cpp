//-----------------------------------------------------------------------------
// File: envidentifiereditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.04.2011
//
// Description:
// Widget to edit the ipxact:envIdentifiers element within view.
//-----------------------------------------------------------------------------

#include "envidentifiereditor.h"

#include <IPXACTmodels/Component/View.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: EnvIdentifierEditor::EnvIdentifierEditor()
//-----------------------------------------------------------------------------
EnvIdentifierEditor::EnvIdentifierEditor(QSharedPointer<View> view, QWidget *parent): 
QGroupBox(tr("Environment identifiers"), parent),
view_(this),
model_(view, this), 
proxy_(this) 
{
	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EnvIdentifierEditor::~EnvIdentifierEditor()
//-----------------------------------------------------------------------------
EnvIdentifierEditor::~EnvIdentifierEditor()
{

}

//-----------------------------------------------------------------------------
// Function: EnvIdentifierEditor::refresh()
//-----------------------------------------------------------------------------
void EnvIdentifierEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: EnvIdentifierEditor::isValid()
//-----------------------------------------------------------------------------
bool EnvIdentifierEditor::isValid() const
{
	return model_.isValid();
}
