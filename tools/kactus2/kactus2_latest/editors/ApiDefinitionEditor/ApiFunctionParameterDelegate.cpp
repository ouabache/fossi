//-----------------------------------------------------------------------------
// File: ApiFunctionParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 18.4.2012
//
// Description:
// Delegate for editing API function parameters.
//-----------------------------------------------------------------------------

#include "ApiFunctionParameterDelegate.h"
#include "ApiFunctionColumns.h"

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::ApiFunctionParameterDelegate()
//-----------------------------------------------------------------------------
ApiFunctionParameterDelegate::ApiFunctionParameterDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    builtInTypes_.append("char");
    builtInTypes_.append("char*");
    builtInTypes_.append("const char*");
    builtInTypes_.append("double");
    builtInTypes_.append("float");
    builtInTypes_.append("int");
    builtInTypes_.append("long");
    builtInTypes_.append("short");
    builtInTypes_.append("signed char");
    builtInTypes_.append("unsigned char");
    builtInTypes_.append("unsigned int");
    builtInTypes_.append("unsigned long");
    builtInTypes_.append("unsigned short");
    builtInTypes_.append("void");
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::~ApiFunctionParameterDelegate()
//-----------------------------------------------------------------------------
ApiFunctionParameterDelegate::~ApiFunctionParameterDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::updateDataTypes()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::updateDataTypes(QStringList const& dataTypes)
{
    customTypes_ = dataTypes;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ApiFunctionParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
                                                    QModelIndex const& index) const
{
    if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_NAME ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_DESC)
    {
        QLineEdit* lineEditor = new QLineEdit(parent);
        connect(lineEditor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return lineEditor;
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_TYPE)
    {
        QComboBox* box = new QComboBox(parent);
        box->setEditable(true);
        box->setInsertPolicy(QComboBox::InsertAlphabetically);

        box->addItems(getDataTypesList());
        return box;
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION)
    {
        QComboBox* box = new QComboBox(parent);
        box->addItem(tr("any"));
        box->addItem(tr("in"));
        box->addItem(tr("out"));
        box->addItem(tr("inout"));

        return box;
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE)
    {
        QComboBox* box = new QComboBox(parent);
        box->addItem("any");

        // Fetch allowed transfer types from the linked COM definition.
        if (comDefinition_ != 0)
        {
            box->addItems(*comDefinition_->getTransferTypes());
        }

        return box;
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_CONTENT_SOURCE)
    {
        QComboBox* box = new QComboBox(parent);

        // Fetch content sources from a COM definition properties.
        box->addItem("");
        box->addItem("Name");

        if (comDefinition_ != 0)
        {
            foreach (QSharedPointer<ComProperty const> property, *comDefinition_->getProperties())
            {
                box->addItem(property->name());
            }
        }

        return box;
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)
    {
        QComboBox* box = new QComboBox(parent);
        box->addItem("no");

        for (int i = 0; i < index.row(); ++i)
        {
            box->addItem(QString::number(i + 1));
        }

        return box;
    }

    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_TYPE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_CONTENT_SOURCE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)  
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QComboBox");

        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        box->setCurrentIndex(box->findText(text));
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_NAME ||
        index.column() ==  ApiFunctionColumns::API_FUNC_PARAM_COL_DESC)
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        line->setText(text);
    }

    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_TYPE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_CONTENT_SOURCE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)  
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QComboBox");

        QString text = box->currentText();
        model->setData(index, text, Qt::EditRole);
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_NAME ||
        index.column() ==  ApiFunctionColumns::API_FUNC_PARAM_COL_DESC)
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = line->text();
        model->setData(index, text, Qt::EditRole);
    }

    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::commitAndCloseEditor()
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

    if (lineEdit != 0)
    {
        emit commitData(lineEdit);
        emit closeEditor(lineEdit);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::getDataTypesList()
//-----------------------------------------------------------------------------
QStringList ApiFunctionParameterDelegate::getDataTypesList() const
{
    QStringList list = builtInTypes_ + customTypes_;
    list.sort();
    list.removeDuplicates();
    return list;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::setComDefinition()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::setComDefinition(QSharedPointer<ComDefinition const> comDefinition)
{
    comDefinition_ = comDefinition;
}
