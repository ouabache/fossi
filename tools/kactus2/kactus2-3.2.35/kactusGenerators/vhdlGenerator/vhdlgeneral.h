/* 
 *  	Created on: 27.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneral.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERAL_H
#define VHDLGENERAL_H

#include <QString>
#include <QTextStream>

namespace VhdlGeneral {

	//! \brief The number of supported vhdl types.
	const unsigned int VHDL_TYPE_COUNT = 17;

	//! \brief The list of supported vhdl types.
	const QString VHDL_TYPES[] = {
		"bit",
		"bit_vector",
		"boolean",
		"character",
		"integer",
		"natural",
		"positive",
		"real",
		"severity_level",
		"signed",
		"std_logic",
		"std_logic_vector",
		"std_ulogic",
		"std_ulogic_vector",
		"string",
		"time",
		"unsigned"
	};

	//! \brief The number of supported vhdl type definitions
	const unsigned int VHDL_TYPEDEF_COUNT = 2;

	//! \brief The list of supported vhdl type definitions
	const QString VHDL_TYPE_DEFINITIONS[] = {
		"IEEE.std_logic_1164.all",
		"IEEE.numeric_std.all",
	};

	/*! \brief Check if the port type is scalar.
	 *
	 * \param typeName The name of the port type to check.
	 *
	 * \return bool True if port type is for scalar port.
	*/
	bool isScalarType(const QString& typeName);

	/*! \brief Get the default type definition for a given type name.
	 *
	 * \param typeName Specified the name of the type.
	 *
	 * \return QString contains the default vhdl type definition for given type name.
	*/
	QString getDefaultVhdlTypeDef(const QString& typeName);

	/*! \brief Create a string that defines a vhdl type.
	 * 
	 * If the size of the given bounds is 1 then "std_logic" is returned.
	 * If the size is greater than 1 then "std_logic_vector" is returned.
	 * 
	 * \param leftBound The left bound for the type.
	 * \param rightBound The right bound for the type.
	 *
	 * \return QString contains the the type.
	*/
	QString useDefaultType(const int leftBound, const int rightBound);

	/*! \brief Create a string that contains the type and bounds for it.
	 * 
	 * For std_logic_vector created string is: std_logic_vector(<left> downto <right>)
	 * For std_logic created string is (bounds do not matter): std_logic
	 * For integer created string is: integer range <right> to <left>
	 * 
	 * \param type Specifies the vhdl type.
	 * \param leftBound The left bound or the higher bound for the type.
	 * \param rightBound The right bound or the lower bound for the type.
	 *
	 * \return VhdlGeneral::QString
	*/
	QString vhdlType2String(const QString& type, 
		const int leftBound = -1,
		const int rightBound = -1);

	/*! \brief Write a description of some element to the stream.
	 * 
	 * \param description The description to write.
	 * \param stream      The text stream to write into.
	 * \param indentation Added to the beginning of each new line, e.g."  " or  "\t"
	 *
	*/
	void writeDescription(const QString& description,
		QTextStream& stream, 
		const QString& indentation = QString(""));

	/*! \brief Check the vhdl types if they can be connected to each other.
	 *
	 * \param type1 The first type to compare.
	 * \param type2 The second type to compare.
	 *
	 * \return bool True if the types are compatible.
	*/


	bool checkVhdlTypeMatch(const QString& type1, const QString& type2);

	/*! \brief Take a default value and add "" or '' around it if needed depending on the type
	 *
	 * \param originalDefaultValue The default value
	 * \param type The type of the value.
	 *
	 * \return QString The default value that can be written to vhdl (this contains the "" or '')
	*/
	QString convertDefaultValue(const QString& originalDefaultValue, 
		const QString& type);
}

#endif // VHDLGENERAL_H