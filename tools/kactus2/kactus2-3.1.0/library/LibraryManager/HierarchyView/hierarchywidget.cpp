//-----------------------------------------------------------------------------
// File: hierarchywidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// The container for hierarchy view and hierarchy filter.
//-----------------------------------------------------------------------------

#include "hierarchywidget.h"
#include "hierarchymodel.h"
#include "hierarchyitem.h"

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: HierarchyWidget::HierarchyWidget()
//-----------------------------------------------------------------------------
HierarchyWidget::HierarchyWidget(LibraryInterface* handler, HierarchyModel* dataModel, QWidget *parent):
QWidget(parent), 
    filter_(new HierarchyFilter(this)),
    view_(this, handler, filter_),
    model_(dataModel)
{
	Q_ASSERT_X(handler, "HierarchyWidget constructor", "Null LibraryInterface pointer given as parameter");
	Q_ASSERT_X(dataModel, "HierarchyWidget constructor", "Null HierarchyModel pointer given as parameter");

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	filter_->setSourceModel(dataModel);
	view_.setModel(filter_);

	view_.sortByColumn(0, Qt::AscendingOrder);
	view_.setColumnWidth(0, 200);

	setupConnections(dataModel);
}

//-----------------------------------------------------------------------------
// Function: HierarchyWidget::~HierarchyWidget()
//-----------------------------------------------------------------------------
HierarchyWidget::~HierarchyWidget()
{
}

//-----------------------------------------------------------------------------
// Function: HierarchyWidget::setupConnections()
//-----------------------------------------------------------------------------
void HierarchyWidget::setupConnections(HierarchyModel* dataModel)
{
    connect(&view_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    // connect the view to the tree model
    connect(&view_, SIGNAL(openComponent(const QModelIndex&)),
        dataModel, SLOT(onOpenComponent(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openDesign(const QModelIndex&)), 
        dataModel, SLOT(onOpenDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openMemoryDesign(const QModelIndex&)), 
        dataModel, SLOT(onOpenMemoryDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openSWDesign(const QModelIndex&)), 
        dataModel, SLOT(onOpenSWDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openSystemDesign(const QModelIndex&)), 
        dataModel, SLOT(onOpenSystemDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewComponent(const QModelIndex&)),
        dataModel, SLOT(onCreateNewComponent(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewDesign(const QModelIndex&)),
        dataModel, SLOT(onCreateNewDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewSWDesign(const QModelIndex&)),
        dataModel, SLOT(onCreateNewSWDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewSystemDesign(const QModelIndex&)),
        dataModel, SLOT(onCreateNewSystemDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewBus(const QModelIndex&)),
        dataModel, SLOT(onCreateNewBus(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewAbsDef(const QModelIndex&)),
        dataModel, SLOT(onCreateNewAbsDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewComDef(const QModelIndex&)),
        dataModel, SLOT(onCreateNewComDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewComDef(const QModelIndex&)),
        dataModel, SLOT(onCreateNewComDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewApiDef(const QModelIndex&)),
        dataModel, SLOT(onCreateNewApiDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewApiDef(const QModelIndex&)),
        dataModel, SLOT(onCreateNewApiDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(exportItem(const QModelIndex&)),
        dataModel, SLOT(onExportItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(showErrors(const QModelIndex&)),
        dataModel, SLOT(onShowErrors(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(componentSelected(const VLNV&)),
        this, SIGNAL(componentSelected(const VLNV&)), Qt::UniqueConnection);

    connect(dataModel, SIGNAL(invalidateFilter()), filter_, SLOT(invalidate()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HierarchyWidget::selectItems()
//-----------------------------------------------------------------------------
void HierarchyWidget::selectItems(VLNV const& vlnv)
{
	// if vlnv is not valid
	if (!vlnv.isValid())
    {
		// do not select anything
		view_.clearSelection();
	}

	// convert the model indexes to view indexes
	QModelIndexList viewIndexes;
	foreach (QModelIndex const& index, model_->findIndexes(vlnv))
    {
		if (index.isValid())
        {
			viewIndexes.append(filter_->mapFromSource(index));
        }
	}

	view_.setSelectedIndexes(viewIndexes);
}

//-----------------------------------------------------------------------------
// Function: HierarchyWidget::getFilter()
//-----------------------------------------------------------------------------
LibraryFilter* HierarchyWidget::getFilter() const
{
    return filter_;
}