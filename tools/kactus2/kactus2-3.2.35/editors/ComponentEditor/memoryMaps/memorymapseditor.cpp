//-----------------------------------------------------------------------------
// File: memorymapseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.04.2012
//
// Description:
// Editor for editing the memory maps of a component.
//-----------------------------------------------------------------------------

#include "memorymapseditor.h"
#include "memorymapsmodel.h"
#include "MemoryMapsColumns.h"
#include "MemoryMapsView.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/RemapState.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: memorymapseditor::MemoryMapsEditor()
//-----------------------------------------------------------------------------
MemoryMapsEditor::MemoryMapsEditor(QSharedPointer<Component> component,
                                   QSharedPointer<ParameterFinder> parameterFinder, LibraryInterface* handler,
                                   QSharedPointer<MemoryMapValidator> memoryMapValidator,
                                   QWidget *parent /* = 0 */):
ItemEditor(component, handler, parent),
view_(new MemoryMapsView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(new MemoryMapsModel(component, parameterFinder, memoryMapValidator, this)),
delegate_()
{
	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Memory maps summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

    // Set case-insensitive sorting.
    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/memMapsList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);
	view_->setSortingEnabled(true);

    delegate_ = new MemoryMapsDelegate(getRemapStateNames(), this);
    view_->setItemDelegate(delegate_);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(memoryMapAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(memoryMapRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(model_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(memoryRemapAdded(int, QSharedPointer<MemoryMap>)),
        this, SIGNAL(memoryRemapAdded(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);
    connect(model_, SIGNAL(memoryRemapRemoved(int ,QSharedPointer<MemoryMap>)),
        this, SIGNAL(memoryRemapRemoved(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);

    connect(model_, SIGNAL(aubChangedOnRow(int)), 
        this, SIGNAL(changeInAddressUnitBitsOnRow(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
	
    connect(view_, SIGNAL(addMemoryRemapItem(const QModelIndex&)), 
        model_, SLOT(onAddMemoryRemap(const QModelIndex&)), Qt::UniqueConnection);
        
	connect(view_, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(onDoubleClick(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(copyRows(QModelIndexList)),
        model_, SLOT(onCopyRows(QModelIndexList)), Qt::UniqueConnection);
    connect(view_, SIGNAL(pasteRows(QModelIndex)), model_, SLOT(onPasteRows(QModelIndex)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::~MemoryMapsEditor()
//-----------------------------------------------------------------------------
MemoryMapsEditor::~MemoryMapsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::refresh()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::refresh()
{
    delegate_->updateRemapStateNames(getRemapStateNames());
	view_->update();
    view_->expandAll();
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::getRemapStateNames()
//-----------------------------------------------------------------------------
QStringList MemoryMapsEditor::getRemapStateNames() const
{
    QStringList remapStateNames;
    foreach (QSharedPointer<RemapState> remapState, *component()->getRemapStates())
    {
        remapStateNames.append(remapState->name());
    }

    return remapStateNames;
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::showEvent()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/memorymaps.html");
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::onDoubleClick()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::onDoubleClick( const QModelIndex& index )
{
	QModelIndex origIndex = proxy_->mapToSource(index);
	
	// index must be valid
	if (!origIndex.isValid())
    {
		return;
	}

	// if the column is for interface references
    if (origIndex.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
		// get the names of the interface that refer to selected memory map
		QStringList busIfNames = model_->data(origIndex, Qt::UserRole).toStringList();

		// if there are no bus interfaces or there are many
		if (busIfNames.size() != 1)
        {
			return;
		}
		
		// inform component editor that bus interface editor should be selected
		emit selectBusInterface(busIfNames.first());
	}
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::sizeHint()
//-----------------------------------------------------------------------------
QSize MemoryMapsEditor::sizeHint() const
{
	return QSize(MemoryMapsEditor::WIDTH, MemoryMapsEditor::HEIGHT);
}
