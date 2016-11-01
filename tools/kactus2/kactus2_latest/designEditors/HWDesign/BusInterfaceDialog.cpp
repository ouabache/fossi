//-----------------------------------------------------------------------------
// File: BusInterfaceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 30.5.2011
//
// Description:
// Interface mode selection dialog.
//-----------------------------------------------------------------------------

#include "BusInterfaceDialog.h"

#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QTableView>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>

#include <designEditors/HWDesign/views/CellEditTableView.h>
#include <designEditors/HWDesign/models/PortGenerationTableModel.h>

#include <library/LibraryManager/libraryinterface.h>

namespace
{
    QString const MODE_NAMES[] =
    {
        "Master",
        "Slave",
        "System",
        "Mirrored Master",
        "Mirrored Slave",
        "Mirrored System",
        "Monitor"
    }; 

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::BusInterfaceDialog(bool enableNameEdit, QWidget* parent) 
    : QDialog(parent),
      layout_(0),
      nameLabel_(0),
      nameEdit_(0),
      modeGroup_(new QGroupBox(tr("Select Interface Mode"), this)),
      btnOK_(new QPushButton(tr("&OK"),this)),
      btnCancel_(new QPushButton(tr("&Cancel"),this)),
      tableGroup_(new QGroupBox(tr("Define Port Names and Descriptions in Target Component"), this)),
      tableEnable_(false),
      lh_(0),
      sourceComp_(),
      destComp_(),
      ports_(),
      portMaps_(),
      busIf_(),
      opposingEnd_(0),
      draftEnd_(0),
      portsModel_(0),
      portsView_(0),
      proxy_(0),
      modes_(0)
{
    setWindowTitle(tr("Define Bus Interface"));

    // Check if name editing is requested.
    if (enableNameEdit)
    {
        // Create the name edit and its label.
        nameLabel_ = new QLabel(tr("Name:"), this);
        nameEdit_ = new QLineEdit(this);
    }

    // Create the radio button group.
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        modeRadioButtons_[i] = new QRadioButton(MODE_NAMES[i], modeGroup_);
        modeRadioButtons_[i]->setVisible(false);       
    }

    // Connect the button signals to accept() and reject().
    connect(btnOK_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel_, SIGNAL(clicked()), this, SLOT(reject()));

    if (nameEdit_ != 0)
    {
        connect(nameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onNameChanged()));
        btnOK_->setEnabled(!nameEdit_->text().isEmpty());
    }

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::~BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::~BusInterfaceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::addMode()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::addMode(General::InterfaceMode mode)
{
    modeRadioButtons_[mode]->setVisible(true);
    modeRadioButtons_[mode]->setChecked(true);
    connect(modeRadioButtons_[mode], SIGNAL(clicked(bool)), this, SLOT(onModeSelected(bool)));
    modes_++;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::getSelectedMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceDialog::getSelectedMode() const
{
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        if (modeRadioButtons_[i]->isChecked())
        {
            return static_cast<General::InterfaceMode>(i);
        }
    }

    // Should not be possible to get here.
    Q_ASSERT(false);
    return General::MASTER;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::setBusInterfaces()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setBusInterfaces(QSharedPointer<Component> srcComponent, 
    QSharedPointer<BusInterface> busInterface, 
    QSharedPointer<Component> targetComponent, 
    LibraryInterface* handler)
{
    sourceComp_ = srcComponent;
    destComp_ = targetComponent;
    busIf_ = busInterface;
    lh_ = handler;

    if (!tableEnable_)
    {
        setupPortTable();      

        if (modes_ == 1)
        {
            btnOK_->setEnabled(portsModel_->isValid());
            portsView_->setEnabled(true);
            modeGroup_->setEnabled(false);
            updatePortsView();
        }
        else
        {
            // Clear initial mode selection.
            for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
            {
                modeRadioButtons_[i]->setAutoExclusive(false);  
                modeRadioButtons_[i]->setAutoExclusive(true);
            }
			updatePortsView();

			btnOK_->setEnabled( portsModel_->isValid() );
			portsView_->setEnabled( true );
        }
    }
   
    QString sourceName = srcComponent->getVlnv().getName();
    if (sourceName.isEmpty())
    {
        sourceName = "Draft Component";
    }
    QString destName = targetComponent->getVlnv().getName();
    if (destName.isEmpty())
    {
        destName = "Draft Component";
    }

    QString sourceHeader = "Name in " + sourceName;
    portsModel_->setHeaderData(PortGenerationTableModel::SRC_NAME, Qt::Horizontal, sourceHeader);

    QString draftHeader = "Name in " + destName;
    portsModel_->setHeaderData(PortGenerationTableModel::TARGET_NAME, Qt::Horizontal, draftHeader);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::getPortMaps()
//-----------------------------------------------------------------------------
QList< QSharedPointer<PortMap> > BusInterfaceDialog::getPortMaps() 
{   
    // Create port maps
    if (tableEnable_ && portMaps_.empty())
    {
        foreach (QSharedPointer<PortMap> portMap, *busIf_->getPortMaps())
        {  
            QString physicalName = "";

            int rowCount = portsModel_->rowCount();
            for(int row = 0; row <rowCount; row++)
            {          
                QModelIndex targetIndex = portsModel_->index(row, PortGenerationTableModel::TARGET_NAME);
                QModelIndex sourceIndex = portsModel_->index(row, PortGenerationTableModel::SRC_NAME);

                QString sourceName = sourceIndex.data(Qt::DisplayRole).toString();
                if (sourceName == portMap->getPhysicalPort()->name_)
                {
                     physicalName = targetIndex.data(Qt::DisplayRole).toString();
                     break;
                }
            }
       
            QSharedPointer<PortMap::PhysicalPort> originalPort = portMap->getPhysicalPort();

            QSharedPointer<PortMap> generated(new PortMap(*portMap));
            QSharedPointer<PortMap::PhysicalPort> physicalPort = generated->getPhysicalPort();
            if (!physicalPort)
            {
                physicalPort = QSharedPointer<PortMap::PhysicalPort>(new PortMap::PhysicalPort());
                generated->setPhysicalPort(physicalPort);
            }

            physicalPort->name_ = physicalName;

            portMaps_.append(generated);
        }
    }

    return portMaps_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::getPorts()
//-----------------------------------------------------------------------------
QList< QSharedPointer<Port> > BusInterfaceDialog::getPorts() 
{
    // Create ports
    if (tableEnable_ && ports_.empty())
    {
        int rowCount = portsModel_->rowCount();
        for (int row = 0; row < rowCount; row++)
        {
            QModelIndex sourceIndex = portsModel_->index(row, PortGenerationTableModel::SRC_NAME);
            QString name = sourceIndex.data(Qt::DisplayRole).toString();

            QModelIndex targetIndex = portsModel_->index(row, PortGenerationTableModel::TARGET_NAME);
            QString generatedName = targetIndex.data(Qt::DisplayRole).toString();
            
            QModelIndex directionIndex = portsModel_->index(row, PortGenerationTableModel::TARGET_DIRECTION);
            QString draftDir = directionIndex.data(Qt::DisplayRole).toString();
            
            QSharedPointer<Port> draftPort(new Port(*sourceComp_->getPort(name)));
            draftPort->setName(generatedName);
            draftPort->setDirection(DirectionTypes::str2Direction(draftDir, DirectionTypes::DIRECTION_INVALID));

            if (draftPort->getDirection() == DirectionTypes::OUT)
            {
                draftPort->setDefaultValue("");
            }

            ports_.append(draftPort);
        }
    }

    return ports_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::setName()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setName(QString const& name)
{
    Q_ASSERT(nameEdit_ != 0);
    nameEdit_->setText(name);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::name()
//-----------------------------------------------------------------------------
QString BusInterfaceDialog::name() const
{
    Q_ASSERT(nameEdit_ != 0);
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::onNameChanged()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onNameChanged()
{
    btnOK_->setEnabled(!nameEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::onTableDataChanged()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onTableDataChanged()
{
    btnOK_->setEnabled(portsModel_->isValid());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::onModeSelected()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onModeSelected(bool const radioButtonChecked)
{
    if (radioButtonChecked && tableEnable_)
    {
        btnOK_->setDefault(false);
        portsView_->setEnabled(true);
        updatePortsView();        
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setupLayout()
{
    layout_ = new QVBoxLayout(this);

    // Create layout for name editing.
    if ( nameEdit_ != 0 )
    {
        QHBoxLayout* nameEditLayout = new QHBoxLayout();
        nameEditLayout->addWidget(nameLabel_);
        nameEditLayout->addWidget(nameEdit_);
        layout_->addLayout(nameEditLayout);
    }

    // Create vertical layout for radio buttons.
    QGridLayout* itemLayout = new QGridLayout(modeGroup_);
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        itemLayout->addWidget(modeRadioButtons_[i], i, 0, 1, 1);
    }
    layout_->addWidget(modeGroup_);
  
    layout_->addWidget(tableGroup_);  
    tableGroup_->setVisible(false);

    // Create layout for OK and Cancel buttons.
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnOK_, 0, Qt::AlignRight);
    buttonLayout->addWidget(btnCancel_, 0, Qt::AlignRight);

    layout_->addLayout(buttonLayout);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::setupPortTable()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setupPortTable()
{
    portsView_ = new CellEditTableView(this);
    portsView_->setSortingEnabled(true);
    portsView_->setItemDelegate(new LineEditDelegate(this));

    portsModel_ = new PortGenerationTableModel(this);   

    proxy_ = new QSortFilterProxyModel(this);
    proxy_->setSourceModel(portsModel_);
    portsView_->setModel(proxy_);  
    portsView_->sortByColumn(1, Qt::AscendingOrder);

    // Create new, horizontal, layout for radio buttons.
    delete modeGroup_->layout();
    QGridLayout* itemLayout = new QGridLayout(modeGroup_);
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        itemLayout->addWidget(modeRadioButtons_[i], 0, i, 1, 1);       
    }
    modeGroup_->setLayout(itemLayout);

    // Create layout for port generation table.
    QGridLayout* tableLayout = new QGridLayout(tableGroup_);          
    tableLayout->addWidget(portsView_,1,0,1,1);   
    tableGroup_->setVisible(true);

    connect(portsModel_,SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)),
        this, SLOT(onTableDataChanged()));

    setMinimumSize(QSize(800, 600));
    tableEnable_ = true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::updatePortsView()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::updatePortsView()
{
    if (tableEnable_ && modes_ != 0)
    {
        portsModel_->initialize(sourceComp_, busIf_, destComp_, lh_, getSelectedMode());        
        onTableDataChanged();
    }
}
