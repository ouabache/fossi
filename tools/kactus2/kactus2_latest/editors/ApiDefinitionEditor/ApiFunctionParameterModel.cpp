//-----------------------------------------------------------------------------
// File: ApiFunctionParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 18.4.2012
//
// Description:
// Model for API function parameters.
//-----------------------------------------------------------------------------

#include "ApiFunctionParameterModel.h"

#include "ApiFunctionColumns.h"

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/kactusExtensions/ApiFunction.h>
#include <IPXACTmodels/kactusExtensions/ApiFunctionParameter.h>

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::ApiFunctionParameterModel()
//-----------------------------------------------------------------------------
ApiFunctionParameterModel::ApiFunctionParameterModel(QObject *parent): QAbstractTableModel(parent), function_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::~ApiFunctionParameterModel()
//-----------------------------------------------------------------------------
ApiFunctionParameterModel::~ApiFunctionParameterModel()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::setFunction()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::setFunction(QSharedPointer<ApiFunction> function)
{
    beginResetModel();
    function_ = function;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::rowCount()
//-----------------------------------------------------------------------------
int ApiFunctionParameterModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid() || function_ == 0)
    {
        return 0;
    }

    return function_->getParamCount();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ApiFunctionParameterModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ApiFunctionColumns::API_FUNC_PARAM_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ApiFunctionParameterModel::data(QModelIndex const& index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || 
        function_ == 0 || index.row() < 0 || index.row() >= function_->getParamCount())
    {
        return QVariant();
    }

    if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_NAME)
    {
        return function_->getParam(index.row())->name();
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_TYPE)
    {
        return function_->getParam(index.row())->getType();
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE)
    {
        QString transferType = function_->getParam(index.row())->getComTransferType();
        if (transferType.isEmpty())
        {
            return tr("any");
        }

        return transferType;
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION)
    {
        DirectionTypes::Direction direction = function_->getParam(index.row())->getComDirection();
        if (direction == DirectionTypes::DIRECTION_INVALID)
        {
            return tr("any");
        }

        return DirectionTypes::direction2Str(direction);
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_CONTENT_SOURCE)
    {
        return function_->getParam(index.row())->getContentSource();
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)
    {
        int paramIndex = function_->getParam(index.row())->getDependentParameterIndex();
        if (paramIndex < 0)
        {
            return "no";
        }

        return (function_->getParam(index.row())->getDependentParameterIndex() + 1);
    }

    else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_DESC)
    {
        return function_->getParam(index.row())->getDescription();
    }

    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant ApiFunctionParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        if (section == ApiFunctionColumns::API_FUNC_PARAM_COL_NAME)
        {
            return tr("Name");
        }
        else if (section == ApiFunctionColumns::API_FUNC_PARAM_COL_TYPE)
        {
            return tr("Type");
        }
        else if (section == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE)
        {
            return tr("COM transfer type");
        }
        else if (section == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION)
        {
            return tr("COM direction");
        }
        else if (section == ApiFunctionColumns::API_FUNC_PARAM_CONTENT_SOURCE)
        {
            return tr("Source");
        }
        else if (section == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)
        {
            return tr("Dependent parameter");
        }
        else if (section == ApiFunctionColumns::API_FUNC_PARAM_COL_DESC)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return section + 1;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ApiFunctionParameterModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION ||
        index.column() == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)
    {
        if (function_->getParam(index.row())->getContentSource().isEmpty())
        {
            return Qt::ItemIsSelectable;
        }
        else
        {
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
        }
    }

    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::setData()
//-----------------------------------------------------------------------------
bool ApiFunctionParameterModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || function_ == 0 || index.row() < 0 || index.row() >= function_->getParamCount())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_NAME)
        {
            function_->getParam(index.row())->setName(value.toString());
        }

        else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_TYPE)
        {
            function_->getParam(index.row())->setType(value.toString());
        }

        else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_TRANSFER_TYPE)
        {
            if (value.toString() == tr("any"))
            {
                function_->getParam(index.row())->setComTransferType("");
            }
            else
            {
                function_->getParam(index.row())->setComTransferType(value.toString());
            }
        }

        else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COM_DIRECTION)
        {
            function_->getParam(index.row())->setComDirection(DirectionTypes::str2Direction(value.toString(),
                DirectionTypes::DIRECTION_INVALID));
        }

        else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_CONTENT_SOURCE)
        {
            if (value.toString().isEmpty())
            {
                function_->getParam(index.row())->setComDirection(DirectionTypes::DIRECTION_INVALID);
                function_->getParam(index.row())->setComTransferType("");
                function_->getParam(index.row())->setDependentParameterIndex(-1);
            }

            function_->getParam(index.row())->setContentSource(value.toString());

            emit dataChanged(index, this->index(index.row(), index.column() + 3));
        }

        else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_DEPENDENT_PARAM)
        {
            QString text = value.toString();

            if (text == QLatin1String("no"))
            {
                function_->getParam(index.row())->setDependentParameterIndex(-1);
            }
            else
            {
                function_->getParam(index.row())->setDependentParameterIndex(value.toInt() - 1);
            }
        }

        else if (index.column() == ApiFunctionColumns::API_FUNC_PARAM_COL_DESC)
        {
            function_->getParam(index.row())->setDescription(value.toString());
        }

        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::isValid()
//-----------------------------------------------------------------------------
bool ApiFunctionParameterModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onAdd()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onAdd()
{
    beginInsertRows(QModelIndex(), function_->getParamCount(), function_->getParamCount());
    function_->addParam(QSharedPointer<ApiFunctionParameter>(new ApiFunctionParameter()));
    endInsertRows();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onAddItem()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onAddItem(QModelIndex const& index)
{
    int row = function_->getParamCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    function_->addParam(QSharedPointer<ApiFunctionParameter>(new ApiFunctionParameter()), row);
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onRemove()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onRemove(QModelIndex const& index)
{
    if (!index.isValid() || function_ == 0)
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    function_->removeParam(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onRemoveItem(QModelIndex const& index)
{
    // don't remove anything if index is invalid
    if (!index.isValid() || function_ == 0 || index.row() < 0 || index.row() >= function_->getParamCount())
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    function_->removeParam(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onMoveItem()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos)
{
    // if there was no item in the starting point
    if (!originalPos.isValid() || originalPos == newPos || 
        originalPos.row() < 0 || originalPos.row() >= function_->getParamCount())
    {
        return;
    }

    // if the new position is invalid index then put the item last in the table
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= function_->getParamCount())
    {
        beginResetModel();

        QSharedPointer<ApiFunctionParameter> param = function_->getParam(originalPos.row());
        function_->removeParam(originalPos.row());
        function_->addParam(param);

        endResetModel();
    }
    // if both indexes were valid
    else
    {
        beginResetModel();
        
        QSharedPointer<ApiFunctionParameter> param = function_->getParam(originalPos.row());
        function_->removeParam(originalPos.row());
        function_->addParam(param, newPos.row());

        endResetModel();
    }

    emit contentChanged();
}