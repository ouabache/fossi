//-----------------------------------------------------------------------------
// File: VerilogSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.09.2014
//
// Description:
// Common elements of the Verilog language.
//-----------------------------------------------------------------------------

#ifndef VERILOGSYNTAX_H
#define VERILOGSYNTAX_H

#include <QRegularExpression>

namespace VerilogSyntax
{
	//! Warning that is generated to generated files.
	const QString TAG_OVERRIDE("WARNING: EVERYTHING ON AND ABOVE THIS LINE MAY BE OVERWRITTEN BY KACTUS2!!!");

    //! Module begins with module <name> #(<parameters>) (<ports>);.
    const QRegularExpression MODULE_BEGIN("module\\s+(\\w+)\\s*(#\\s*[(].*[)])?\\s*(?=([(][^)]*[)])?\\s*;)?",
        QRegularExpression::DotMatchesEverythingOption);

	//! Module header begins with module <name> #(.
	const QRegularExpression MODULE_KEY_WORD("module\\s+(\\w+)\\s*(#?\\s*[(])");

    //! Module ends with keyword endmodule.
    const QRegularExpression MODULE_END("endmodule");

    //! Pattern for ranges for e.g. port sizes are given as [left:right].
    const QString RANGE("\\[.*?\\s*[:]\\s*.*?\\]");

    //! Regular expression for capturing range terms.
    const QRegularExpression CAPTURING_RANGE("\\[(.*?)\\s*[:]\\s*(.*?)\\]");

    /*  Identifiers e.g. port names may contain characters a-z, A-Z, numbers, underscores and dollar signs.
     *  Multiple identifiers declared at once must be comma separated.
     */
    const QString NAMES("[a-zA-Z0-9_$]+(?:\\s*[,]\\s*[a-zA-Z0-9_$]+)*");

    //! Pattern for one-line comments.
    const QString COMMENT("//[ \\t]*([^\\r\\n]*)(?=\\r?\\n|$)");

    //! Single-line comments.
    const QRegularExpression COMMENTLINE("(^|\\r?\\n)[ \\t]*" + COMMENT);

    //! Multi-line comments.
    const QRegularExpression MULTILINE_COMMENT("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption);  

    //! An expression that may an operator or an alphanumeric symbol.
    const QString OPERATION_OR_ALPHANUMERIC("([+*\\(\\)\\{\\}/-])|([']?\\{)|(`?\\w+)|((\\w+)?'\\w+)");
    
    //! Name + value pair, e.g. name=value.
    const QString NAME_VALUE = "\\w+\\s*=(\\s*(" + OPERATION_OR_ALPHANUMERIC + "))+";  
}

#endif // VERILOGSYNTAX_H