//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGEXPRESSIONPARSER_H
#define SYSTEMVERILOGEXPRESSIONPARSER_H

#include "ExpressionParser.h"

#include <QString>

//-----------------------------------------------------------------------------
//! Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------
class SystemVerilogExpressionParser : public ExpressionParser
{
public:

	//! The constructor.
	SystemVerilogExpressionParser();

	//! The destructor.
	virtual ~SystemVerilogExpressionParser();

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;

    /*!
     *  Check if the given expression is an array.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is an array, otherwise false.
     */
    virtual bool isArrayExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is a plain value and does not need evaluation.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a plain value, otherwise false.
     */
    virtual bool isPlainValue(QString const& expression) const;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const;

protected:
     
    /*!
     *  Parses a constant number to a real number.
     *
     *      @param [in] constantNumber   The constant to parse.
     *
     *      @return The real value of the constant.
     */
    virtual qreal parseConstantToDecimal(QString const& constantNumber) const;

        /*!
     *  Splits the given expression to string list with terms and operations as separate items.
     *
     *      @param [in] expression   The expression to split.
     *
     *      @return The separated list.
     */
    QStringList toStringList(QString const& expression) const;

private:

	// Disable copying.
	SystemVerilogExpressionParser(SystemVerilogExpressionParser const& rhs);
	SystemVerilogExpressionParser& operator=(SystemVerilogExpressionParser const& rhs);

    /*!
     *  Checks if the given expression is a string.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a string, otherwise false.
     */
    bool isStringLiteral(QString const &expression) const;

    /*!
     *  Checks if the given expression is a numeric literal.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a literal, otherwise false.
     */
    bool isLiteral(QString const& expression) const;

    /*!
     *  Check if the selected expression is a comparison.
     *
     *      @param [in] expression  The selected expression.
     *
     *      @return True, if the selected expression is a comparison, otherwise false.
     */
    bool isComparison(QString const& expression) const;

    /*!
     *  Parse the selected comparison.
     *
     *      @param [in] expression  The selected comparison.
     *
     *      @return 1 for a comparison with a value of true, 0 for false.
     */
    QString parseComparison(QString const& expression) const;

    QString parseArray(QString const& expression) const;

    /*!
     *  Splits the given operand to string list with terms and parentheses as separate items.
     *
     *      @param [in] operand   The operand to split.
     *
     *      @return The separated list.
     */
    QStringList parseLiteralAndParentheses(QString const& operand) const;

    /*!
     *  Solves function calls to math functions in a given equation.
     *
     *      @param [in/out] equation   The equation to solve.
     */
    void solveMathFuctions(QStringList& equation) const;
    
    /*!
     *  Solves the SystemVerilog $clog2 function.
     *
     *      @param [in] value   The value for which the function is called.
     *
     *      @return The solved value.
     */
    QString solveClog2(QString const& value) const;

    /*!
     *  Solves expressions in parentheses in a given equation.
     *
     *      @param [in/out] equation   The equation to solve.
     */
    void solveExpressionsInParentheses(QStringList& equation) const;

    /*!
     *  Finds the matching end parenthesis in the given equation for the opening parenthesis in given position.
     *
     *      @param [in] equation            The equation to search for ending parenthesis.
     *      @param [in] parenthesesStart    The position of the opening parenthesis.
     *
     *      @return The position of the ending parenthesis.
     */
    int findMatchingEndParenthesis(QStringList const& equation, int parenthesesStart) const;

    int findMatchingEndParenthesis(QString const& equation, int parenthesesStart) const;

    /*!
     *  Solves power operations in a given equation.
     *
     *      @param [in/out] equation   The equation to solve.
     */  
    void solvePower(QStringList& equation) const;

    /*!
     *  Solves multiply and division operations in a given equation.
     *
     *      @param [in/out] equation   The equation to solve.
     */
    void solveMultiplyAndDivide(QStringList& equation) const;

    /*!
    *  Solves addition and subtraction operations in given equation.
    *
    *      @param [in/out] equation   The equation to solve.
    */
    void solveAdditionAndSubtraction(QStringList& equation) const;

    /*!
    *  Solves binary operations in given equation.
    *
    *      @param [in/out]  equation         The equation to solve.
    *      @param [in]      binaryOperator   An expression for finding the operator to solve.
    */
    void solveBinaryOperationsFromLeftToRight(QStringList& equation, QRegularExpression const& binaryOperator) const;

    /*!
    *  Solves a binary operation.
    *
    *      @param [in] firstTerm   The first term of the operation.
    *      @param [in] operation   The operation to solve.
    *      @param [in] secondTerm  The second term of the operation.
    *
    *      @return The result of the operation.
    */
    QString solve(QString const& firstTerm, QString const& operation, QString const& secondTerm) const;
       
    /*!
     *  Get the precision used from the terms.
     *
     *      @param [in] firstTerm   The first term of the operation.
     *      @param [in] secondTerm  The second term of the operation.
     *
     *      @return The precision of the decimal used in the terms of the operation.
     */
    int getDecimalPrecision(QString const& firstTerm, QString const& secondTerm) const;

    /*!
     *  Get the base for a given number.
     *
     *      @param [in] constantNumber  The selected number.
     *
     *      @return The base for the selected number. Either 2, 8, 10 or 16.
     */
    int getBaseForNumber(QString const& constantNumber) const;

    /*!
     *  Converts the base format to the base number e.g. h to 16.
     *
     *      @param [in] baseFormat   The format to convert.
     *
     *      @return The base number for the format.
     */
    int baseForFormat(QString const& baseFormat) const;
};

#endif // SYSTEMVERILOGEXPRESSIONPARSER_H
