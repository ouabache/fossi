//-----------------------------------------------------------------------------
// File: tst_documentGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.02.2015
//
// Description:
// Unit test for class documentGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>

#include <tests/MockObjects/LibraryMock.h>
#include <tests/MockObjects/DesignWidgetFactoryMock.h>
#include <tests/MockObjects/HWDesignWidgetMock.h>

#include <editors/ComponentEditor/common/ExpressionFormatterFactoryImplementation.h>

#include <common/utils.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Fileset.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Memorymap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QWidget>

class tst_documentGenerator : public QObject
{
    Q_OBJECT

public:
    tst_documentGenerator();

private slots:
    
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    //! Test cases.

    void testInvalidVlnvInConstructor();

    void testFileHeaderIsWritten();
    void testTableOfContentsIsWrittenWithOnlyTopComponent();

    void testParametersWrittenWithOnlyTopComponent();

    void testMemoryMapsWrittenWithTopComponent();
    void testAddressBlocksWrittenWithTopComponent();
    void testExpressionsInAddressBlocks();
    void testRegistersWrittenWithTopComponent();
    void testFieldsWrittenWithTopComponent();
    void testMemoryMapToFieldWrittenWithTopComponent();

    void testPortsWrittenWithOnlyTopComponent();

    void testBusInterfacesWrittenWithoutPorts();

    void testFileSetsWrittenForTopComponent();

    void testViewsWrittenForTopComponent();

    void testDesignIsWritten();

    void testEndOfDocumentWrittenForTopComponent();

private:

    /*!
     *  Create the generator used in most test cases.
     */
    DocumentGenerator* createTestGenerator();

    /*!
     *  Read the output file of the generator.
     */
    void readOutputFile();

    /*!
     *  Create a model parameter used in the tests.
     *
     *      @param [in] name            Name of the parameter.
     *      @param [in] dataType        Data type of the parameter.
     *      @param [in] value           Value of the parameter.
     *      @param [in] description     Description of the parameter.
     *      @param [in] uuID            Id of the parameter.
     */
   /* QSharedPointer<ModelParameter> createTestModelParameter(QString const& name, QString const& dataType,
        QString const& value, QString const& description, QString const& uuID, QString const& arrayLeft,
        QString const& arrayRight);*/

    /*!
     *  Create a parameter used in the tests.
     *
     *      @param [in] name            Name of the parameter.
     *      @param [in] value           Value of the parameter.
     *      @param [in] description     Description of the parameter.
     *      @param [in] uuID            Id of the parameter.
     */
    QSharedPointer<Parameter> createTestParameter(QString const& name, QString const& value,
        QString const& description, QString const& uuID, QString const& arrayLeft, QString const& arrayRight);

    /*!
     *  Create a port used in the tests.
     *
     *      @param [in] name            Name of the port.
     *      @param [in] leftBound       The left bound of the port.
     *      @param [in] rightBound      The right bound of the port.
     *      @param [in] defaultValue    The default value of the port.
     */
    QSharedPointer<Port> createTestPort(QString const& name, QString const& leftBound, QString const& rightBound,
        QString const& defaultValue, QString const& arrayLeft, QString const& arrayRight);

    /*!
     *  Create a map for configurable element values.
     *
     *      @param [in] component   The component, whose configurable element values are being created.
     */
    QList<QSharedPointer<ConfigurableElementValue> > createConfigurableElementvalues(QSharedPointer <Component> component);

    QSharedPointer<Field> createTestField (QString const& name, QString const& description, QString const& offset,
        QString const& width);

    QSharedPointer<Register> createTestRegister (QString const& name, QString const& offset, QString const& size,
        QString const& dimension, QString const& description);

    QSharedPointer<AddressBlock> createTestAddressBlock (QString const& name, QString const& description,
        QString const& baseAddress, QString const& range, QString const& width,
        QList <QSharedPointer <Register> > registers);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& name, QString const& description, 
        int addressUnitbits, QList<QSharedPointer <AddressBlock> > addressBlocks);

    //! Get the string used to describe a space.
    QString getSpaceString();

    //! Get the string used to describe an indent.
    QString getIndentString();

    //! Get the string used to describe a table.
    QString getTableString();

    //! Get the string used to describe the encoding.
    QString getEncodingString();

    //! Get the string used to describe the document type.
    QString getDoctypeString();

    //! Get the string used to describe valid w3c strict.
    QString getValidW3CStrictString();

    QSharedPointer<Component> topComponent_;

    VLNV topComponentVlnv_;

    QString output_;

    QDateTime generationTime_;

    LibraryMock library_;

    DesignWidgetFactoryMock designWidgetFactory_;

    ExpressionFormatterFactoryImplementation expressionFormatterFactory_;

    QString targetPath_;

    QWidget* generatorParentWidget_;
};

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::tst_documentGenerator()
//-----------------------------------------------------------------------------
tst_documentGenerator::tst_documentGenerator() :
topComponent_(), topComponentVlnv_(), output_(), generationTime_(), library_(this), designWidgetFactory_(&library_),
    expressionFormatterFactory_(), targetPath_("./generatorOutput.html"), generatorParentWidget_(new QWidget)
{

}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_documentGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_documentGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::init()
//-----------------------------------------------------------------------------
void tst_documentGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponentVlnv_ = vlnv;
    topComponent_ = QSharedPointer<Component>(new Component(topComponentVlnv_));

    library_.clear();

    QVERIFY(!QFile::exists(targetPath_));
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_documentGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    QFile::remove(targetPath_);
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testInvalidVlnvInConstructor()
{
    VLNV invalidVlnv(VLNV::COMPONENT, "invalid", "library", "component", "0");

    DocumentGenerator* generator (new DocumentGenerator(&library_, invalidVlnv, &designWidgetFactory_,
        &expressionFormatterFactory_, generatorParentWidget_));

    QSignalSpy spy(generator, SIGNAL(errorMessage(QString const&)));

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeDocumentation(stream, targetPath_);

    targetFile.close();

    delete generator;
    generator = 0;

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toString(), QString("VLNV was not found in the library."));
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testFileHeaderIsWritten()
{
    QCoreApplication::setOrganizationName("TUT");
    QCoreApplication::setApplicationName("TestRunner");
    QSettings settings;
    settings.setValue("General/Username", "testUser");

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);
    
    generationTime_ = QDateTime::currentDateTime();

    generator->writeHtmlHeader(stream);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(getDoctypeString() + "\n"
    "<html>\n"
        "\t<head>\n"
        "\t" + getEncodingString() + "\n"
            "\t\t<title>Kactus2 generated documentation for component " + topComponent_->getVlnv().getName() +
            " " + topComponent_->getVlnv().getVersion() + "</title>\n"
        "\t</head>\n"
        "\t<body>\n"
            "\t\t<h6>This document was generated by Kactus2 on " +
            generationTime_.toString("dd.MM.yyyy hh:mm:ss") + " by user testUser</h6>\n");

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testTableOfContentsIsWrittenWithOnlyTopComponent()
{
    QList <QSharedPointer<Parameter> > componentParameters;
    QSharedPointer<Parameter> parameter = createTestParameter("parameter", "1", "", "P-ID", "", "");
    componentParameters.append(parameter);

    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "1", "1", "1", "");
    QSharedPointer <Field> testField = createTestField("field", "", "1", "8");
    testRegister->getFields()->append(testField);
    registers.append(testRegister);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "", "'h0", "4", "32",
        registers);
    addressBlocks.append(testAddressBlock);

    QList <QSharedPointer <MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> testMemoryMap = createTestMemoryMap("memoryMap", "", 8, addressBlocks);
    memoryMaps.append(testMemoryMap);

    QSharedPointer<Port> port = createTestPort("port", "10", "1", "", "", "");

    QSharedPointer<BusInterface> busInterface (new BusInterface());
    busInterface->setName("busInterface");

    QSharedPointer<FileSet> fileset (new FileSet("fileSet"));

    QSharedPointer<View> view (new View("view"));

    topComponent_->getParameters()->append(componentParameters);
    topComponent_->getPorts()->append(port);
    topComponent_->getBusInterfaces()->append(busInterface);
    topComponent_->getFileSets()->append(fileset);
    topComponent_->getViews()->append(view);
    topComponent_->getMemoryMaps()->append(memoryMaps);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    unsigned int runningNumber = 0;

    generator->writeTableOfContents(runningNumber, stream);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<a href=\"#" + topComponent_->getVlnv().toString() + "\">1. Component" + getSpaceString() +
        topComponent_->getVlnv().getVendor() + " - " + topComponent_->getVlnv().getLibrary() + " - " +
        topComponent_->getVlnv().getName() + " - " + topComponent_->getVlnv().getVersion() + "</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".kts_params\">1.1. Kactus2 attributes</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".parameters\">1.2. General parameters</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".memoryMaps\">1.3. Memory maps</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".ports\">1.4. Ports</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".interfaces\">1.5. Bus interfaces</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".fileSets\">1.6. File sets</a><br>\n"
        "\t\t" + getIndentString() + "<a href=\"#" + topComponent_->getVlnv().toString() +
        ".views\">1.7. Views</a><br>\n"
            );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testParametersWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testParametersWrittenWithOnlyTopComponent()
{
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID-parameter",
        "1", "0");
    QSharedPointer <Parameter> refParameter = createTestParameter("refParameter", "ID-parameter", "Describe this",
        "ID-refer", "", "");

    QList <QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(parameter);
    componentParameters.append(refParameter);

    topComponent_->getParameters()->append(componentParameters);
    topComponent_->setHierarchy(KactusAttribute::FLAT);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeParameters(stream, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".kts_params\">0.1 Kactus2 attributes</a></h2>\n"
        "\t\t<p>\n"
        "\t\t\t<strong>" + getIndentString() + "Product hierarchy: </strong>" +
        KactusAttribute::hierarchyToString(topComponent_->getHierarchy()) + "<br>\n"
        "\t\t\t<strong>" + getIndentString() + "Component implementation: </strong>" +
        KactusAttribute::implementationToString(topComponent_->getImplementation()) + "<br>\n"
        "\t\t\t<strong>" + getIndentString() + "Component firmness: </strong>" +
        KactusAttribute::firmnessToString(topComponent_->getFirmness()) + "<br>\n"
        "\t\t</p>\n"
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".parameters\">0.2 General parameters</a></h2>\n"
        "\t\t\t" + getTableString() + "List of parameters defined for the component\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Value</th>\n"
        "\t\t\t\t\t<th>Array left</th>\n"
        "\t\t\t\t\t<th>Array right</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + parameter->name() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getValue() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getArrayLeft() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->getArrayRight() + "</td>\n"
        "\t\t\t\t\t<td>" + parameter->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + refParameter->name() + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td>" + refParameter->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testMemoryMapsWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testMemoryMapsWrittenWithTopComponent()
{
    QList <QSharedPointer <AddressBlock> > addressBlocks;

    QSharedPointer<MemoryMap> memoryMap = createTestMemoryMap("memoryMap", "example Description", 8, addressBlocks);

    topComponent_->getMemoryMaps()->append(memoryMap);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeMemoryMaps(stream, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".memoryMaps\">0.1 Memory maps</a></h2>\n"
        "\t\t\t<h3><a id=\""+ topComponent_->getVlnv().toString() + ".memoryMap." +
        memoryMap->name() + "\">0.1.1 " + memoryMap->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + memoryMap->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Address unit bits (AUB):</strong> " + 
        memoryMap->getAddressUnitBits() + "<br>\n"
        "\t\t\t</p>"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testAddressBlocksWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testAddressBlocksWrittenWithTopComponent()
{
    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "4", "2", "2", "");
    registers.append(testRegister);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "example", "'h0", "4",
        "32", registers);
    addressBlocks.append(testAddressBlock);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeAddressBlocks(addressBlocks, stream, subHeaderNumber, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;
    
    QString expectedOutput(
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".addressBlock." +
        testAddressBlock->name() + "\">0.1.1.1 " + testAddressBlock->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() +
        "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "List of values in " + testAddressBlock->name() + ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Usage</th>\n"
        "\t\t\t\t\t<th>Base address [AUB]</th>\n"
        "\t\t\t\t\t<th>Range [AUB]</th>\n"
        "\t\t\t\t\t<th>Width [AUB]</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + General::usage2Str(testAddressBlock->getUsage()) + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getBaseAddress() + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getRange() + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getWidth() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testAddressBlock->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getVolatile() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testExpressionsInAddressBlocks()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testExpressionsInAddressBlocks()
{
    QSharedPointer <Parameter> targetParameter = createTestParameter("target", "4", "", "ID-TARGET", "", "");

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("addressBlock", "example", "'h0",
        "ID-TARGET", "ID-TARGET + 2", QList <QSharedPointer <Register> > ());
    addressBlocks.append(testAddressBlock);

    topComponent_->getParameters()->append(targetParameter);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeAddressBlocks(addressBlocks, stream, subHeaderNumber, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".addressBlock." +
        testAddressBlock->name() + "\">0.1.1.1 " + testAddressBlock->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() +
        "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "List of values in " + testAddressBlock->name() + ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Usage</th>\n"
        "\t\t\t\t\t<th>Base address [AUB]</th>\n"
        "\t\t\t\t\t<th>Range [AUB]</th>\n"
        "\t\t\t\t\t<th>Width [AUB]</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + General::usage2Str(testAddressBlock->getUsage()) + "</td>\n"
        "\t\t\t\t\t<td>" + "'h0" + "</td>\n"
        "\t\t\t\t\t<td>" + "target" + "</td>\n"
        "\t\t\t\t\t<td>" + "target + 2" + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testAddressBlock->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getVolatile() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testRegistersWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testRegistersWrittenWithTopComponent()
{
    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> testRegister = createTestRegister("register", "4", "2", "2", "exampleDescription");
    registers.append(testRegister);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeRegisters(registers, stream, subHeaderNumber, subHeaderNumber, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".register." + testRegister->name() +
        "\">0.1.1.1.1 " + testRegister->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testRegister->description() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "List of values in " + testRegister->name() + ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Offset [AUB]</th>\n"
        "\t\t\t\t\t<th>Size [bits]</th>\n"
        "\t\t\t\t\t<th>Dimension</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Reset value</th>\n"
        "\t\t\t\t\t<th>Reset mask</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + testRegister->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testRegister->getAccess()) + "</td>\n"
        //"\t\t\t\t\t<td>" + testRegister->getRegisterValue() + "</td>\n"
        //"\t\t\t\t\t<td>" + testRegister->getRegisterMask() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testFieldsWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testFieldsWrittenWithTopComponent()
{
    QSharedPointer<Field> testField = createTestField("testField", "Example Field", "2", "8");

    QSharedPointer<Register> fieldRegister =createTestRegister("FieldRegister", "10", "10", "10", "");
    fieldRegister->getFields()->append(testField);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeFields(fieldRegister, stream);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t\t<h4>Register " + fieldRegister->name() + " contains the following fields:</h4>\n"
        "\t\t\t" + getTableString() + "List of fields contained within register " + fieldRegister->name() +
        ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Field name</th>\n"
        "\t\t\t\t\t<th>Offset [bits]</th>\n"
        "\t\t\t\t\t<th>Width [bits]</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv().toString() + ".field." + testField->name() +
        "\">" + testField->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + testField->getBitOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getBitWidth() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getVolatile().toString() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testField->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>" + testField->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testMemoryMapToFieldWrittenWithTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testMemoryMapToFieldWrittenWithTopComponent()
{
    QSharedPointer<Field> testField = createTestField("testField", "", "2", "8");

    QList <QSharedPointer <Register> > registers;
    QSharedPointer<Register> testRegister = createTestRegister("testRegister", "10", "4", "2", "Describing reg.");
    testRegister->getFields()->append(testField);
    registers.append(testRegister);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> testAddressBlock = createTestAddressBlock("testAddress", "Describe.", "'h0", "4",
        "32", registers);
    addressBlocks.append(testAddressBlock);

    QList <QSharedPointer <MemoryMap> > memoryMaps;
    QSharedPointer <MemoryMap> testMemoryMap = createTestMemoryMap("testMemoryMap", "", 8, addressBlocks);
    memoryMaps.append(testMemoryMap);

    topComponent_->getMemoryMaps()->append(memoryMaps);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeMemoryMaps(stream, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".memoryMaps\">0.1 Memory maps</a></h2>\n"
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".memoryMap." + testMemoryMap->name() +
        "\">0.1.1 " + testMemoryMap->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Address unit bits (AUB):</strong> " +
        testMemoryMap->getAddressUnitBits() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".addressBlock." +
        testAddressBlock->name() + "\">0.1.1.1 " + testAddressBlock->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testAddressBlock->description() +
        "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "List of values in " + testAddressBlock->name() + ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Usage</th>\n"
        "\t\t\t\t\t<th>Base address [AUB]</th>\n"
        "\t\t\t\t\t<th>Range [AUB]</th>\n"
        "\t\t\t\t\t<th>Width [AUB]</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + General::usage2Str(testAddressBlock->getUsage()) + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getBaseAddress() + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getRange() + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getWidth() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testAddressBlock->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>" + testAddressBlock->getVolatile() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".register." + testRegister->name() +
        "\">0.1.1.1.1 " + testRegister->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testRegister->description() + "<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "List of values in " + testRegister->name() + ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Offset [AUB]</th>\n"
        "\t\t\t\t\t<th>Size [bits]</th>\n"
        "\t\t\t\t\t<th>Dimension</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Reset value</th>\n"
        "\t\t\t\t\t<th>Reset mask</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + testRegister->getAddressOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getSize() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getDimension() + "</td>\n"
        "\t\t\t\t\t<td>" + testRegister->getVolatile() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testRegister->getAccess()) + "</td>\n"
        //"\t\t\t\t\t<td>" + testRegister->getRegisterValue() + "</td>\n"
        //"\t\t\t\t\t<td>" + testRegister->getRegisterMask() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        "\t\t\t<h4>Register " + testRegister->name() + " contains the following fields:</h4>\n"
        "\t\t\t" + getTableString() + "List of fields contained within register " + testRegister->name() +
        ".\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Field name</th>\n"
        "\t\t\t\t\t<th>Offset [bits]</th>\n"
        "\t\t\t\t\t<th>Width [bits]</th>\n"
        "\t\t\t\t\t<th>Volatile</th>\n"
        "\t\t\t\t\t<th>Access</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv().toString() + ".field." +
        testField->name() + "\">" + testField->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + testField->getBitOffset() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getBitWidth() + "</td>\n"
        "\t\t\t\t\t<td>" + testField->getVolatile().toString() + "</td>\n"
        "\t\t\t\t\t<td>" + AccessTypes::access2Str(testField->getAccess()) + "</td>\n"
        "\t\t\t\t\t<td>" + testField->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testPortsWrittenWithOnlyTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testPortsWrittenWithOnlyTopComponent()
{
    QList <QSharedPointer <Parameter> > componentParameters;
    QSharedPointer <Parameter> parameter = createTestParameter("parameter", "10", "Description", "ID-parameter",
        "", "");
    componentParameters.append(parameter);

    QSharedPointer <Port> portRef = createTestPort("portRef", "ID-parameter", "4", "ID-parameter", "2",
        "ID-parameter");

    topComponent_->getPorts()->append(portRef);

    topComponent_->getParameters()->append(componentParameters);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    
    generator->writePorts(stream, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".ports\">0.1 Ports</a></h2>\n"
        "\t\t\t" + getTableString() + "List of all ports the component has.\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Name</th>\n"
        "\t\t\t\t\t<th>Direction</th>\n"
        "\t\t\t\t\t<th>Left bound</th>\n"
        "\t\t\t\t\t<th>Right bound</th>\n"
        "\t\t\t\t\t<th>Port type</th>\n"
        "\t\t\t\t\t<th>Type definition</th>\n"
        "\t\t\t\t\t<th>Default value</th>\n"
        "\t\t\t\t\t<th>Array left</th>\n"
        "\t\t\t\t\t<th>Array right</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td><a id=\"" + topComponent_->getVlnv().toString() + ".port." + portRef->name() + "\">" +
        portRef->name() + "</a></td>\n"
        "\t\t\t\t\t<td>" + DirectionTypes::direction2Str(portRef->getDirection()) + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->getRightBound() + "</td>\n"
        "\t\t\t\t\t<td>" + portRef->getTypeName() + "</td>\n"
        "\t\t\t\t\t<td>" + portRef->getTypeDefinition(portRef->getTypeName()) + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->getArrayLeft() + "</td>\n"
        "\t\t\t\t\t<td>parameter</td>\n"
        "\t\t\t\t\t<td>" + portRef->description() + "</td>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t</table>\n");

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testBusInterfacesWrittenWithoutPorts()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testBusInterfacesWrittenWithoutPorts()
{
    QSharedPointer <BusInterface> busInterface (new BusInterface);
    busInterface->setName("interface");
    busInterface->setInterfaceMode(General::MASTER);

    topComponent_->getBusInterfaces()->append(busInterface);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeInterfaces(stream, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".interfaces\">0.1 Bus interfaces</a></h2>\n"
        "\t\t\t<h3>0.1.1 " + busInterface->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Interface mode:</strong> " +
        General::interfaceMode2Str(busInterface->getInterfaceMode()) + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Ports used in this interface:</strong>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testFileSetsWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testFileSetsWrittenForTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    QSharedPointer<FileSet> testFileSet (new FileSet);
    testFileSet->setName("testFileSet");
    testFileSet->setDescription("example description");
    testFileSet->setGroups("documentation");

    topComponent_->getFileSets()->append(testFileSet);

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeFileSets(stream, subHeaderNumber);

    targetFile.close();

    delete generator;
    generator = 0;

    QString groups = testFileSet->getGroups()->join(", ");

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".fileSets\">0.1 File sets</a></h2>\n"
        "\t\t\t<h3><a id=\"" + topComponent_->getVlnv().toString() + ".fileSet." + testFileSet->name() +
        "\">0.1.1 " + testFileSet->name() + "</a></h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t" + getIndentString() + "<strong>Description:</strong> " + testFileSet->description() + "<br>\n"
        "\t\t\t" + getIndentString() + "<strong>Identifiers:</strong> " + groups + "<br>\n"
        "\t\t\t</p>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testViewsWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testViewsWrittenForTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    QSharedPointer<View> flatView(new View());

    topComponent_->getViews()->append(flatView);

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;
    QStringList pictureList;

    generator->writeViews(stream, subHeaderNumber, pictureList);

    targetFile.close();

    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t<h2><a id=\"" + topComponent_->getVlnv().toString() + ".views\">0.1 Views</a></h2>\n"
        "\t\t\t<h3>0.1.1 View: " + flatView->name() + "</h3>\n"
        "\t\t\t<p>\n"
        "\t\t\t<strong>" + getIndentString() + "Type: </strong>non-hierarchical<br>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testHierarchicalDesignIsWritten()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testDesignIsWritten()
{
    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    QSharedPointer<Design> design = QSharedPointer<Design>(new Design(designVlnv));
    library_.writeModelToFile("C:/Test/TestLibrary/TestDesign/1.0/TestDesign.1.0.xml", design);
    library_.addComponent(design);

    VLNV firstVlnv (VLNV::COMPONENT, "Test", "TestLibrary", "FirstComponent", "1.0");
    QSharedPointer<Component> refComponent = QSharedPointer<Component>(new Component(firstVlnv));

    QList <QSharedPointer<Parameter> > componentParameters;

    QSharedPointer<Parameter> targetParameter = createTestParameter("firstParameter", "10", "", "ID-TARGET", "",
        "");
    QSharedPointer<Parameter> referParameter = createTestParameter("referer", "ID-TARGET", "", "ID-REF", "", "");
    componentParameters.append(targetParameter);
    componentParameters.append(referParameter);

    refComponent->getParameters()->append(componentParameters);

    library_.addComponent(refComponent);

    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(new ConfigurableVLNVReference(firstVlnv));
    QSharedPointer<ComponentInstance> firstInstance(new ComponentInstance("firstInstance", instanceVLNV));
    firstInstance->getConfigurableElementValues()->append(createConfigurableElementvalues(refComponent));
    
    design->getComponentInstances()->append(firstInstance);

    QSharedPointer<View> hierarchicalView(new View);
    hierarchicalView->setName("HierarchicalView");
    hierarchicalView->setDesignInstantiationRef("design_instantiation");
    topComponent_->getViews()->append(hierarchicalView);

    QSharedPointer<DesignInstantiation> designInstantiation(new DesignInstantiation("design_instantiation"));
    designInstantiation->setDesignReference(
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(designVlnv)));

    topComponent_->getDesignInstantiations()->append(designInstantiation);

    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    int subHeaderNumber = 1;

    generator->writeView(hierarchicalView, stream, subHeaderNumber, subHeaderNumber);

    targetFile.close();
    delete generator;
    generator = 0;

    QString expectedOutput(
        "\t\t\t<h4>0.1.1.2 Design " + design->getVlnv().toString(" - ") + "</h4>\n"
        "\t\t\t<p>\n"
        "\t\t\t<strong>" + getIndentString() + "IP-Xact file: </strong><a href=\"\">TestDesign.1.0.xml</a><br>\n"
        "\t\t\t<br>\n"
        "\t\t\t</p>\n"
        "\t\t\t" + getTableString() + "Component instantiations within this design\">\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<th>Instance name</th>\n"
        "\t\t\t\t\t<th>Component type</th>\n"
        "\t\t\t\t\t<th>Configurable values</th>\n"
        "\t\t\t\t\t<th>Active view</th>\n"
        "\t\t\t\t\t<th>Description</th>\n"
        "\t\t\t\t</tr>\n"
        "\t\t\t\t<tr>\n"
        "\t\t\t\t\t<td>" + firstInstance->getInstanceName() + "</td>\n"
        "\t\t\t\t\t<td><a href=\"#" + firstVlnv.toString(":") + "\">" + firstVlnv.toString(" - ") + "</a></td>\n"
        "\t\t\t\t\t<td>\n"
        "\t\t\t\t\t" + targetParameter->name() + " = " + targetParameter->getValue() + "<br>\n"
        "\t\t\t\t\t" + referParameter->name() + " = " + targetParameter->name() + "\n"
        "\t\t\t\t\t</td>\n"
        "\t\t\t\t\t<td></td>\n"
        "\t\t\t\t\t<td>" + firstInstance->getDescription() + "</td>\n"
        "\t\t\t\t</tr>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::testEndOfDocumentWrittenForTopComponent()
//-----------------------------------------------------------------------------
void tst_documentGenerator::testEndOfDocumentWrittenForTopComponent()
{
    DocumentGenerator* generator = createTestGenerator();

    QFile targetFile(targetPath_);
    targetFile.open(QFile::WriteOnly);
    QTextStream stream(&targetFile);

    generator->writeEndOfDocument(stream);

    targetFile.close();

    QString expectedOutput(
        getValidW3CStrictString() +
        "\t</body>\n"
        "</html>\n"
        );

    readOutputFile();

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            readOutputFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator* tst_documentGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    DocumentGenerator* generator (new DocumentGenerator(&library_, topComponentVlnv_, &designWidgetFactory_, 
        &expressionFormatterFactory_, generatorParentWidget_));

    return generator;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_documentGenerator::readOutputFile()
{
    QFile outputFile(targetPath_);

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestModelParameter()
//-----------------------------------------------------------------------------
/*QSharedPointer<ModelParameter> tst_documentGenerator::createTestModelParameter(QString const& name,
    QString const& dataType, QString const& value, QString const& description, QString const& uuID,
    QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<ModelParameter> modelParameter (new ModelParameter);
    modelParameter->setName(name);
    modelParameter->setDataType(dataType);
    modelParameter->setValue(value);
    modelParameter->setDescription(description);
    modelParameter->setValueId(uuID);
    modelParameter->setArrayLeft(arrayLeft);
    modelParameter->setArrayRight(arrayRight);

    return modelParameter;
}*/

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_documentGenerator::createTestParameter(QString const& name, QString const& value,
    QString const& description, QString const& uuID, QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<Parameter> parameter (new Parameter);
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setDescription(description);
    parameter->setValueId(uuID);
    parameter->setArrayLeft(arrayLeft);
    parameter->setArrayRight(arrayRight);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_documentGenerator::createTestPort(QString const& name, QString const& leftBound,
    QString const& rightBound, QString const& defaultValue, QString const& arrayLeft, QString const& arrayRight)
{
    QSharedPointer<Port> newPort (new Port);
    newPort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
    newPort->setName(name);
    newPort->setLeftBound(leftBound);
    newPort->setRightBound(rightBound);
    newPort->setDefaultValue(defaultValue);
    newPort->setArrayLeft(arrayLeft);
    newPort->setArrayRight(arrayRight);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createConfigurableElementvalues()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ConfigurableElementValue> > tst_documentGenerator::createConfigurableElementvalues(QSharedPointer <Component> component)
{    
    ExpressionFormatter* refExpressionFormatter = expressionFormatterFactory_.makeExpressionFormatter(component);

    QList<QSharedPointer<ConfigurableElementValue> > instanceConfigurableElementValues;

    foreach (QSharedPointer<Parameter> parameter, *component->getParameters())
    {
        instanceConfigurableElementValues.append(QSharedPointer<ConfigurableElementValue>(
            new ConfigurableElementValue(refExpressionFormatter->formatReferringExpression(parameter->getValue()), parameter->name())));
    }

    delete refExpressionFormatter;
    refExpressionFormatter = 0;

    return instanceConfigurableElementValues;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestField()
//-----------------------------------------------------------------------------
QSharedPointer <Field> tst_documentGenerator::createTestField(QString const& name, QString const& description,
    QString const& offset, QString const& width)
{
    QSharedPointer <Field> testField (new Field);
    testField->setName(name);
    testField->setDescription(description);
    testField->setBitOffset(offset);
    testField->setBitWidth(width);
    testField->setVolatile(false);
    testField->setAccess(AccessTypes::READ_ONLY);

    return testField;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestRegister()
//-----------------------------------------------------------------------------
QSharedPointer <Register> tst_documentGenerator::createTestRegister(QString const& name, QString const& offset,
    QString const& dimension, QString const& size, QString const& description)
{
    QSharedPointer<Register> testRegister (new Register);
    testRegister->setName(name);
    testRegister->setAddressOffset(offset);
    testRegister->setSize(size);
    testRegister->setDimension(dimension);
    testRegister->setDescription(description);
    testRegister->setVolatile(true);
    testRegister->setAccess(AccessTypes::READ_WRITE);
    //testRegister->setRegisterValue("2");
    //testRegister->setRegisterMask("4");

    return testRegister;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer <AddressBlock> tst_documentGenerator::createTestAddressBlock(QString const& name,
    QString const& description, QString const& baseAddress, QString const& range, QString const& width,
    QList<QSharedPointer<Register> > registers)
{
    QSharedPointer <AddressBlock> testAddressBlock(new AddressBlock);
    testAddressBlock->setName(name);
    testAddressBlock->setUsage(General::REGISTER);
    testAddressBlock->setDescription(description);
    testAddressBlock->setBaseAddress(baseAddress);
    testAddressBlock->setRange(range);
    testAddressBlock->setWidth(width);
    testAddressBlock->setAccess(AccessTypes::READ_WRITE);
    testAddressBlock->setVolatile(true);

    foreach (QSharedPointer<RegisterBase> testRegister, registers)
    {
        testAddressBlock->getRegisterData()->append(testRegister);
    }

    return testAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::createTestMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer <MemoryMap> tst_documentGenerator::createTestMemoryMap(QString const& name,
    QString const& descprition, int addressUnitbits, QList<QSharedPointer<AddressBlock> > addressBlocks)
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap);
    memoryMap->setName(name);
    memoryMap->setAddressUnitBits(QString::number(addressUnitbits));
    memoryMap->setDescription(descprition);

    foreach (QSharedPointer <AddressBlock> includedAddressBlock, addressBlocks)
    {
        memoryMap->getMemoryBlocks()->append(includedAddressBlock);
    }

    return memoryMap;
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getSpaceString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getSpaceString()
{
    return QString("&nbsp;");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getIndentString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getIndentString()
{
    return QString("&nbsp;&nbsp;&nbsp;");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getTableString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getTableString()
{
    return QString("<table frame=\"box\" rules=\"all\" border=\"1\" cellPadding=\"3\" title=\"");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getEncodingString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getEncodingString()
{
    return QString("<meta http-equiv=\"Content-Type\"content=\"text/html; charset=utf-8\">");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getDoctypeString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getDoctypeString()
{
    return QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">");
}

//-----------------------------------------------------------------------------
// Function: tst_documentGenerator::getValidW3CStrictString()
//-----------------------------------------------------------------------------
QString tst_documentGenerator::getValidW3CStrictString()
{
    return QString("\t\t<p>\n"
        "\t\t\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
        "\t\t\t<img src=\"http://www.w3.org/Icons/valid-html401\""
        "alt=\"Valid HTML 4.01 Strict\" height=\"31\""
        "width=\"88\">\n"
        "\t\t\t</a>\n"
        "\t\t</p>\n");
}

QTEST_MAIN(tst_documentGenerator)

#include "tst_documentGenerator.moc"
