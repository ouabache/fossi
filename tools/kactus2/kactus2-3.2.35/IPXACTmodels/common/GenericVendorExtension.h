//-----------------------------------------------------------------------------
// File: GenericVendorExtension.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 8.5.2014
//
// Description:
// Generic implementation for any vendor extension.
//-----------------------------------------------------------------------------

#ifndef GENERICVENDOREXTENSION_H
#define GENERICVENDOREXTENSION_H

#include "VendorExtension.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QObject>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// class GenericVendorExtension.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT GenericVendorExtension : public VendorExtension
{

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] extensionNode   The DOM node representation of the vendor extension.
	 */
	GenericVendorExtension(QDomNode const& extensionNode);

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual GenericVendorExtension* clone() const;

	/*!
	 *  The destructor.
	 */
	virtual ~GenericVendorExtension();

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    //! Returns the DOM node presentation of the extension.
    QDomNode node();

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

private:
	// Disable copying.
    GenericVendorExtension(GenericVendorExtension const& other);

    //! Disable assign.
    GenericVendorExtension& operator=(GenericVendorExtension const& rhs);

    /*!
     *  Writes a DOM node to XML using a given XML writer.
     *
     *      @param [in] node        The DOM node to write.
     *      @param [in] writer      The XML writer to use.
     */
    void writeNode(QDomNode const& node, QXmlStreamWriter& writer) const;

    /*!
     *  Writes all child noted of a DOM node to XML using a given XML writer.
     *
     *      @param [in] node        The DOM node to write.
     *      @param [in] writer      The XML writer to use.
     */
    void writeChildNodes(QDomNode const& node, QXmlStreamWriter& writer) const;

    /*!
     *  Writes all the attributes of a DOM node to XML using a given XML writer.
     *
     *      @param [in] node        The DOM node to write.
     *      @param [in] writer      The XML writer to use.
     */
    void writeAttributes(QDomNode const& node, QXmlStreamWriter& writer) const;
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The DOM representation of the vendor extension.
    QDomNode vendorExtension_;

};
#endif // GENERICVENDOREXTENSION_H

