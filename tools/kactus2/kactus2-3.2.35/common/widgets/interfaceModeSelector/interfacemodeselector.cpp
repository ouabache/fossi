//-----------------------------------------------------------------------------
// File: interfacemodeselector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The editor to select an interface mode for bus interface within a component.
//-----------------------------------------------------------------------------

#include "interfacemodeselector.h"

#include <IPXACTmodels/Component/BusInterface.h>

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::InterfaceModeSelector()
//-----------------------------------------------------------------------------
InterfaceModeSelector::InterfaceModeSelector(QWidget *parent, QSharedPointer<BusInterface> busif,
    bool showMonitor): QComboBox(parent),
    showMonitor_(showMonitor)
{
    initialize();	

    // if bus interface was specified
    if (busif)
    {
        setCurrentIndex(busif->getInterfaceMode());
    }
    // if there is no bus interface then set "undefined" as selected
    else
    {
        setCurrentIndex(General::INTERFACE_MODE_COUNT);
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::InterfaceModeSelector()
//-----------------------------------------------------------------------------
InterfaceModeSelector::InterfaceModeSelector(General::InterfaceMode mode, QWidget* parent, bool showMonitor):
QComboBox(parent),
showMonitor_(showMonitor)
{
	initialize();

	setCurrentIndex(mode);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::~InterfaceModeSelector()
//-----------------------------------------------------------------------------
InterfaceModeSelector::~InterfaceModeSelector()
{
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::initialize()
//-----------------------------------------------------------------------------
void InterfaceModeSelector::initialize()
{
	// mode names can not be edited
	setEditable(false);

	setSizeAdjustPolicy(QComboBox::AdjustToContents);

	// add all options to the combo box
	for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; ++i)
    {
		// if monitor should not be displayed then don't add it
		if (!showMonitor_ && i == General::MONITOR)
        {
			continue;
		}

		addItem(General::INTERFACE_MODE_NAMES[i]);
	}

	connect(this, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(setMode(const QString&)), Qt::UniqueConnection);

	// the interface mode is mandatory by default
	setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::setMode()
//-----------------------------------------------------------------------------
void InterfaceModeSelector::setMode(const General::InterfaceMode mode)
{
	// if the mode the set is monitor and it is not shown
	if (!showMonitor_ && mode == General::MONITOR)
    {
		return;
	}

	disconnect(this, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setMode(const QString&)));
	
	// the names in the combo box are in the same order as the interface modes
	setCurrentIndex(mode);
	
	connect(this, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(setMode(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::setMode()
//-----------------------------------------------------------------------------
void InterfaceModeSelector::setMode(QString const& modeName)
{
	disconnect(this, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setMode(const QString&)));

	// find the item for the text
	int index = findText(modeName, Qt::MatchFixedString);

	// if text was found
	if (index >= 0)
    {
		setCurrentIndex(index);
		emit modeSelected(static_cast<General::InterfaceMode>(index));
	}

	connect(this, SIGNAL(currentIndexChanged(const QString&)), 
        this, SLOT(setMode(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceModeSelector::selected()
//-----------------------------------------------------------------------------
General::InterfaceMode InterfaceModeSelector::selected() const
{
	int index = currentIndex();
	return static_cast<General::InterfaceMode>(index);
}
