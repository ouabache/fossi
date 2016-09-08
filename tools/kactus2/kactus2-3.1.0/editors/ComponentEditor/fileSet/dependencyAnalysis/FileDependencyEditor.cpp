//-----------------------------------------------------------------------------
// File: FileDependencyEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 22.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#include "FileDependencyEditor.h"

#include "FileDependencyColumns.h"
#include "FileDependencySourceDialog.h"
#include "FileDependencyItem.h"
#include "FileDependencyDelegate.h"

#include <common/widgets/ScanProgressWidget/scanprogresswidget.h>
#include <common/utils.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <IPXACTmodels/common/VLNV.h>

#include <Plugins/PluginSystem/PluginManager.h>
#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfoList>
#include <QSettings>
#include <QHeaderView>
#include <QDebug>
#include <QAction>
#include <QVariant>

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::FileDependencyEditor(QSharedPointer<Component> component,
                                           QString const& basePath,
                                           PluginManager const& pluginMgr, QWidget* parent)
    : QWidget(parent),
      toolbar_(this),
      progressBar_(this),
      graphWidget_(this),
      infoWidget_(this),
      component_(component),
      fileTypeLookup_(),
      ignoreExtList_(),
      model_(pluginMgr, component, basePath + "/"),
      basePath_(basePath),
      scanning_(false),
      filterActions_(this),
      runAnalysisAction_(0),
      timer_(0),
      progWidget_(0)
{
    // Initialize the widgets.
    progressBar_.setStyleSheet("QProgressBar:horizontal { margin: 0px; border: none; background: #cccccc; } "
                               "QProgressBar::chunk:horizontal { background: #009eff;}");
    progressBar_.setFixedHeight(2);
    progressBar_.setTextVisible(false);
    progressBar_.setContentsMargins(0, 0, 0, 0);
    progressBar_.setValue(0);

    graphWidget_.setContentsMargins(0, 0, 0, 0);
    graphWidget_.getView()->setModel(&model_);
    graphWidget_.getView()->setItemDelegate(new FileDependencyDelegate(component, this));

    graphWidget_.getView()->resizeColumnToContents(FileDependencyColumns::TREE_EXPAND);
    graphWidget_.getView()->resizeColumnToContents(FileDependencyColumns::STATUS);
    graphWidget_.getView()->resizeColumnToContents(FileDependencyColumns::CREATE_DEPENDENCY);
    graphWidget_.getView()->setColumnWidth(FileDependencyColumns::PATH, 250);
    graphWidget_.getView()->header()->setSectionResizeMode(FileDependencyColumns::TREE_EXPAND, QHeaderView::Fixed);
    graphWidget_.getView()->header()->setSectionResizeMode(FileDependencyColumns::STATUS, QHeaderView::Fixed);
    graphWidget_.getView()->header()->setSectionResizeMode(FileDependencyColumns::CREATE_DEPENDENCY, QHeaderView::Fixed);

    filterActions_.setExclusive(false);

    // Set up the toolbar
    toolbar_.setFloatable(false);
    toolbar_.setMovable(false);
    toolbar_.setStyleSheet(QString("QToolBar { border: none; }"));

    // Create the filter buttons in the toolbar.
    addFilterButton(QIcon(":/icons/common/graphics/traffic-light_green.png"), "Show Unchanged Files",
                    FileDependencyGraphView::FILTER_GREEN);
    addFilterButton(QIcon(":/icons/common/graphics/traffic-light_yellow.png"), "Show Changed Files (Only Contents Changed)",
                    FileDependencyGraphView::FILTER_YELLOW);
    addFilterButton(QIcon(":/icons/common/graphics/traffic-light_red.png"), "Show Changed Files (Dependencies Changed)",
                    FileDependencyGraphView::FILTER_RED);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_twoway.png"), "Show Bidirectional Dependencies",
                    FileDependencyGraphView::FILTER_TWO_WAY);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_oneway.png"), "Show Unidirectional Dependencies",
                    FileDependencyGraphView::FILTER_ONE_WAY);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_manual.png"), "Show Manual Dependencies",
                    FileDependencyGraphView::FILTER_MANUAL);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_auto.png"), "Show Analyzed Dependencies",
                    FileDependencyGraphView::FILTER_AUTOMATIC);
    addFilterButton(QIcon(":/icons/common/graphics/external.png"), "Show External",
                    FileDependencyGraphView::FILTER_EXTERNAL);
    addFilterButton(QIcon(":/icons/common/graphics/diff.png"), "Show Differences",
                    FileDependencyGraphView::FILTER_DIFFERENCE);
    connect(&filterActions_, SIGNAL(triggered(QAction*)), this, SLOT(filterToggle(QAction*)));

    toolbar_.addSeparator();
    toolbar_.addAction(QIcon(":/icons/common/graphics/import_folders.png"), "Import Source Directories",
                       this, SLOT(openSourceDialog()));
    runAnalysisAction_ = toolbar_.addAction(QIcon(":/icons/common/graphics/control-play.png"), "Rescan",
                                            this, SLOT(scan()));

    // Create the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&toolbar_);
    layout->addWidget(&progressBar_);
    layout->addWidget(&graphWidget_, 1);
    layout->addWidget(&infoWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(dependenciesChanged()), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(analysisProgressChanged(int)),
            this, SLOT(updateProgressBar(int)), Qt::UniqueConnection);
    connect(graphWidget_.getView(), SIGNAL(selectionChanged(FileDependency*)),
            &infoWidget_, SLOT(setEditedDependency(FileDependency*)), Qt::UniqueConnection);

    connect(&infoWidget_, SIGNAL(dependencyChanged(FileDependency*)),
            &model_, SIGNAL(dependencyChanged(FileDependency*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::~FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::~FileDependencyEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::openSourceDialog()
//-----------------------------------------------------------------------------
void FileDependencyEditor::openSourceDialog()
{
    // Show the source directories dialog.
    FileDependencySourceDialog dialog(basePath_, component_->getSourceDirectories(), this);

    if (dialog.exec() == QDialog::Accepted)
    {
        component_->setSourceDirectories(dialog.getSourceDirectories());
        scan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scan()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scan()
{
    if (scanning_)
    {
        model_.stopAnalysis();
        return;
    }

    emit scanStarted();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    runAnalysisAction_->setIcon(QIcon(":/icons/common/graphics/control-stop.png"));
    scanning_ = true;

    // Preparations. Resolve file types for each extension.
    resolveExtensionFileTypes();

    // Phase 1. Scan all files and folders in the source paths recursively.
    progWidget_ = new ScanProgressWidget(this);
    progWidget_->setRange(0, 1);
    progWidget_->setValue(1);
    progWidget_->setMessage(tr("Scanning source directories..."));
    
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(scanDirectories()));
    timer_->start();

    progWidget_->exec();

    // Phase 2. Run the dependency analysis.
    if (isEnabled())
    {
        progressBar_.setMaximum(model_.getTotalStepCount());
        model_.startAnalysis();
    }
    else
    {
        finishScan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::resolveExtensionFileSets()
//-----------------------------------------------------------------------------
void FileDependencyEditor::resolveExtensionFileTypes()
{
    fileTypeLookup_.clear();
    
    // Retrieve the file types information from the settings.
    QSettings settings;
    ignoreExtList_ = settings.value("FileTypes/IgnoredExtensions").toString().split(';');

    settings.beginGroup("FileTypes");

    foreach (QString const& fileType, settings.childGroups())
    {
        settings.beginGroup(fileType);

        QStringList fileTypeExtensions = settings.value("Extensions").toString().split(';');
        foreach (QString const& extension, fileTypeExtensions)
        {
            // Add to the lookup map only if the extension is not already in use.
            if (!fileTypeLookup_.contains(extension))
            {
                fileTypeLookup_.insert(extension, fileType);
            }
        }
        settings.endGroup();
    }

    settings.endGroup();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanFiles()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanFiles(QString const& path)
{
    FileDependencyItem* folderItem = model_.addFolder(path);

    QFileInfoList list = QDir(General::getAbsolutePath(basePath_ + "/", path)).entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo const& info, list)
    {
        // Check if the entry is a directory.
        if (info.isDir())
        {
            scanFiles(General::getRelativePath(basePath_, info.absoluteFilePath()));
        }
        // Otherwise add the file if it does not belong to ignored extensions or is not an IP-XACT file.
        else if (!ignoreExtList_.contains(info.completeSuffix()) && !isFileIPXact(info.absoluteFilePath()))
        {
            // Otherwise the entry is a file.
            // Check which file type corresponds to the extension.
            QString fileType = fileTypeLookup_.value(info.completeSuffix(), "unknown");

            // Check if the file is already packaged into the metadata.
            QString relativePath = General::getRelativePath(basePath_, info.absoluteFilePath());

            QList<QSharedPointer<File> > fileRefs;
            foreach (QSharedPointer<FileSet> fileSet, *component_->getFileSets())
            {
                foreach (QSharedPointer<File> file, *fileSet->getFiles())
                {
                    if (file->name() == relativePath)
                    {
                        fileRefs.append(file);
                    }
                }
            }

            // Add the file to the component file sets if not already found and is not yet ignored.
            if (fileRefs.empty() && !component_->getIgnoredFiles().contains(relativePath))
            {
                QSharedPointer<FileSet> fileSet;

                QString fileSetName = QString(path).replace(QRegularExpression("^(./)?../.*/"), "external_");
                if (fileSetName == ".")
                {
                    fileSetName = tr("base");
                }

                // Check if the file set does not exist in the component.
                if (component_->hasFileSet(fileSetName))
                {
                    fileSet = component_->getFileSet(fileSetName);
                }
                else
                {
                    fileSet.reset(new FileSet(fileSetName, ""));
                    component_->getFileSets()->append(fileSet);
                    emit fileSetAdded(fileSet.data());
                }

                QSharedPointer<File> file(new File(relativePath));
                file->addFileType(fileType);
                fileSet->addFile(file);
                emit fileAdded(file.data());

                fileRefs.append(file);
            }

            folderItem->addFile(component_, relativePath, fileType, fileRefs);
        }
    }
}


//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::isFileIPXact()
//-----------------------------------------------------------------------------
bool FileDependencyEditor::isFileIPXact(QString const& filename) const
{
    // Try to open the file for reading.
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }

    QXmlStreamReader reader(&file);

    if (!reader.readNextStartElement())
    {
        return false;
    }

    if (reader.hasError())
    {
        return false;
    }

    QString type = reader.qualifiedName().toString();
    file.close();

    // Check if the type is a valid IP-XACT/Kactus2 object type.
    return (VLNV::string2Type(type) != VLNV::INVALID);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::updateProgressBar()
//-----------------------------------------------------------------------------
void FileDependencyEditor::updateProgressBar(int value)
{
    progressBar_.setValue(value);

    if (value == 0)
    {
        finishScan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::filterToggle()
//-----------------------------------------------------------------------------
void FileDependencyEditor::filterToggle(QAction* action)
{
    // Get current filters from the graph.
    FileDependencyGraphView::DependencyFilters filters = graphWidget_.getView()->getFilters();

    // Toggle the appropriate filter based on the button that was clicked.
    filters ^= action->data().toUInt();

    // Apply the new filter setup to the view.
    graphWidget_.getView()->setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::addFilterButton()
//-----------------------------------------------------------------------------
void FileDependencyEditor::addFilterButton(QIcon icon, QString const& iconText,
    FileDependencyGraphView::DependencyFilter filter)
{
    FileDependencyGraphView::DependencyFilters filters = graphWidget_.getView()->getFilters();

    QAction* tmp = toolbar_.addAction(icon, iconText);
    tmp->setData(filter);
    tmp->setCheckable(true);
    tmp->setChecked(filters & filter);

    filterActions_.addAction(tmp);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanDirectories()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanDirectories()
{
    model_.beginReset();

    // First scan the source directories.
    if (isEnabled())
    {
        foreach (QString const& sourcePath, component_->getSourceDirectories())
        {
            scanFiles(sourcePath);
        }
    }

    // Then add files that are part of the file sets but were not added in the file scan.
    foreach (QSharedPointer<FileSet> fileSet, *component_->getFileSets())
    {
        foreach (QSharedPointer<File> file, *fileSet->getFiles())
        {
            // For non-url files, check if the model does not contain a corresponding file item.
            if (!QRegExp(Utils::URL_VALIDITY_REG_EXP).exactMatch(file->name()) &&
                model_.findFileItem(file->name()) == 0)
            {
                QFileInfo info(file->name());
                QString folderPath = info.path();

                // Create a folder item for the file if not already created.
                FileDependencyItem* folderItem = model_.findFolderItem(folderPath);

                if (folderItem == 0)
                {
                    folderItem = model_.addFolder(folderPath);
                }

                // Create a file item.
                QList<QSharedPointer<File> > fileRefs;
                foreach (QSharedPointer<FileSet> fileSet, *component_->getFileSets())
                {
                    foreach (QSharedPointer<File> filesetFile, *fileSet->getFiles())
                    {
                        if (filesetFile->name() == file->name())
                        {
                            fileRefs.append(filesetFile);
                        }
                    }
                }

                QString fileType = "";
                if (!file->getFileTypes()->empty())
                {
                    fileType = file->getFileTypes()->first();
                }

                folderItem->addFile(component_, file->name(), fileType, fileRefs);
            }
        }
    }

    model_.endReset();

    timer_->stop();
    delete timer_;
    timer_ = 0;
    delete progWidget_;
    progWidget_ = 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setCompact()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setCompact(bool compact)
{
    toolbar_.setVisible(!compact);
    infoWidget_.setVisible(!compact);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::refresh()
//-----------------------------------------------------------------------------
void FileDependencyEditor::refresh()
{
    model_.refresh();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setDependenciesEditable()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setDependenciesEditable(bool editable)
{
    graphWidget_.getView()->setDependenciesEditable(editable);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::isScanning()
//-----------------------------------------------------------------------------
bool FileDependencyEditor::isScanning() const
{
    return scanning_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::finishScan()
//-----------------------------------------------------------------------------
void FileDependencyEditor::finishScan()
{
    scanning_ = false;
    runAnalysisAction_->setIcon(QIcon(":/icons/common/graphics/control-play.png"));

    QApplication::restoreOverrideCursor();

    emit filesUpdated();
    emit scanCompleted();
}
