//-----------------------------------------------------------------------------
// File: VhdlSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 4.7.2013
//
// Description:
// This file contains common elements of the VHDL language
//-----------------------------------------------------------------------------

#ifndef VHDLSYNTAX_H
#define VHDLSYNTAX_H

#include <Plugins/common/HDLmath.h>

#include <QRegularExpression>

namespace VHDLSyntax
{
    //! Whitespace without line end.
    const QString SPACE = "[ \\t]*";    

    //! Platform-independent endline.
    const QString ENDLINE = "(\\r\\n?|\\n\\r?)";

    //! VHDL comment.
    const QString COMMENT = "[-][-]" + SPACE + "([^\\r\\n]*)(?=" + ENDLINE + "|$)";
   
    //! VHDL port and generic names are separated using a colon (,).
    const QString NAMES = "\\w+(?:\\s*[,]?\\s*\\w+)*";

    //! Supported std_logic values.
    const QString STD_ULOGIC = "['][U|X|0|1|Z|W|L|H|-][']";

    //! Others is used to initialize a group of bits in std_logic_vector.
    const QString OTHERS_EXP = "[(]\\s*others\\s*=>\\s*" + STD_ULOGIC + "\\s*[)]";

    //! VHDL default value definition is mathematical expression, vector assignment with others, logical value
    // or other string value.
    const QString DEFAULT = "[:][=]\\s*((?:" + HDLmath::TERM + ")|" + OTHERS_EXP + "|[^\\s:;]+)";

    //! VHDL pragmas begin like comments with -- followed by a reserved word.
    const QString PRAGMA = "[-][-]" + SPACE + "(?:PRAGMA|SYNTHESIS|SYNOPSYS)[^\\r\\n]*";

    //! VHDL comment line.
    const QString COMMENT_LINE_EXP = SPACE + COMMENT + "(?=" + ENDLINE + ")";

    /*! All except the last port/generic declaration ends with a semicolon. 
     *  If there is a comment on the same line, it will be captured to the description by VhdlParser.
     *  Additional comments may follow the last declaration before the end of the string.
     */
    const QString DECLARATION_END = "(?:\\s*[;]" + SPACE + "(?:" + COMMENT + "))|"
                                    "(?:(?=\\s*[;]))|"
                                    "(?:" + SPACE + "(?:" + COMMENT + ")?(?=(?:\\s*(?:" + COMMENT + ")*\\s*)*$))";
    
    //! Entity declaration is ENTITY <name> IS ... END [ENTITY] [<name>];
    const QRegularExpression ENTITY_BEGIN_EXP("(ENTITY)\\s+(\\w+)\\s+(IS)", 
        QRegularExpression::CaseInsensitiveOption);   
    const QRegularExpression ENTITY_END_EXP("(END)\\s*(ENTITY)?\\s*(\\w+)?\\s*[;]", 
        QRegularExpression::CaseInsensitiveOption);

}

#endif // VHDLSYNTAX_H
