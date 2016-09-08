//-----------------------------------------------------------------------------
// File: InterfaceDirectionNameSorter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2014
//
// Description:
// Class for sorting component port names by interface, direction and name.
//-----------------------------------------------------------------------------

#ifndef INTERFACEDIRECTIONNAMESORTER_H
#define INTERFACEDIRECTIONNAMESORTER_H

#include "PortSorter.h"

#include "../veriloggeneratorplugin_global.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/DirectionTypes.h>

//-----------------------------------------------------------------------------
//! Class for sorting component port names by interface, direction and name.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT InterfaceDirectionNameSorter : public PortSorter
{
public:

	//! The constructor.
	InterfaceDirectionNameSorter();

	//! The destructor.
	~InterfaceDirectionNameSorter();

    /*!
     *  Gets the sorted port names of a component.
     *
     *      @param [in] component   The component whose port names to find.
     *
     *      @return The sorted port names of a given component.
     */
    virtual QStringList sortedPortNames(QSharedPointer<Component> component) const;

private:

	// Disable copying.
	InterfaceDirectionNameSorter(InterfaceDirectionNameSorter const& rhs);
	InterfaceDirectionNameSorter& operator=(InterfaceDirectionNameSorter const& rhs);

    //! Helper struct for sorting port names.
    struct SortKey 
    {
        QString interfaceName;              //<! The name of the port's interface.
        DirectionTypes::Direction portDirection;   //<! The direction of the port.
        QString portName;                   //<! The name of the port.

        //! Convenience constructor for SortKey.
        SortKey(QString interfaceName, DirectionTypes::Direction direction, QString portName) : 
            interfaceName(interfaceName), portDirection(direction), portName(portName)  {}

        //! Comparison operator for sorting.
        bool operator<(SortKey const& other) const;
    };
};

#endif // INTERFACEDIRECTIONNAMESORTER_H
