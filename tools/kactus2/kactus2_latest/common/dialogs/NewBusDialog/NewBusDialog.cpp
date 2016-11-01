//-----------------------------------------------------------------------------
// File: NewBusDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Dialog for new bus VLNV selection and bus abstraction generation settings.
//-----------------------------------------------------------------------------

#include "NewBusDialog.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>

#include <library/LibraryManager/libraryinterface.h>

#include <editors/BusDefinitionEditor/absdefgroup.h>
#include <editors/BusDefinitionEditor/busdefgroup.h>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: NewBusDialog::NewBusDialog()
//-----------------------------------------------------------------------------
NewBusDialog::NewBusDialog(LibraryInterface* handler, QWidget* parent) :
NewObjectDialog(handler, VLNV::BUSDEFINITION , false, parent),
nameSelection_(tr("Name"), this),
descriptionSelection_(tr("Description"), this)
{         
    setWindowTitle(tr("New bus interface"));  

    setupLayout();
    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: NewBusDialog::~NewBusDialog()
//-----------------------------------------------------------------------------
NewBusDialog::~NewBusDialog()
{
}

//-----------------------------------------------------------------------------
// Function: NewBusDialog::getSignalSelection()
//-----------------------------------------------------------------------------
NewBusDialog::Signal_selection NewBusDialog::getSignalSelection() const
{
    if (nameSelection_.isChecked())
    {
        return USE_NAME;
    }
    else
    {
        return USE_DESCRIPTION;
    }
}

//-----------------------------------------------------------------------------
// Function: NewBusDialog::~NewBusDialog()
//-----------------------------------------------------------------------------
void NewBusDialog::setupLayout()
{
    QGroupBox* signalButtonGroup = new QGroupBox(tr("Select source for logical signal generation"), this);
    QVBoxLayout* buttonLayout = new QVBoxLayout(signalButtonGroup);
    buttonLayout->addWidget(&nameSelection_);
    buttonLayout->addWidget(&descriptionSelection_);
    nameSelection_.setChecked(true);

    QVBoxLayout* topLayout = dynamic_cast<QVBoxLayout*>(layout());
    if (topLayout)
    {
        topLayout->insertWidget(0, signalButtonGroup);    
    }
    else
    {
        layout()->addWidget(signalButtonGroup);
    }
}
