//-----------------------------------------------------------------------------
// File: globalheadersavemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.02.2013
//
// Description:
// The model class to display the header files to be created for CPU instances within a design.
//-----------------------------------------------------------------------------

#include "globalheadersavemodel.h"

#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QDir>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::GlobalHeaderSaveModel()
//-----------------------------------------------------------------------------
GlobalHeaderSaveModel::GlobalHeaderSaveModel( LibraryInterface* handler, QObject *parent ):
QAbstractTableModel(parent),
handler_(handler),
table_(),
comp_(),
design_()
{

}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::~GlobalHeaderSaveModel()
//-----------------------------------------------------------------------------
GlobalHeaderSaveModel::~GlobalHeaderSaveModel()
{
	qDeleteAll(table_);
	table_.clear();
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::rowCount()
//-----------------------------------------------------------------------------
int GlobalHeaderSaveModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return table_.size();
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::columnCount()
//-----------------------------------------------------------------------------
int GlobalHeaderSaveModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return GlobalHeaderSaveModel::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags GlobalHeaderSaveModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::headerData()
//-----------------------------------------------------------------------------
QVariant GlobalHeaderSaveModel::headerData( int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}
	if (Qt::DisplayRole == role)
    {
        if (section == GlobalHeaderSaveModel::INSTANCE_NAME)
        {
            return tr("Instance\nname");
        }
        else if (section == GlobalHeaderSaveModel::INTERFACE_NAME)
        {
            return tr("Interface\nname");
        }
        else if (section == GlobalHeaderSaveModel::FILE_NAME)
        {
            return tr("File name");
        }
        else if (section == GlobalHeaderSaveModel::FILE_PATH)
        {
            return tr("File path");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::data()
//-----------------------------------------------------------------------------
QVariant GlobalHeaderSaveModel::data( const QModelIndex& index, int role) const
{
	if (!index.isValid())
    {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= table_.size())
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (index.column() == GlobalHeaderSaveModel::INSTANCE_NAME)
        {
            return table_.at(index.row())->instance_;
        }
        else if (index.column() == GlobalHeaderSaveModel::INTERFACE_NAME)
        {
            return table_.at(index.row())->interface_;
        }
        else if (index.column() == GlobalHeaderSaveModel::FILE_NAME)
        {
            return table_.at(index.row())->fileInfo_.fileName();
        }
        else if (index.column() == GlobalHeaderSaveModel::FILE_PATH)
        {
            Q_ASSERT(comp_);

            VLNV identifier = table_.at(index.row())->comp_;

            // display the relative path from xml directory to the header to be generated
            QDir xmlDir(handler_->getDirectoryPath(comp_->getVlnv()));
            QString headerPath = table_.at(index.row())->fileInfo_.absoluteFilePath();
            QString relPath = xmlDir.relativeFilePath(headerPath);

            return relPath;
        }
        else
        {
            return QVariant();
        }
	}
	// user role always returns the absolute file path
	else if (Qt::UserRole == role)
    {
		Q_ASSERT(comp_);

		// if the header dir exists
		QFileInfo headerInfo(table_.at(index.row())->fileInfo_.absolutePath());
		if (headerInfo.exists())
        {
			// return path to the directory for headers
			return headerInfo.absoluteFilePath();
		}

		// if not then at least the xml directory exists
		else
        {
			return handler_->getDirectoryPath(comp_->getVlnv());
		}		
	}
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::setData()
//-----------------------------------------------------------------------------
bool GlobalHeaderSaveModel::setData( const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid())
    {
		return false;
	}
	else if (index.row() < 0 || index.row() >= table_.size())
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == GlobalHeaderSaveModel::INSTANCE_NAME ||
            index.column() == GlobalHeaderSaveModel::INTERFACE_NAME)
        {
            return false;
        }
        else if (index.column() == GlobalHeaderSaveModel::FILE_NAME)
        {
            table_[index.row()]->fileInfo_.setFile(value.toString());

            // both indexes change
            QModelIndex pathIndex =
                QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_PATH, QModelIndex());
            emit dataChanged(index, pathIndex);
            return true;
        }
        else if (index.column() == GlobalHeaderSaveModel::FILE_PATH)
        {
            table_[index.row()]->fileInfo_.setFile(value.toString());

            // both columns change
            QModelIndex nameIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_NAME, QModelIndex());
            emit dataChanged(nameIndex, index);
            return true;
        }
        else
        {
            return false;
        }
	}
	else if (Qt::UserRole == role)
    {
		table_[index.row()]->fileInfo_.setFile(value.toString());

		// the change affects to both columns
		QModelIndex leftIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_NAME, QModelIndex());
		QModelIndex rightIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_PATH, QModelIndex());

		// inform of changes
		emit dataChanged(leftIndex, rightIndex);
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::getHeaderOptions()
//-----------------------------------------------------------------------------
const QList<GlobalHeaderSaveModel::SaveFileOptions*>& GlobalHeaderSaveModel::getHeaderOptions() const
{
	return table_;
}

//-----------------------------------------------------------------------------
// Function: globalheadersavemodel::setDesign()
//-----------------------------------------------------------------------------
void GlobalHeaderSaveModel::setDesign( QSharedPointer<Component> topComp, QSharedPointer<Design> design )
{
	beginResetModel();

	// update the component and design used
	comp_ = topComp;
	design_ = design;

	// remove previous items
	qDeleteAll(table_);
	table_.clear();

	foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
		// parse the component model for the instance
		VLNV compVLNV = *instance->getComponentRef();
		QSharedPointer<const Document> libComp = handler_->getModelReadOnly(compVLNV);
		QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();
		
        // headers are only generated for CPUs and their master interfaces
        if (!comp || !comp->isCpu())
        {
			continue;
		}
		
		// create header for each master interface
		QStringList masterInterfaces = comp->getMasterInterfaces();
		foreach (QString interfaceName, masterInterfaces)
        {
			// if the operated interface is not connected to any other instance within the design
			if (!design_->hasInterconnection(instance->getInstanceName(), interfaceName))
            {
				continue;
			}
			
			GlobalHeaderSaveModel::SaveFileOptions* options = new SaveFileOptions();

			options->instance_ = instance->getInstanceName();
			options->interface_ = interfaceName;
			options->comp_ = compVLNV;
			options->instanceId_ = instance->getUuid();

			// the path to the directory containing the xml metadata
			QString compPath(handler_->getDirectoryPath(topComp->getVlnv()));

			// the relative path from the xml dir to the header to generate
			QString headerPath = QString("%1/%2/%3.h").arg(
                tr("headers"), instance->getInstanceName(), interfaceName);

			// the absolute path to the header file
			const QString fullPath = QString("%1/%2").arg(compPath, headerPath);

			// create the file info instance
			options->fileInfo_ = QFileInfo(fullPath);

			table_.append(options);
		}
	}

	endResetModel();
}
