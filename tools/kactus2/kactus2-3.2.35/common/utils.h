/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: utils.h
 * 		
 * 		Description: This file contains declarations for general purpose 
 * 		functions that can be used in the whole software.
 */

#ifndef UTILS_H
#define UTILS_H

#include <common/Global.h>

#include <QRegularExpression>
#include <QObject>

namespace Utils {

	//! Contains firmness search settings.
	struct ImplementationOptions {
	
		//! If true then hardware components should be included in search.
		bool hw_;

		//! If true then software components should be included in search.
		bool sw_;

		//! If true then system components should be included in search.
		bool system_;

		/*! The default constructor
		 *
		 * Constructs struct with all options set to true.
		*/
		ImplementationOptions();
	};

	//! Contains the search settings for hierarchy.
	struct HierarchyOptions {

		//! If true then global objects should be included in search.
		bool flat_;

		//! If true then product objects should be included in search.
		bool product_;

		//! If true then board objects should be included in search.
		bool board_;

		//! If true then chip objects should be included in search.
		bool chip_;

		//! If true then soc objects should be included in search.
		bool soc_;

		//! If true then ip objects should be included in search.
		bool ip_;

		/*! The default constructor.
		 *
		 * Constructs struct with all options set to true.
		*/
		HierarchyOptions();
	};

	//! Contains the Re-usability search filters.
	struct FirmnessOptions {

		//! If true then templates should be included in search.
		bool templates_;

		//! If true then mutables should be included in search.
		bool mutable_;

		//! If true then fixeds should be included in search.
		bool fixed_;

		/*! The default constructor
		 *
		 * Constructs struct with all options set to true
		*/
		FirmnessOptions();
	};

	//! Contains the search options for document types.
	struct TypeOptions {

		//! If true then components should be included in search.
		bool components_;

		//! If true then bus definitions should be included in search.
		bool buses_;

		//! If true then other IP-Xact types should be included in search.
		bool advanced_;

		/*! The default constructor
		 *
		 * Constructs struct with all options set to true.
		*/
		TypeOptions();
	};

	//! The struct to handle a port name and its left and right boundary.
	struct Mapping {

	//! The name of the port.
		QString portName_;

	//! The left bound of the port.
		int left_;

	//! The right bound of the port.
		int right_;

		/*! The default constructor
		 *
		*/
		Mapping();

		/*! The constructor
		 *
		 * \param name The name for the port in the mapping.
		 *
		*/
		Mapping(const QString& name);

		/*! The constructor
		 *
		 * \param name The name for the port in the mapping.
		 * \param left The left bound for the port.
		 * \param right The right bound for the port.
		 *
		*/
		Mapping(const QString& name, int left, int right);

		//! Operator that returns true if the port names are identical.
		bool operator==(const Mapping& other) const;

		//! Operator that returns true if the port names are not identical.
		bool operator!=(const Mapping& other) const;

		//! Operator for sorting mappings by the name
		bool operator<(const Mapping& other) const;

		//! Operator for sorting mappings by name
		bool operator>(const Mapping& other) const;
	};

	/*! Convert a string to unsigned int format.
	 * 
	 * The multiples in the string are converted as following:
	 * k/K = 2^10
	 * M   = 2^20
	 * G   = 2^30
	 * T   = 2^40
	 * P   = 2^50
	 * 
	 * \param str The string to convert.
	 *
	 * \return The result of the conversion.
	*/
	KACTUS2_API quint64 str2Uint(const QString& str);

	/*! Convert a string to int format.
	 * 
	 * The multiples in the string are converted as following:
	 * k/K = 2^10
	 * M   = 2^20
	 * G   = 2^30
	 * T   = 2^40
	 * P   = 2^50
	 * 
	 * \param str The string to convert.
	 *
	 * \return The result of the conversion.
	*/
	KACTUS2_API qint64 str2Int(const QString& str);

    //! Regular expression to validate URLs.
    const QRegularExpression URL_VALIDITY_REG_EXP = QRegularExpression(
        "^[a-z]{3,9}[:][/]{2}[a-z0-9]+([-.][a-z0-9]+)*([/][-a-z0-9_.;,?=&%#~+]*)*$",
        QRegularExpression::CaseInsensitiveOption);

    /*!
     *  Retrieves and returns the name of the current user.
     */
    KACTUS2_API QString getCurrentUser();
}

#endif // UTILS_H
