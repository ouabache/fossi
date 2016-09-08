//-----------------------------------------------------------------------------
// File: DesignWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 12.6.2012
//
// Description:
// Base class for all design widgets.
//-----------------------------------------------------------------------------

#include "DesignWidget.h"

#include <common/GenericEditProvider.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/StickyNote/StickyNote.h>
#include <designEditors/common/StickyNote/StickyNoteRemoveCommand.h>
#include <designEditors/common/Association/Association.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/Design.h>

#include <QScrollBar>
#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <QApplication>
#include <QGraphicsItem>


//-----------------------------------------------------------------------------
// Function: DesignWidget::DesignWidget()
//-----------------------------------------------------------------------------
DesignWidget::DesignWidget(LibraryInterface* lh, QWidget* parent)
    : TabDocument(parent, DOC_ZOOM_SUPPORT | DOC_DRAW_MODE_SUPPORT | DOC_PRINT_SUPPORT |
                          DOC_PROTECTION_SUPPORT | DOC_EDIT_SUPPORT | DOC_VISIBILITY_CONTROL_SUPPORT, 30, 300),
      library_(lh),
      view_(new QGraphicsView(this)),
      editedComponent_(),
      viewName_(),
      editProvider_(new GenericEditProvider(EDIT_HISTORY_SIZE)),
      diagram_(0)
{

    view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    view_->verticalScrollBar()->setTracking(true);
    connect(view_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScroll(int)));

	addVisibilityControl(tr("Sticky Notes"), true);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::~DesignWidget()
//-----------------------------------------------------------------------------
DesignWidget::~DesignWidget()
{
    view_->verticalScrollBar()->disconnect(this);

    delete diagram_;
    diagram_ = 0;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV DesignWidget::getIdentifyingVLNV() const 
{
    const QSharedPointer<DesignConfiguration> designConf = diagram_->getDesignConfiguration();

    if (designConf) {
        return designConf->getDesignRef();
    }
    else {
        return editedComponent_->getHierRef(viewName_);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setDiagram()
//-----------------------------------------------------------------------------
void DesignWidget::setDiagram(DesignDiagram* diagram)
{
    diagram_ = diagram;

    connect(diagram_, SIGNAL(openComponent(const VLNV&)),
        this, SIGNAL(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openSWDesign(const VLNV&, QString const&)),
        this, SIGNAL(openSWDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openBus(VLNV const&, VLNV const&, bool)),
        this, SIGNAL(openBus(VLNV const&, VLNV const&, bool)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(openCSource(ComponentItem*)),
        this, SIGNAL(openCSource(ComponentItem*)), Qt::UniqueConnection);

    connect(diagram_, SIGNAL(componentSelected(ComponentItem*)),
        this, SIGNAL(componentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
        this, SIGNAL(interfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(connectionSelected(GraphicsConnection*)),
        this, SIGNAL(connectionSelected(GraphicsConnection*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(clearItemSelection()),
        this, SIGNAL(clearItemSelection()), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(componentInstantiated(ComponentItem*)),
        this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(componentInstanceRemoved(ComponentItem*)),
        this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

    connect(diagram_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(diagram_, SIGNAL(helpUrlRequested(QString const&)),
        this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    view_->setScene(diagram_);
    centerViewTo(QPointF(0, 0));
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setZoomLevel()
//-----------------------------------------------------------------------------
void DesignWidget::setZoomLevel(int level)
{
    TabDocument::setZoomLevel(level);

    double newScale = getZoomLevel() / 100.0;
    QMatrix oldMatrix = view_->matrix();
    view_->resetMatrix();
    view_->translate(oldMatrix.dx(), oldMatrix.dy());
    view_->scale(newScale, newScale);

    emit zoomChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::fitInView()
//-----------------------------------------------------------------------------
void DesignWidget::fitInView()
{
    QRectF itemRect = diagram_->itemsBoundingRect();
    float scaleX = qMax(0, view_->width() - 10 - view_->verticalScrollBar()->width()) / itemRect.width();
    float scaleY = qMax(0, view_->height() - 10 - view_->horizontalScrollBar()->height()) / itemRect.height();

    int scaleLevel = int(qMin(scaleX, scaleY) * 10) * 10;
    setZoomLevel(scaleLevel);

    centerViewTo(itemRect.center());
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setMode()
//-----------------------------------------------------------------------------
void DesignWidget::setMode(DrawMode mode)
{
    diagram_->setMode(mode);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setProtection()
//-----------------------------------------------------------------------------
void DesignWidget::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    diagram_->setProtection(locked);
    diagram_->setMode(MODE_SELECT);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::refresh()
//-----------------------------------------------------------------------------
void DesignWidget::refresh()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QSharedPointer<Document> libComp = library_->getModel(editedComponent_->getVlnv());
    QSharedPointer<Component> comp = libComp.staticCast<Component>();

    setDesign(comp, viewName_);
    setModified(false);

    TabDocument::refresh();

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getOpenDocument()
//-----------------------------------------------------------------------------
VLNV DesignWidget::getOpenDocument() const
{
    if (editedComponent_ != 0)
    {
        return editedComponent_->getVlnv();
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV DesignWidget::getDocumentVLNV() const
{
    return editedComponent_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getEditProvider()
//-----------------------------------------------------------------------------
QSharedPointer<IEditProvider> DesignWidget::getEditProvider() const
{
    return editProvider_;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::save()
//-----------------------------------------------------------------------------
bool DesignWidget::save()
{
    getDiagram()->updateHierComponent();

    // Create the design.
    QSharedPointer<Design> design = getDiagram()->getDesign();
    QSharedPointer<DesignConfiguration> designConf = getDiagram()->getDesignConfiguration();

    /*if (designConf)
    {
        design = diagram_->createDesign(designConf->getDesignRef());
    }
    else
    {
        design = diagram_->createDesign(editedComponent_->getHierRef(viewName_));
    }*/

    if (design == 0)
    {
        return false;
    }

    library_->beginSave();

    bool writeSucceeded = true;

    // Write the files.
    if (designConf && !library_->writeModelToFile(designConf))
    {
        writeSucceeded = false;
    }

    if (!library_->writeModelToFile(design))
    {
        writeSucceeded = false;
    }

    if (!library_->writeModelToFile(editedComponent_))
    {
        writeSucceeded = false;
    }

    library_->endSave();

    if (writeSucceeded)
    {
        return TabDocument::save();
    }
    else
    {
        emit errorMessage(tr("Error saving design to disk."));
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::onVerticalScroll()
//-----------------------------------------------------------------------------
void DesignWidget::onVerticalScroll(int y)
{
    QPointF pt(0.0, y);
    QMatrix mat = view_->matrix().inverted();
    diagram_->onVerticalScroll(mat.map(pt).y());
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setDesign()
//-----------------------------------------------------------------------------
bool DesignWidget::setDesign(QSharedPointer<Component> component, const QString& viewName)
{
    editedComponent_ = component;
    viewName_ = viewName;
    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::removeSelectedNotes()
//-----------------------------------------------------------------------------
void DesignWidget::removeSelectedNotes()
{
    QList<QGraphicsItem*> selectedItems = getDiagram()->selectedItems();
    getDiagram()->clearSelection();

    QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());
    foreach (QGraphicsItem* selected, selectedItems)
    {
        StickyNote* note = qgraphicsitem_cast<StickyNote*>(selected);
        StickyNoteRemoveCommand* noteRemoveCommand = new StickyNoteRemoveCommand(note, getDiagram(), 
            parentCommand.data());

        connect(noteRemoveCommand, SIGNAL(addVendorExtension(QSharedPointer<VendorExtension>)),
            diagram_, SLOT(onVendorExtensionAdded(QSharedPointer<VendorExtension>)), Qt::UniqueConnection);

        connect(noteRemoveCommand, SIGNAL(removeVendorExtension(QSharedPointer<VendorExtension>)),
            diagram_, SLOT(onVendorExtensionRemoved(QSharedPointer<VendorExtension>)), Qt::UniqueConnection);

        noteRemoveCommand->redo();
    }

    getEditProvider()->addCommand(parentCommand);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::removeSelectedAssociations()
//-----------------------------------------------------------------------------
void DesignWidget::removeSelectedAssociations()
{
    QList<QGraphicsItem*> selectedItems = getDiagram()->selectedItems();
    getDiagram()->clearSelection();

    QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());
    foreach (QGraphicsItem* selected, selectedItems)
    {
        Association* association = qgraphicsitem_cast<Association*>(selected);
        QUndoCommand* associationRemoveCmd = new AssociationRemoveCommand(association, getDiagram(), 
            parentCommand.data()); 
        associationRemoveCmd->redo();
    }

    getEditProvider()->addCommand(parentCommand);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getDiagram()
//-----------------------------------------------------------------------------
DesignDiagram* DesignWidget::getDiagram() const
{
    return diagram_;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::centerViewTo()
//-----------------------------------------------------------------------------
void DesignWidget::centerViewTo(QPointF const& centerPoint)
{
    view_->centerOn(centerPoint);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* DesignWidget::getLibraryInterface()
{
    return library_;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getOpenViewName()
//-----------------------------------------------------------------------------
QString DesignWidget::getOpenViewName() const
{
    return viewName_;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getEditedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DesignWidget::getEditedComponent() const
{
    return editedComponent_;
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::print()
//-----------------------------------------------------------------------------
void DesignWidget::print()
{
    // get the rect that bounds all items on box
    QRectF boundingRect = diagram_->itemsBoundingRect();

    // avoid clipping of the edges of the objects by increasing the rect a little
    boundingRect.setHeight(boundingRect.height() + 2);
    boundingRect.setWidth(boundingRect.width() + 2);

    // set the size of the picture
    QPixmap pic(boundingRect.size().toSize());

    // render the picture for the design
    QPainter picPainter(&pic);
    picPainter.fillRect(pic.rect(), QBrush(Qt::white));
    diagram_->render(&picPainter, pic.rect(), boundingRect.toRect());

    // create the printer instance, set default settings for it and
    // execute print dialog for user to select the printer and desired options
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPaperSize(QPrinter::A4);
    QPrintDialog dialog(&printer, this);

    if (dialog.exec() == QPrintDialog::Accepted)
    {
        // create painter instance to draw the picture for the printer
        QPainter painter(&printer);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
            QPainter::SmoothPixmapTransform, true);

        // scale the picture to match the size of the printer page
        QPixmap scaledPixmap = pic.scaled(printer.pageRect().size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // draw the picture on the printer
        painter.drawPixmap(printer.pageRect().topLeft(), scaledPixmap);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setVisibilityControlState()
//-----------------------------------------------------------------------------
void DesignWidget::setVisibilityControlState(QString const& name, bool state)
{
    TabDocument::setVisibilityControlState(name, state);
    diagram_->setVisibilityControlState(name, state);
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::getInstances()
//-----------------------------------------------------------------------------
QList<ComponentItem*> DesignWidget::getInstances() const
{
    return diagram_->getInstances();
}

//-----------------------------------------------------------------------------
// Function: showEvent()
//-----------------------------------------------------------------------------
void DesignWidget::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    diagram_->onShow();
}

//-----------------------------------------------------------------------------
// Function: DesignWidget::setEditedComponent()
//-----------------------------------------------------------------------------
void DesignWidget::setEditedComponent(QSharedPointer<Component> component)
{
    editedComponent_ = component;
}
