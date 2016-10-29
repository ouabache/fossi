//-----------------------------------------------------------------------------
// File: FileTypesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 20.02.2013
//
// Description:
// Delegate which provides widgets for the file types settings view.
//-----------------------------------------------------------------------------

#include "FileTypesDelegate.h"

#include "FileTypesModel.h"

#include <IPXACTmodels/common/FileTypes.h>

#include <QComboBox>
#include <QLineEdit>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileTypesDelegate::FileTypesDelegate()
//-----------------------------------------------------------------------------
FileTypesDelegate::FileTypesDelegate(QObject* parent /*= 0*/)
    : QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileTypesDelegate::~FileTypesDelegate()
//-----------------------------------------------------------------------------
FileTypesDelegate::~FileTypesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: FileTypesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileTypesDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    switch (index.column())
    {
    case FILE_TYPES_COL_NAME:
        {
            QComboBox* combo = new QComboBox(parent);

				// contains all file types that have been defined already
				QStringList usedTypes = index.model()->data(index, Qt::UserRole).toStringList();

            for (unsigned int i = 0; i < FileTypes::FILE_TYPE_COUNT; ++i)
            {
                // if the file type is not yet defined then add it to the list of possible types
                if (!usedTypes.contains(FileTypes::FILE_TYPES[i]))
                {
                    combo->addItem(FileTypes::FILE_TYPES[i]);
                }
            }
            
            combo->setEditable(true);
            return combo;
        }

    case FILE_TYPES_COL_EXTENSIONS:
        {
            QLineEdit* line = new QLineEdit(parent);
            return line;
        }

    default:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileTypesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FileTypesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    switch (index.column())
    {
    case FILE_TYPES_COL_NAME:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(combo, "FileTypesDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            combo->setEditText(text);
            break;
        }
        
    case FILE_TYPES_COL_EXTENSIONS:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "FileTypesDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            line->setText(text);
            break;
        }

    default:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileTypesDelegate::setModelData()
//-----------------------------------------------------------------------------
void FileTypesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    switch (index.column())
    {
    case FILE_TYPES_COL_NAME:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(combo, "FileTypesDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = combo->currentText();
            model->setData(index, text, Qt::EditRole);
            break;
        }

    case FILE_TYPES_COL_EXTENSIONS:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "FileTypesDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = line->text();
            model->setData(index, text, Qt::EditRole);
            break;
        }

    default:
        {
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
        }
    }
}
