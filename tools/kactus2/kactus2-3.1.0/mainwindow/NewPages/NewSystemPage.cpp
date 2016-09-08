//-----------------------------------------------------------------------------
// File: NewSystemPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 9.6.2011
//
// Description:
// New system property page.
//-----------------------------------------------------------------------------

#include "NewSystemPage.h"

#include <library/LibraryManager/libraryitem.h>
#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::NewSystemPage(LibraryInterface* libInterface, QWidget* parentDlg)
: NewPage(libInterface, VLNV::COMPONENT, tr("New System"), tr("Creates SW architecture that can be mapped to HW"), parentDlg),
actionGroupBox_(new QGroupBox(tr("Mapping Options"), this)),
actionGroup_(new QButtonGroup(this)),
emptyRadioButton_(new QRadioButton(tr("Leave initially unmapped"), this)),
emptyDescLabel_(new QLabel(tr("Creates an unmapped SW architecture. A new system component will be created."), this)),
mapRadioButton_(new QRadioButton(tr("Map to specific HW now"), this)),
mapDescLabel_(new QLabel(tr("Creates a SW architecture and maps it to selected top-level HW. A system view will "
              "be created for the HW component to reference the new system design."), this)),
              treeLabel_(new QLabel(tr("Select top-level HW component:"), this)),
              compTreeWidget_(new QTreeWidget(this)),             
              viewLabel_(new QLabel(tr("Select view of top-level HW component:"), this)),
              viewComboBox_(new QComboBox(this))
{
    emptyDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    emptyDescLabel_->setWordWrap(true);
    mapDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    mapDescLabel_->setWordWrap(true);

    QFont font = emptyRadioButton_->font();
    font.setBold(true);
    emptyRadioButton_->setFont(font);
    emptyRadioButton_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");
    mapRadioButton_->setFont(font);
    mapRadioButton_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");

    actionGroup_->addButton(emptyRadioButton_);
    actionGroup_->addButton(mapRadioButton_);

    // Create the tree widget and fill it with VLNV data.
    compTreeWidget_->setColumnCount(1);
    compTreeWidget_->setSelectionBehavior(QAbstractItemView::SelectItems);
    compTreeWidget_->header()->close();
    compTreeWidget_->setIconSize(QSize(20, 20));

    for (int i = 0; i < libInterface->getTreeRoot()->getNumberOfChildren(); ++i)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, libInterface->getTreeRoot()->child(i)->name());

        // Add child items.
        addChildItems(libInterface->getTreeRoot()->child(i), item);

        // Add only items that have children.
        if (item->childCount() == 0)
        {
            delete item;
            continue;
        }

        // Add the item to the tree.
        compTreeWidget_->addTopLevelItem(item);
    }

    compTreeWidget_->sortItems(0, Qt::AscendingOrder);
    compTreeWidget_->setFixedHeight(160);

    connect(compTreeWidget_, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(onTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::SYSTEM);
    vlnvEditor_->addNameExtension(".sysdesign");
    vlnvEditor_->addNameExtension(".sysdesigncfg");
    //vlnvEditor_->setTitle(tr("VLNV of the new system design"));

    QVBoxLayout* groupLayout = new QVBoxLayout(actionGroupBox_);
    groupLayout->addWidget(emptyRadioButton_);
    groupLayout->addWidget(emptyDescLabel_);
    groupLayout->addWidget(mapRadioButton_);
    groupLayout->addWidget(mapDescLabel_);

    // Setup the layout.
    QVBoxLayout* topLayout = dynamic_cast<QVBoxLayout*>(layout());
    Q_ASSERT(topLayout);
    topLayout->insertWidget(3, actionGroupBox_);
    topLayout->insertWidget(4, treeLabel_);
    topLayout->insertWidget(5, compTreeWidget_, 1);
    topLayout->insertWidget(6, viewLabel_);
    topLayout->insertWidget(7, viewComboBox_);

    connect(actionGroup_, SIGNAL(buttonClicked(QAbstractButton*)),
        this, SLOT(actionChanged(QAbstractButton*)), Qt::UniqueConnection);

    emptyRadioButton_->setChecked(true);
    actionChanged(emptyRadioButton_);
}

//-----------------------------------------------------------------------------
// Function: ~NewSystemPage()
//-----------------------------------------------------------------------------
NewSystemPage::~NewSystemPage()
{

}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool NewSystemPage::prevalidate() const
{
    if (mapRadioButton_->isChecked())
    {
        if (compTreeWidget_->currentItem() == 0)
        {
            return false;
        }

        QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

        return (!data.isNull() && data.value<VLNV>().isValid() &&
            NewPage::prevalidate());
    }
    else
    {
        return NewPage::prevalidate();
    }
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewSystemPage::validate()
{
    Q_ASSERT(prevalidate());

    VLNV vlnv = vlnvEditor_->getVLNV();
    bool validVLNV = NewPage::validate();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".sysdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".sysdesigncfg", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (emptyRadioButton_->isChecked() && !validVLNV)
    {
        showErrorForReservedVLVN(vlnv);
        return false;
    }

    if (!isUnusedVLNV(designVLNV))
    {
        showErrorForReservedVLVN(designVLNV);
        return false;
    }

    if (!isUnusedVLNV(desConfVLNV))
    {
        showErrorForReservedVLVN(desConfVLNV);        
        return false;
    }    

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewSystemPage::apply()
{
    // Create the system in a different way based on the mapping option selection.
    if (mapRadioButton_->isChecked())
    {
        Q_ASSERT(compTreeWidget_->currentItem() != 0);
        QVariant data = compTreeWidget_->currentItem()->data(0, Qt::UserRole);

        emit createSystem(data.value<VLNV>(), viewComboBox_->currentText(),
            vlnvEditor_->getVLNV(), selectedPath());
    }
    else
    {
        emit createSystem(VLNV(), "", vlnvEditor_->getVLNV(),selectedPath());
    }
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewSystemPage::onPageChange()
{
    // Discard the VLNV and collapse the tree widget.
    compTreeWidget_->collapseAll();
    compTreeWidget_->setCurrentItem(0);
    return NewPage::onPageChange();
}

//-----------------------------------------------------------------------------
// Function: onTreeItemChanged()
//-----------------------------------------------------------------------------
void NewSystemPage::onTreeItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem*)
{
    if (cur != 0)
    {
        // Check if the item has a VLNV (i.e. is a leaf item).
        QVariant data = cur->data(0, Qt::UserRole);

        if (!data.isNull())
        {
            VLNV compVLNV = data.value<VLNV>();
            VLNV sysVLNV = vlnvEditor_->getVLNV();

            // Auto-fill the VLNV editor information (vendor and library fields).
            vlnvEditor_->setVLNV(VLNV(VLNV::DESIGN, compVLNV.getVendor(), compVLNV.getLibrary(),
                sysVLNV.getName(), sysVLNV.getVersion()));

            // Add all available hierarchical views to the view combo box.
            QSharedPointer<Document> libComp = libInterface_->getModel(compVLNV);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            viewComboBox_->clear();

            foreach (QString const& viewName, comp->getHierViews())
            {
                viewComboBox_->addItem(viewName);
            }
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: addChildItems()
//-----------------------------------------------------------------------------
void NewSystemPage::addChildItems(LibraryItem const* libItem, QTreeWidgetItem* treeItem)
{
    for (int i = 0; i < libItem->getNumberOfChildren(); ++i)
    {
        VLNV vlnv = libItem->child(i)->getVLNV();

        if (vlnv.isValid() && vlnv.getType() != VLNV::COMPONENT)
        {
            continue;
        }

        // Create a child tree widget item.
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, libItem->child(i)->name());

        // Recursively add its children if this is not the leaf level.
        if (!vlnv.isValid())
        {
            addChildItems(libItem->child(i), item);

            // Add the item as a child of the given tree item if it has children.
            if (item->childCount() > 0)
            {
                treeItem->addChild(item);
            }
            else
            {
                delete item;
            }
        }
        else
        {
            // Only hierarchical HW components are added.
            QSharedPointer<Document> libComp = libInterface_->getModel(vlnv);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            QStringList views = comp->getHierViews();
            views.removeAll("kts_sw_ref");
            views.removeAll("kts_sys_ref");

            if (comp->getImplementation() != KactusAttribute::HW || views.isEmpty())
            {
                delete item;
                continue;
            }

            // Add the VLNV to the item's data.
            item->setData(0, Qt::UserRole, QVariant::fromValue((vlnv)));
            item->setIcon(0, QIcon(":icons/common/graphics/hier-hw-component.png"));
            treeItem->addChild(item);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: NewSystemPage::actionChanged()
//-----------------------------------------------------------------------------
void NewSystemPage::actionChanged(QAbstractButton* button)
{
    if (button == emptyRadioButton_)
    {
        compTreeWidget_->collapseAll();
        compTreeWidget_->setCurrentItem(0);
        viewComboBox_->clear();
    }

    treeLabel_->setEnabled(button == mapRadioButton_);
    compTreeWidget_->setEnabled(button == mapRadioButton_);

    viewLabel_->setEnabled(button == mapRadioButton_);
    viewComboBox_->setEnabled(button == mapRadioButton_);

    emit contentChanged();
}
