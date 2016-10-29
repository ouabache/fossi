//-----------------------------------------------------------------------------
// File: SelectItemTypeDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 20.5.2011
//
// Description:
// Item type selection dialog.
//-----------------------------------------------------------------------------

#include "SelectItemTypeDialog.h"

//-----------------------------------------------------------------------------
// Function: SelectItemTypeDialog()
//-----------------------------------------------------------------------------
SelectItemTypeDialog::SelectItemTypeDialog(unsigned int allowedItems, QWidget* parent) : 
    QDialog(parent),
    allowedItems_(allowedItems),
    layout_(new QVBoxLayout(this)), 
    allowedItemsGroup_(new QGroupBox(tr("Item Type"), this)),    
    interfaceButton_(new QRadioButton(tr("Interface"), allowedItemsGroup_)),
    componentButton_(new QRadioButton(allowedItemsGroup_)),
    dialogButtons_(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this))
{
    Q_ASSERT(allowedItems != ColumnTypes::NONE);

    setWindowTitle(tr("Add Item"));

    setupLayout();

    setComponentButtonLabel();

    connect(dialogButtons_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(dialogButtons_, SIGNAL(rejected()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------
// Function: ~SelectItemTypeDialog()
//-----------------------------------------------------------------------------
SelectItemTypeDialog::~SelectItemTypeDialog()
{

}

//-----------------------------------------------------------------------------
// Function: getSelectedItemType()
//-----------------------------------------------------------------------------
ColumnTypes::ColumnItemType SelectItemTypeDialog::getSelectedItemType() const
{
    ColumnTypes::ColumnItemType type = ColumnTypes::NONE;

    if (interfaceButton_->isChecked())
    {
        type = ColumnTypes::INTERFACE;
    }
    else if (componentButton_->isChecked())
    {
        type = ColumnTypes::COMPONENT;
    }
    else
    {
        // We should not get here at all.
        Q_ASSERT(false);     
    }

    return type;
}

//-----------------------------------------------------------------------------
// Function: SelectItemTypeDialog::setupLayout()
//-----------------------------------------------------------------------------
void SelectItemTypeDialog::setupLayout()
{
    QVBoxLayout* itemLayout = new QVBoxLayout(allowedItemsGroup_);
    itemLayout->addWidget(interfaceButton_);
    itemLayout->addWidget(componentButton_);

    layout_->addWidget(allowedItemsGroup_);
    layout_->addWidget(dialogButtons_);
}

//-----------------------------------------------------------------------------
// Function: SelectItemTypeDialog::setComponentButtonLabel()
//-----------------------------------------------------------------------------
void SelectItemTypeDialog::setComponentButtonLabel()
{
    QString componentButtonLabel = "";

    if (allowedItems_ & ColumnTypes::COMPONENT)
    {
        componentButtonLabel += tr("Component/");
    }

    if (allowedItems_ & ColumnTypes::CHANNEL)
    {
        componentButtonLabel += tr("Channel/");
    }

    if (allowedItems_ & ColumnTypes::BRIDGE)
    {
        componentButtonLabel += tr("Bridge/");
    }

    Q_ASSERT(!componentButtonLabel.isEmpty());

    componentButtonLabel.chop(1);   //<! Remove last slash.
    componentButton_->setText(componentButtonLabel);
}
