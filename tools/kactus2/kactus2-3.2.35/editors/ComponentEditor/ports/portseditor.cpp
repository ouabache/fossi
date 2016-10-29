//-----------------------------------------------------------------------------
// File: portseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.03.2011
//
// Description:
// Editor to edit the ports of a component.
//-----------------------------------------------------------------------------

#include "portseditor.h"

#include "portsdelegate.h"
#include "portsmodel.h"
#include "PortsView.h"

#include <common/dialogs/NewBusDialog/NewBusDialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <wizards/BusInterfaceWizard/BusInterfaceWizard.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: PortsEditor::PortsEditor()
//-----------------------------------------------------------------------------
PortsEditor::PortsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
                         QSharedPointer<ParameterFinder> parameterFinder,
                         QSharedPointer<ExpressionFormatter> expressionFormatter,
                         QSharedPointer<PortValidator> portValidator, QWidget *parent /* = 0 */):
ItemEditor(component, handler, parent),
view_(new PortsView(this)), 
model_(0),
proxy_(this),
component_(component),
handler_(handler)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new PortsModel(component->getModel(), expressionParser, parameterFinder, expressionFormatter,
        portValidator, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/portListing.csv").arg(componentPath);
	view_->setDefaultImportExportPath(defaultPath);
	view_->setAllowImportExport(true);
    view_->setAlternatingRowColors(false);
    view_->setSortingEnabled(true);
    view_->setItemsDraggable(false);
    view_->setDelegate(new PortsDelegate(parameterCompleter, parameterFinder, this));

    proxy_.setSourceModel(model_);
    view_->setModel(&proxy_);

    view_->resizeColumnsToContents();

    proxy_.setSortCaseSensitivity(Qt::CaseInsensitive);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
  
    connect(view_, SIGNAL(createBus(QStringList const& )), this, SLOT(onCreateNewInteface(QStringList const& )));
    connect(view_, SIGNAL(createExistingBus(QStringList const& )), 
        this, SLOT(onCreateInterface(QStringList const& )));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)), 
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Ports"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_, 1);
	layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::~PortsEditor()
//-----------------------------------------------------------------------------
PortsEditor::~PortsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: PortsEditor::isValid()
//-----------------------------------------------------------------------------
bool PortsEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: portseditor::refresh()
//-----------------------------------------------------------------------------
void PortsEditor::refresh()
{
	view_->update();
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::showEvent()
//-----------------------------------------------------------------------------
void PortsEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);
    emit helpUrlRequested("componenteditor/ports.html");
}

//-----------------------------------------------------------------------------
// Function: portseditor::setAllowImportExport()
//-----------------------------------------------------------------------------
void PortsEditor::setAllowImportExport(bool allow)
{
	view_->setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::setComponent()
//-----------------------------------------------------------------------------
void PortsEditor::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
    model_->setModelAndLockCurrentPorts(component_->getModel());
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::onCreateNewInteface()
//-----------------------------------------------------------------------------
void PortsEditor::onCreateNewInteface(QStringList const& selectedPorts)
{
    // Ask user for new bus definition VLNV.
    NewBusDialog dialog(handler_, this);
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // create the vlnvs that identify the bus definition and abstraction definition
    VLNV busVLNV = dialog.getVLNV();
    busVLNV.setType(VLNV::BUSDEFINITION);
    
    // remove the possible .busDef from the end of the name field
    VLNV absVLNV = busVLNV;
    QString absDefName = busVLNV.getName();
    absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);
    absVLNV.setName(absDefName + ".absDef");
    absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

    // by default the abs def and bus def are saved to same directory
    QString absDirectory = dialog.getPath();

    if (handler_->contains(absVLNV))
    {
        VLNV newAbsDefVLNV;

        if (!NewObjectDialog::saveAsDialog(this, handler_, absVLNV, newAbsDefVLNV, absDirectory,
            QStringLiteral("Set VLNV for abstraction definition")))
        {
                return; // if user canceled
        }
        // save the created abstraction definition vlnv
        absVLNV = newAbsDefVLNV;
    }

    // Create a bus definition.
    QSharedPointer<BusDefinition> busDef(new BusDefinition());
    busDef->setVlnv(busVLNV);

    // Create the file for the bus definition.
    if (!handler_->writeModelToFile(absDirectory, busDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // create an abstraction definition
    QSharedPointer<AbstractionDefinition> absDef(new AbstractionDefinition());
    absDef->setVlnv(absVLNV);

    // set reference from abstraction definition to bus definition
    absDef->setBusType(busVLNV);

    // create the file for the abstraction definition
    if (!handler_->writeModelToFile(absDirectory, absDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // Create interface with unique name within the component.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    QString ifName = busVLNV.getName();
    int id = 1;
    while(component_->hasInterface(ifName))
    {
        ifName = busVLNV.getName() + "_" + QString::number(id);
        id++;
    }

    busIf->setName(ifName);
    QSharedPointer<AbstractionType> abstractionVLNV(new AbstractionType());
    abstractionVLNV->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(absVLNV)));
    busIf->getAbstractionTypes()->append(abstractionVLNV);
    busIf->setBusType(busVLNV);

    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component_, busIf, handler_, selectedPorts, this, absVLNV, 
        dialog.getSignalSelection() == NewBusDialog::USE_DESCRIPTION);
    component_->getBusInterfaces()->append(busIf);   

    connect(&wizard, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&wizard, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    if (wizard.exec() == QWizard::Accepted)
    {
        emit createInterface();
        emit contentChanged();         
    }
    else
    {
        component_->getBusInterfaces()->removeAll(busIf);
    }
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::onCreateInterface()
//-----------------------------------------------------------------------------
void PortsEditor::onCreateInterface(QStringList const& selectedPorts)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    
    QSharedPointer<AbstractionType> abstraction(new AbstractionType());
    QSharedPointer<ConfigurableVLNVReference> abstractionReference(new ConfigurableVLNVReference());
    abstractionReference->setType(VLNV::ABSTRACTIONDEFINITION);
    abstraction->setAbstractionRef(abstractionReference);
    
    busIf->getAbstractionTypes()->append(abstraction);

    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component_, busIf, handler_, selectedPorts, this);
    component_->getBusInterfaces()->append(busIf);   

    connect(&wizard, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&wizard, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    if (wizard.exec() == QWizard::Accepted)
    {
        emit createInterface();
        emit contentChanged();         
    }
    else
    {
        component_->getBusInterfaces()->removeAll(busIf);
    }
}
