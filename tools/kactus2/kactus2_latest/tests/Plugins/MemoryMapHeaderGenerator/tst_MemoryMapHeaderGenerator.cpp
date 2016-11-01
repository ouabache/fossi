//-----------------------------------------------------------------------------
// File: tst_MemoryMapHeaderGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.05.2015
//
// Description:
// Unit test for class MemoryMapHeaderGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/MemoryMapHeaderGenerator/memorymapheadergenerator.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <tests/MockObjects/LibraryMock.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Businterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/SlaveInterface.h>

#include <IPXACTmodels/kactusExtensions/SystemView.h>


//#include <IPXACTmodels/subspacemap.h>

#include <QFile>

class tst_MemoryMapHeaderGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapHeaderGenerator();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testLocalMemoryMapHeaderGeneration();
    void testGenerationWithHexadecimalRegisterOffset();
    void testGenerationWithReferencingRegisterOffset();
    void testLocalMemoryMapAddressBlockIsMemoryOrReserved();

    void testDesignMemoryMapHeaderGeneration();
    void testMemoryMapHeaderGenerationInDesignWithMultipleSlaves();
    void testDesignMemoryMapHeaderWithReferences();
    void testDesignMemoryMapHeaderWithMasterReference();
    void testDesignMemoryMapHeaderWithConfigurableElements();

    void testDesignMemoryMapHeaderWithChannel();
    void testChannelDesignWithReferences();

    void testDesignMemoryMapHeaderWithBridge();
    void testDesignMemoryMapHeaderWithOpaqueBridge();

    //void testSystemMemoryMapHeaderGeneration();

private:

    MemoryMapHeaderGenerator* createTestGenerator();

    QSharedPointer<Register> createTestRegister(QString const& registerName, QString const& addressOffset);

    QSharedPointer<AddressBlock> createTestAddressBlock(QString const& addressName,
        QList<QSharedPointer<Register> > addressBlockRegisters);
    
    QSharedPointer<AddressSpace> createTestAddressSpace(QString const& spaceName,
        QSharedPointer<MemoryMap> localMemoryMap);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& memoryMapName,
        QList<QSharedPointer<AddressBlock> > addressBlocks);

    QSharedPointer<Parameter> createTestParameter(QString const& paramName, QString const& paramID,
        QString const& value);

    QSharedPointer<Component> createTestMasterComponent(QString const& componentName,
        QString const& instanceBaseAddress);

    QSharedPointer<Component> createTestSlaveComponent(QString const& componentName, QString const& registerOffset);

    QSharedPointer<Design> createTestHWDesign(QString const& designName, QSharedPointer<Component> masterComponent,
        QSharedPointer<Component> slaveComponent);

    QSharedPointer<Component> createTestChannelComponent(QString const& componentName,
        QString const& mirroredMasterBaseAddress, QString const& mirroredSlaveRemap,
        QString const& mirroredSlaveRange);

    QSharedPointer<Design> createTestMiddleDesign(QString const& designName,
        QSharedPointer<Component> masterComponent, QSharedPointer<Component> slaveComponent,
        QSharedPointer<Component> middleComponent);

    QSharedPointer<Component> createTestBridgeComponent(QString const& componentName,
        QString const& masterBaseAddress, bool bridgeIsOpaque, QString const& bridgeReference);

    void addConfigurableElementValueToInstance(QSharedPointer<ComponentInstance> instance,
        QString const& referenceID, QString const& configurableValue);

    void readOutPutFile();

    MemoryMapHeaderGenerator* headerGenerator_;

    QSharedPointer<Component> topComponent_;

    LibraryMock library_;

    QString targetPath_;

    QString output_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::tst_MemoryMapHeaderGenerator()
//-----------------------------------------------------------------------------
tst_MemoryMapHeaderGenerator::tst_MemoryMapHeaderGenerator():
headerGenerator_(), topComponent_(), library_(this), targetPath_("./generatorOutput.h"), output_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::init()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    library_.clear();

    headerGenerator_ = createTestGenerator();

    if (QFile::exists(targetPath_))
    {
        QWARN("Output file already exists.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    delete headerGenerator_;
    headerGenerator_ = 0;
    
    QFile::remove(targetPath_);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testLocalMemoryMapHeaderGeneration()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testLocalMemoryMapHeaderGeneration()
{
    QSharedPointer<Register> headerRegister1 = createTestRegister("headerReg", "4");
    QSharedPointer<Register> headerRegister2 = createTestRegister("headerReg2", "10");
    QSharedPointer<Register> headerRegister3 = createTestRegister("headerReg3", "24");

    QList<QSharedPointer<Register> > registerList;
    registerList.append(headerRegister1);
    registerList.append(headerRegister2);

    QList<QSharedPointer<Register> > otherRegisterList;
    otherRegisterList.append(headerRegister3);

    QList<QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressBlock = createTestAddressBlock("localAddressBlock", registerList);
    QSharedPointer<AddressBlock> otherAddressBlock = createTestAddressBlock("otherAddressBlock", otherRegisterList);
    QSharedPointer<AddressBlock> thirdAddressBlock = createTestAddressBlock("thirdBlock",
        QList<QSharedPointer<Register> > ());
    addressBlocks.append(addressBlock);
    addressBlocks.append(otherAddressBlock);
    addressBlocks.append(thirdAddressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemoryMap", addressBlocks);

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaceList(new QList<QSharedPointer<AddressSpace> >());
    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("addressSpace1", localMemoryMap);
    addressSpaceList->append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);
    QSharedPointer<Document> libComponent = library_.getModel(topComponent_->getVlnv());

    QSignalSpy generatorSpy(&adapter, SIGNAL(infoMessage(QString const&)));

    headerGenerator_->runGenerator(&adapter, libComponent);

    QCOMPARE(generatorSpy.count(), 4);

    QString initialMessage = generatorSpy.first().at(0).toString();
    QCOMPARE(initialMessage, QString("Running %1 %2.").arg(headerGenerator_->getName(),
        headerGenerator_->getVersion()));
    QString generatorStartMessage = generatorSpy.at(1).first().toString();
    QCOMPARE(generatorStartMessage, QString("Generation started %1").arg(QDateTime::currentDateTime().
        toString(Qt::LocalDate)));
    QString writingFinishedMessage = generatorSpy.at(2).first().toString();
    QCOMPARE(writingFinishedMessage, QString("Finished writing file generatorOutput.h."));
    QString generatorFinishedMessage = generatorSpy.at(3).first().toString();
    QCOMPARE(generatorFinishedMessage, QString("Generation complete."));

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv().toString("_") + "_" + localMemoryMap->name() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->name() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Address block: " + addressBlock->name() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister1->name() + "\n"
        " * Offset: 4\n"
        "*/\n"
        "#define " + headerRegister1->name().toUpper() + " 0x4\n"
        "/*\n"
        " * Register name: " + headerRegister2->name() + "\n"
        " * Offset: 10\n"
        "*/\n"
        "#define " + headerRegister2->name().toUpper() + " 0xa\n"
        "\n"
        "/*\n"
        " * Address block: " + otherAddressBlock->name() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister3->name() + "\n"
        " * Offset: 24\n"
        "*/\n"
        "#define " + headerRegister3->name().toUpper() + " 0x18\n"
        "\n"
        "/*\n"
        " * Address block: " + thirdAddressBlock->name() + "\n"
        "*/\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testGenerationWithHexadecimalNumber()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testGenerationWithHexadecimalRegisterOffset()
{
    QSharedPointer<Register> headerRegister1 = createTestRegister("headerReg", "'ha");

    QList<QSharedPointer<Register> > registerList;
    registerList.append(headerRegister1);

    QList<QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressBlock = createTestAddressBlock("localAddressBlock", registerList);
    addressBlocks.append(addressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemoryMap", addressBlocks);

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaceList(new QList<QSharedPointer<AddressSpace> >());
    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("addressSpace1", localMemoryMap);
    addressSpaceList->append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);
    QSharedPointer<Document> libComponent = library_.getModel(topComponent_->getVlnv());

    headerGenerator_->runGenerator(&adapter, libComponent);

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv().toString("_") + "_" + localMemoryMap->name() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->name() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Address block: " + addressBlock->name() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister1->name() + "\n"
        " * Offset: 'ha\n"
        "*/\n"
        "#define " + headerRegister1->name().toUpper() + " 0xa\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
    );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testGenerationWithReferencingRegisterOffset()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testGenerationWithReferencingRegisterOffset()
{
    QSharedPointer<Parameter> referencedParameter = createTestParameter("testParameter", "Param_ID", "40");
    topComponent_->getParameters()->append(referencedParameter);

    QSharedPointer<Register> headerRegister1 = createTestRegister("headerReg", "Param_ID");
    QSharedPointer<Register> headerRegister2 = createTestRegister("headerReg2", "Param_ID * 2");
    QSharedPointer<Register> headerRegister3 = createTestRegister("headerReg3", "'h4 + Param_ID");

    QList<QSharedPointer<Register> > registerList;
    registerList.append(headerRegister1);
    registerList.append(headerRegister2);
    registerList.append(headerRegister3);

    QList<QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressBlock = createTestAddressBlock("localAddressBlock", registerList);
    addressBlocks.append(addressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemoryMap", addressBlocks);

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaceList(new QList<QSharedPointer<AddressSpace> >());
    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("addressSpace1", localMemoryMap);
    addressSpaceList->append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);
    QSharedPointer<Document> libComponent = library_.getModel(topComponent_->getVlnv());

    headerGenerator_->runGenerator(&adapter, libComponent);

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv().toString("_") + "_" + localMemoryMap->name() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->name() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Address block: " + addressBlock->name() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister1->name() + "\n"
        " * Offset: testParameter = 0x28\n"
        "*/\n"
        "#define " + headerRegister1->name().toUpper() + " 0x28\n"
        "/*\n"
        " * Register name: " + headerRegister2->name() + "\n"
        " * Offset: testParameter * 2 = 0x50\n"
        "*/\n"
        "#define " + headerRegister2->name().toUpper() + " 0x50\n"
         "/*\n"
        " * Register name: " + headerRegister3->name() + "\n"
        " * Offset: 'h4 + testParameter = 0x2c\n"
        "*/\n"
        "#define " + headerRegister3->name().toUpper() + " 0x2c\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
    );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testLocalMemoryMapAddressBlockIsMemory()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testLocalMemoryMapAddressBlockIsMemoryOrReserved()
{
    QSharedPointer<Parameter> referencedParameter = createTestParameter("refParam", "ID", "14");
    topComponent_->getParameters()->append(referencedParameter);

    QSharedPointer<AddressBlock> memoryAddressBlock = createTestAddressBlock("headerOne",
        QList<QSharedPointer<Register> > ());
    memoryAddressBlock->setUsage(General::MEMORY);
    memoryAddressBlock->setWidth("4 * 8");
    memoryAddressBlock->setRange("8");
    memoryAddressBlock->setBaseAddress("ID + 2");

    QSharedPointer<AddressBlock> reservedAddressBlock = createTestAddressBlock("headerTwo",
        QList<QSharedPointer<Register> > ());
    reservedAddressBlock->setUsage(General::RESERVED);
    reservedAddressBlock->setWidth("16");
    reservedAddressBlock->setRange("ID * ID");

    QList<QSharedPointer<AddressBlock> > addressList;
    addressList.append(memoryAddressBlock);
    addressList.append(reservedAddressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMap", addressList);

    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("localSpace", localMemoryMap);
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaceList(new QList<QSharedPointer<AddressSpace> >());
    addressSpaceList->append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);
    QSharedPointer<Document> libComponent = library_.getModel(topComponent_->getVlnv());

    headerGenerator_->runGenerator(&adapter, libComponent);

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv().toString("_") + "_" + localMemoryMap->name() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->name() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Memory block name: " + memoryAddressBlock->name() + "\n"
        " * Width: 32 = " + memoryAddressBlock->getWidth() + "\n"
        " * Range: " + memoryAddressBlock->getRange() + "\n"
        "*/\n"
        "#define " + memoryAddressBlock->name().toUpper() + "_START 0x10 // refParam + 2" +
            "\n"
        "#define " + memoryAddressBlock->name().toUpper() + "_END 0x17\n"
        "\n"
         "/*\n"
         " * Reserved block name: " + reservedAddressBlock->name() + "\n"
         " * Width: 16\n"
         " * Range: 196 = refParam * refParam\n"
         "*/\n"
         "#define " + reservedAddressBlock->name().toUpper() + "_START 0x" +
             reservedAddressBlock->getBaseAddress() + "\n"
         "#define " + reservedAddressBlock->name().toUpper() + "_END 0xc3\n"
         "\n"
         "#endif /* " + headerDefine + " */\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderGeneration()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderGeneration()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "8");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4");
    QSharedPointer<Design> headerDesign = createTestHWDesign("testDesign", masterComponent, slaveComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    
    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");
    
    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getDesignInstantiations()->append(testInstatiation); 

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    QSignalSpy generatorSpy(&adapter, SIGNAL(infoMessage(QString const&)));

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    QCOMPARE(generatorSpy.count(), 4);

    QString initialMessage = generatorSpy.first().at(0).toString();
    QCOMPARE(initialMessage, QString("Running %1 %2.").arg(headerGenerator_->getName(),
        headerGenerator_->getVersion()));
    QString generatorStartMessage = generatorSpy.at(1).first().toString();
    QCOMPARE(generatorStartMessage, QString("Generation started %1").arg(QDateTime::currentDateTime().
        toString(Qt::LocalDate)));
    QString writingFinishedMessage = generatorSpy.at(2).first().toString();
    QCOMPARE(writingFinishedMessage, QString("Finished writing file generatorOutput.h."));
    QString generatorFinishedMessage = generatorSpy.at(3).first().toString();
    QCOMPARE(generatorFinishedMessage, QString("Generation complete."));


    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x8\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0xc\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderGenerationWithMultipleSlaves()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testMemoryMapHeaderGenerationInDesignWithMultipleSlaves()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4");
    QSharedPointer<Component> slaveComponentTwo = createTestSlaveComponent("slaveComponentTwo", "8");

    QSharedPointer<Design> headerDesign (new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "headerDesign",
        "1.0")));

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponentTwo);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponentTwo.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);

    QSharedPointer<ComponentInstance> masterInstance (new ComponentInstance("masterInstance_0", 
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(masterComponent->getVlnv())))); 
    masterInstance->setUuid("masterID");

    QSharedPointer<ComponentInstance> slaveInstanceOne (new ComponentInstance("slaveInstance_0", 
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveComponent->getVlnv())))); 
    slaveInstanceOne->setUuid("slaveID_0");

    QSharedPointer<ComponentInstance> slaveInstanceTwo (new ComponentInstance("slaveInstance_1",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveComponent->getVlnv()))));
    slaveInstanceTwo->setUuid("slaveID_1");

    QSharedPointer<ComponentInstance> slaveInstanceThree (new ComponentInstance("slaveInstanceTwo_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveComponentTwo->getVlnv()))));
    slaveInstanceThree->setUuid("slaveIDTwo_0");
    
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances(
        new QList<QSharedPointer<ComponentInstance> >());
    componentInstances->append(masterInstance);
    componentInstances->append(slaveInstanceOne);
    componentInstances->append(slaveInstanceTwo);
    componentInstances->append(slaveInstanceThree);
    headerDesign->setComponentInstances(componentInstances);

    QSharedPointer<ActiveInterface> interfaceMaster (
        new ActiveInterface(masterInstance->getInstanceName(), masterComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceSlaveOne (new ActiveInterface(
        slaveInstanceOne->getInstanceName(), slaveComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceSlaveTwo (new ActiveInterface(
        slaveInstanceTwo->getInstanceName(), slaveComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceSlaveThree (new ActiveInterface(
        slaveInstanceThree->getInstanceName(), slaveComponent->getBusInterfaceNames().at(0)));

    QSharedPointer<Interconnection> masterSlaveOneConnection(
        new Interconnection("masterSlaveOne", interfaceMaster));
    masterSlaveOneConnection->getActiveInterfaces()->append(interfaceSlaveOne);

    QSharedPointer<Interconnection> masterSlaveTwoConnection(new Interconnection(
        "masterSlaveTwo", interfaceMaster));
    masterSlaveTwoConnection->getActiveInterfaces()->append(interfaceSlaveTwo);

    QSharedPointer<Interconnection> masterSlaveThreeConnection(new Interconnection(
        "msaterSlaveThree", interfaceMaster));
    masterSlaveThreeConnection->getActiveInterfaces()->append(interfaceSlaveThree);

    QSharedPointer<QList<QSharedPointer<Interconnection> > >  interconnectionList = 
        headerDesign->getInterconnections();
    interconnectionList->append(masterSlaveOneConnection);
    interconnectionList->append(masterSlaveTwoConnection);
    interconnectionList->append(masterSlaveThreeConnection);
    
    QSharedPointer<View> headerTestView(new View("headerView"));
    
    headerTestView->setDesignInstantiationRef("testInstantiation");
    
    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: " + slaveInstanceOne->getInstanceName() + " Interface: " +
            slaveComponent->getBusInterfaceNames().at(0) + "\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define " + slaveInstanceOne->getInstanceName().toUpper() + "_SLAVEREGISTER 0x4\n"
        "\n"
        "/*\n"
        " * Instance: " + slaveInstanceTwo->getInstanceName() + " Interface: " +
            slaveComponent->getBusInterfaceNames().at(0) + "\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define " + slaveInstanceTwo->getInstanceName().toUpper() + "_SLAVEREGISTER 0x4\n"
        "\n"
        "/*\n"
        " * Instance: " + slaveInstanceThree->getInstanceName() + " Interface: " +
            slaveComponentTwo->getBusInterfaceNames().at(0) + "\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponentTwo->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 8\n"
        "*/\n"
        "#define " + slaveInstanceThree->getInstanceName().toUpper() + "_SLAVEREGISTER 0x8\n"
        "\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithReferences()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithReferences()
{
    QSharedPointer<Parameter> masterParameter = createTestParameter("masterParameter", "param_ID", "14");
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "10");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "8");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4 * param_ID");
    QSharedPointer<Design> headerDesign = createTestHWDesign("testDesign", masterComponent, slaveComponent);

    masterComponent->getParameters()->append(masterParameter);
    slaveComponent->getParameters()->append(slaveParameter);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    
    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x8\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4 * slaveParameter = 0x28\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x30\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithMasterReference()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithMasterReference()
{
    QSharedPointer<Parameter> masterParameter = createTestParameter("masterParameter", "master_ID", "14");
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "10");
    QSharedPointer<Parameter> slaveParameterZero = createTestParameter("slaveZero", "zero_ID", "2");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "master_ID + 4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "zero_ID * param_ID");
    QSharedPointer<Design> headerDesign = createTestHWDesign("testDesign", masterComponent, slaveComponent);

    masterComponent->getParameters()->append(masterParameter);
    slaveComponent->getParameters()->append(slaveParameter);
    slaveComponent->getParameters()->append(slaveParameterZero);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    
    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x12\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: slaveZero * slaveParameter = 0x14\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x26\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithConfigurableElements()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithConfigurableElements()
{
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "10");
    QSharedPointer<Parameter> otherSlaveParameter = createTestParameter("otherParameter", "other_ID", "14");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");

    QSharedPointer<Component> slaveComponent (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary",
        "slaveComponent", "1.0")));

    QSharedPointer<Register> slaveRegister = createTestRegister("slaveRegister", "param_ID * 6");
    QSharedPointer<Register> otherSlaveRegister = createTestRegister("otherRegister", "other_ID + 2");
    QList<QSharedPointer<Register> > slaveRegisterList;
    slaveRegisterList.append(slaveRegister);
    slaveRegisterList.append(otherSlaveRegister);

    QSharedPointer<AddressBlock> slaveAddress = createTestAddressBlock("slaveAddress", slaveRegisterList);
    QList<QSharedPointer<AddressBlock> > slaveAddressList;
    slaveAddressList.append(slaveAddress);

    QSharedPointer<MemoryMap> slaveMemory = createTestMemoryMap("slaveMemory", slaveAddressList);

    QSharedPointer<BusInterface> slaveBus (new BusInterface());
    slaveBus->setName("slaveBusInterface");
    QSharedPointer<SlaveInterface> enslavedInterface(new SlaveInterface);
    enslavedInterface->setMemoryMapRef(slaveMemory->name());
    slaveBus->setSlave(enslavedInterface);

    slaveComponent->getMemoryMaps()->append(slaveMemory);

    slaveComponent->getBusInterfaces()->append(slaveBus);

    slaveComponent->getParameters()->append(slaveParameter);
    slaveComponent->getParameters()->append(otherSlaveParameter);

    QSharedPointer<Design> headerDesign (new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "testDesign", "1.0")));

    QSharedPointer<DesignConfiguration> headerDesignConfiguration (new DesignConfiguration(VLNV(
        VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "testDesignConfiguration", "1.0")));

    QSharedPointer<ComponentInstance> masterInstance (new ComponentInstance("masterInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(masterComponent->getVlnv()))));
    masterInstance->setUuid("masterID");

    QSharedPointer<ComponentInstance> slaveInstance (new ComponentInstance("slaveInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveComponent->getVlnv()))));
    slaveInstance->setUuid("slaveID");

    addConfigurableElementValueToInstance(slaveInstance, "param_ID", "4");
    addConfigurableElementValueToInstance(slaveInstance, "other_ID", "param_ID + 20");

    headerDesign->getComponentInstances()->append(masterInstance);
    headerDesign->getComponentInstances()->append(slaveInstance);

    QSharedPointer<ActiveInterface> interfaceMaster(
        new ActiveInterface(masterInstance->getInstanceName(), masterComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceSlave (
        new ActiveInterface(slaveInstance->getInstanceName(), slaveComponent->getBusInterfaceNames().at(0)));

    QSharedPointer<Interconnection> masterSlaveConnection(new Interconnection("masterSlaveConnection", interfaceMaster));
    masterSlaveConnection->getActiveInterfaces()->append(interfaceSlave);

    headerDesign->getInterconnections()->append(masterSlaveConnection);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    library_.addComponent(headerDesignConfiguration);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesignConfiguration.1.0.xml",
        topComponent_);

    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, headerDesignConfiguration, headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemory\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddress\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: slaveParameter * 6 = 0x18\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x18\n"
        "/*\n"
        " * Register name: otherRegister\n"
        " * Offset: otherParameter + 2 = 0x1a\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_OTHERREGISTER 0x1a\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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

    // Check that the parameters were not changed.
    QCOMPARE(slaveParameter->getValue(), QString("10"));
    QCOMPARE(otherSlaveParameter->getValue(), QString("14"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithChannel()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithChannel()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "8");
    QSharedPointer<Component> channelComponent = createTestChannelComponent("channelComponent", "0", "4", "32");
    QSharedPointer<Design> headerDesign = createTestMiddleDesign("channelDesign", masterComponent, slaveComponent,
        channelComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(channelComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelDesign.1.0.xml", topComponent_);

    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x8\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 8\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x10\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testChannelDesignWithReferences()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testChannelDesignWithReferences()
{
    QSharedPointer<Parameter> masterParameter = createTestParameter("masterParameter", "master_ID", "2");
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "4");
    QSharedPointer<Parameter> otherSlaveParameter = createTestParameter("otherParameter", "other_ID", "8");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "param_ID");
    QSharedPointer<Component> channelComponent = createTestChannelComponent("channelComponent",
        "master_ID", "param_ID+3", "other_ID*master_ID");
    QSharedPointer<Design> headerDesign = createTestMiddleDesign("channelDesign", masterComponent, slaveComponent,
        channelComponent);

    channelComponent->getParameters()->append(masterParameter);
    channelComponent->getParameters()->append(slaveParameter);
    channelComponent->getParameters()->append(otherSlaveParameter);

    slaveComponent->getParameters()->append(slaveParameter);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(channelComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelDesign.1.0.xml", topComponent_);

    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0xb\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: slaveParameter = 0x4\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0xf\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithBridge()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithBridge()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4");
    QSharedPointer<Component> bridgeComponent = createTestBridgeComponent("bridgeComponent", "16", false, "");

    QSharedPointer<Design> headerDesign = createTestMiddleDesign("bridgeDesign", masterComponent, slaveComponent,
        bridgeComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(bridgeComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeDesign.1.0.xml", topComponent_);
    
    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv().toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x14\n"
        " * Source component: " + slaveComponent->getVlnv().toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x18\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
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
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithOpaqueBridge()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithOpaqueBridge()
{

   /* QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "2");

    QSharedPointer<MemoryMap> opaqueMemoryMap = createTestMemoryMap("opaqueMemoryMap",
        QList<QSharedPointer<AddressBlock> > ());
    
    QSharedPointer<> opaqueSubspace (new SubspaceMap());
    opaqueSubspace->setBaseAddress("16");

    QList<QSharedPointer<MemoryMapItem> > opaqueItems;
    opaqueItems.append(opaqueSubspace);
    opaqueMemoryMap->setItems(opaqueItems);

    QSharedPointer<Component> bridgeComponent = createTestBridgeComponent("opqueBridge", "4", true,
        opaqueMemoryMap->name());

    bridgeComponent->getMemoryMaps()->append(opaqueMemoryMap);

    QSharedPointer<Design> headerDesign = createTestMiddleDesign("bridgeDesign", masterComponent, slaveComponent,
        bridgeComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(bridgeComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeDesign.1.0.xml", topComponent_);

    QSharedPointer<View> headerTestView(new View("headerView"));
    headerTestView->setDesignInstantiationRef("testInstantiation");

    QSharedPointer<DesignInstantiation> testInstatiation(new DesignInstantiation("testInstantiation"));
    testInstatiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(headerDesign->getVlnv())));

    topComponent_->getViews()->append(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    QSignalSpy errorMessageSignals(&adapter, SIGNAL(errorMessage(QString const&)));

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<Document>(), headerDesign);

    QCOMPARE(errorMessageSignals.count(), 1);
    QString errorMessage = errorMessageSignals.first().first().toString();
    QString expectedErrorMessage("An opaque bridge was found in interface bridgeSlaveBusInterface in instance "
        "channelInstance_0. Currently, opaque bridges are handled as transparent bridges.");
    QCOMPARE(errorMessage, expectedErrorMessage);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        );

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }*/
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testSystemMemoryMapHeaderGeneration()
//-----------------------------------------------------------------------------
/*
void tst_MemoryMapHeaderGenerator::testSystemMemoryMapHeaderGeneration()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "8");
    QSharedPointer<Design> hwDesign = createTestHWDesign("hwDesign", masterComponent, slaveComponent);
    QSharedPointer<DesignConfiguration> hwDesignConfiguration (new DesignConfiguration(VLNV(
        VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "hwDesignConfiguration", "1.0")));

    QSharedPointer<Design> systemDesign (new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "systemDesign",
        "1.0")));
    QSharedPointer<DesignConfiguration> systemDesignConfiguration (new DesignConfiguration(VLNV(
        VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "systemDesignConfiguration", "1.0")));

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(hwDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/hwDesign.1.0.xml", topComponent_);
    library_.addComponent(hwDesignConfiguration);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/hwDesignConfiguration.1.0.xml", topComponent_);
    library_.addComponent(systemDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/systemDesign.1.0.xml", topComponent_);
    library_.addComponent(systemDesignConfiguration);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/systemDesignConfiguration.1.0.xml",
        topComponent_);

    View* hwDesignView = new View("headerView");
    hwDesignView->setHierarchyRef(*hwDesignConfiguration->getVlnv());
    topComponent_->addView(hwDesignView);

    SystemView* systemDesignView = new SystemView("systemView");
    systemDesignView->setHierarchyRef(*systemDesignConfiguration->getVlnv());
    systemDesignView->setHWViewRef(hwDesignView->name());
    topComponent_->addSystemView(systemDesignView);

       QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, "", this);

    headerGenerator_->runGenerator(&adapter, topComponent_, systemDesignConfiguration, systemDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "abc"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}*/

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestGenerator()
//-----------------------------------------------------------------------------
MemoryMapHeaderGenerator* tst_MemoryMapHeaderGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    MemoryMapHeaderGenerator* generator = new MemoryMapHeaderGenerator();

    return generator;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestRegister()
//-----------------------------------------------------------------------------
QSharedPointer<Register> tst_MemoryMapHeaderGenerator::createTestRegister(QString const& registerName,
    QString const& addressOffset)
{
    QSharedPointer<Register> newRegister (new Register());
    newRegister->setName(registerName);
    newRegister->setAddressOffset(addressOffset);
    newRegister->setSize("16");

    return newRegister;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> tst_MemoryMapHeaderGenerator::createTestAddressBlock(QString const& addressName,
    QList<QSharedPointer<Register> > addressBlockRegisters)
{
    QSharedPointer<AddressBlock> newAddressBlock (new AddressBlock());
    newAddressBlock->setName(addressName);
    newAddressBlock->setBaseAddress("0");
    newAddressBlock->setRange("16");
    newAddressBlock->setWidth("32");
    newAddressBlock->setUsage(General::REGISTER);
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerModelRef =newAddressBlock->getRegisterData();

    foreach(QSharedPointer<Register> targetRegister, addressBlockRegisters)
    {
        registerModelRef->append(targetRegister);
    }

    return newAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> tst_MemoryMapHeaderGenerator::createTestMemoryMap(QString const& memoryMapName,
    QList<QSharedPointer<AddressBlock> > addressBlocks)
{
    QSharedPointer<MemoryMap> newMemoryMap (new MemoryMap());
    newMemoryMap->setName(memoryMapName);
    newMemoryMap->setAddressUnitBits("8");

    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > newMemoryMapItems = newMemoryMap->getMemoryBlocks();

    foreach (QSharedPointer<AddressBlock> newAddressBlock, addressBlocks)
    {
        newMemoryMapItems->append(newAddressBlock);
    }

    return newMemoryMap;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> tst_MemoryMapHeaderGenerator::createTestAddressSpace(QString const& spaceName,
    QSharedPointer<MemoryMap> localMemoryMap)
{
    QSharedPointer<AddressSpace> newAddressSpace(new AddressSpace());
    newAddressSpace->setName(spaceName);
    newAddressSpace->setLocalMemoryMap(localMemoryMap);

    return newAddressSpace;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_MemoryMapHeaderGenerator::createTestParameter(QString const& paramName,
    QString const& paramID, QString const& value)
{
    QSharedPointer<Parameter> newParameter(new Parameter);
    newParameter->setName(paramName);
    newParameter->setValueId(paramID);
    newParameter->setValue(value);

    return newParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestMasterComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestMasterComponent(QString const& componentName,
    QString const& instanceBaseAddress)
{
    QSharedPointer<Component> newMaster (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary", componentName,
        "1.0")));

    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("localSpace", QSharedPointer<MemoryMap> ());
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaceList(new QList<QSharedPointer<AddressSpace> >());
    addressSpaceList->append(localAddressSpace);

    newMaster->setAddressSpaces(addressSpaceList);

    QStringList addressSpaceRefs;
    addressSpaceRefs.append(localAddressSpace->name());

    QSharedPointer<Cpu> masterCpu (new Cpu());
    masterCpu->setAddressSpaceRefs(addressSpaceRefs);
    
    newMaster->getCpus()->append(masterCpu);

    QSharedPointer<BusInterface> masterBus (new BusInterface());
    masterBus->setName("masterBusInterface");
    QSharedPointer<MasterInterface> interfaceMaster(new MasterInterface);
    interfaceMaster->setBaseAddress(instanceBaseAddress);
    interfaceMaster->setAddressSpaceRef(localAddressSpace->name());
    masterBus->setMaster(interfaceMaster);
    masterBus->setInterfaceMode(General::MASTER);

    newMaster->getBusInterfaces()->append(masterBus);

    return newMaster;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createSlaveComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestSlaveComponent(QString const& componentName,
    QString const& registerOffset)
{
    QSharedPointer<Component> newSlave (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary", componentName,
        "1.0")));

    QSharedPointer<Register> slaveRegister = createTestRegister("slaveRegister", registerOffset);
    QList<QSharedPointer<Register> > componentRegisters;
    componentRegisters.append(slaveRegister);

    QSharedPointer<AddressBlock> slaveAddressBlock = createTestAddressBlock("slaveAddressBlock",
        componentRegisters);
    QList<QSharedPointer<AddressBlock> > addressBlocks;
    addressBlocks.append(slaveAddressBlock);

    QSharedPointer<MemoryMap> slaveMemoryMap = createTestMemoryMap("slaveMemoryMap", addressBlocks);
    
    newSlave->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<BusInterface> slaveBus (new BusInterface());
    slaveBus->setName("slaveBusInterface");
    QSharedPointer<SlaveInterface> interfaceSlave(new SlaveInterface);
    interfaceSlave->setMemoryMapRef(slaveMemoryMap->name());
    slaveBus->setSlave(interfaceSlave);

    newSlave->getBusInterfaces()->append(slaveBus);

    return newSlave;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_MemoryMapHeaderGenerator::createTestHWDesign(QString const& designName,
    QSharedPointer<Component> masterComponent, QSharedPointer<Component> slaveComponent)
{
    QSharedPointer<Design> newDesign(new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", designName, "1.0")));

    QSharedPointer<ComponentInstance> masterInstance (new ComponentInstance("masterInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(masterComponent->getVlnv()))));
    masterInstance->setUuid("masterID");

    QSharedPointer<ComponentInstance> slaveInstance (new ComponentInstance("slaveInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveComponent->getVlnv()))));
    slaveInstance->setUuid("slaveID");
    
    newDesign->getComponentInstances()->append(masterInstance);
    newDesign->getComponentInstances()->append(slaveInstance);

    QSharedPointer<ActiveInterface> interfaceMaster (
        new ActiveInterface(masterInstance->getInstanceName(), masterComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceSlave (
        new ActiveInterface(slaveInstance->getInstanceName(), slaveComponent->getBusInterfaceNames().at(0)));

    QSharedPointer<Interconnection> masterSlaveConnection(
        new Interconnection("masterSlaveConnection", interfaceMaster));
    masterSlaveConnection->getActiveInterfaces()->append(interfaceSlave);

    newDesign->getInterconnections()->append(masterSlaveConnection);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestChannelComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestChannelComponent(QString const& componentName,
    QString const& mirroredMasterBaseAddress, QString const& mirroredSlaveRemap, QString const& mirroredSlaveRange)
{
    QSharedPointer<Component> newChannelComponent (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary",
        componentName, "1.0")));

    QSharedPointer<Register> localRegister = createTestRegister("mirroredRegister", "4");
    QList<QSharedPointer<Register> > localRegisterList;
    localRegisterList.append(localRegister);

    QSharedPointer<AddressBlock> mirroredAddressBlock = createTestAddressBlock("mirroredAddressBlock",
        localRegisterList);
    QList<QSharedPointer<AddressBlock> > localAddressBlockList;
    localAddressBlockList.append(mirroredAddressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMasterMemory", localAddressBlockList);

    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("localAddressSpace", localMemoryMap);
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaceList(new QList<QSharedPointer<AddressSpace> >());
    addressSpaceList->append(localAddressSpace);

    newChannelComponent->setAddressSpaces(addressSpaceList);

    // Create mirrored master interface.
    QSharedPointer<BusInterface> mirroredMasterBus(new BusInterface);
    mirroredMasterBus->setName("mirroredMasterInterface");
    QSharedPointer<MasterInterface> mirroredMasterInterface(new MasterInterface);
    mirroredMasterInterface->setBaseAddress(mirroredMasterBaseAddress);
    mirroredMasterInterface->setAddressSpaceRef(localAddressSpace->name());
    mirroredMasterBus->setMaster(mirroredMasterInterface);
    mirroredMasterBus->setInterfaceMode(General::MIRROREDMASTER);

    // Create mirrored slave interface.
    QSharedPointer<MirroredSlaveInterface> interfaceMirroredSlave (new MirroredSlaveInterface());
    QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress(
        new MirroredSlaveInterface::RemapAddress(mirroredSlaveRemap));
    interfaceMirroredSlave->getRemapAddresses()->append(remapAddress);
    interfaceMirroredSlave->setRange(mirroredSlaveRange);

    QSharedPointer<BusInterface> mirroredSlaveBus(new BusInterface);
    mirroredSlaveBus->setName("mirroredSlaveInterface");
    mirroredSlaveBus->setMirroredSlave(interfaceMirroredSlave);
    mirroredSlaveBus->setInterfaceMode(General::MIRROREDSLAVE);

    newChannelComponent->getBusInterfaces()->append(mirroredMasterBus);
    newChannelComponent->getBusInterfaces()->append(mirroredSlaveBus);

    QStringList interfaceNames;
    interfaceNames.append(mirroredMasterBus->name());
    interfaceNames.append(mirroredSlaveBus->name());

    QSharedPointer<Channel> masterSlaveChannel (new Channel());
    masterSlaveChannel->setName("masterSlaveChannel");
    masterSlaveChannel->setInterfaces(interfaceNames);

    newChannelComponent->getChannels()->append(masterSlaveChannel);

    return newChannelComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestChannelDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_MemoryMapHeaderGenerator::createTestMiddleDesign(QString const& designName,
    QSharedPointer<Component> masterComponent, QSharedPointer<Component> slaveComponent,
    QSharedPointer<Component> middleComponent)
{
    QSharedPointer<Design> newDesign(new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", designName, "1.0")));

    QSharedPointer<ComponentInstance> masterInstance (new ComponentInstance("masterInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(masterComponent->getVlnv()))));
    masterInstance->setUuid("masterID");

    QSharedPointer<ComponentInstance> slaveInstance (new ComponentInstance("slaveInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveComponent->getVlnv()))));
    slaveInstance->setUuid("slaveID");

    QSharedPointer<ComponentInstance> channelInstance (new ComponentInstance("channelInstance_0",
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(middleComponent->getVlnv()))));
    channelInstance->setUuid("middleID");

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances = newDesign->getComponentInstances();
    componentInstances->append(masterInstance);
    componentInstances->append(slaveInstance);
    componentInstances->append(channelInstance);    

    QSharedPointer<ActiveInterface> interfaceMaster (new ActiveInterface(masterInstance->getInstanceName(),
        masterComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceMiddleFirst (new ActiveInterface(channelInstance->getInstanceName(),
        middleComponent->getBusInterfaceNames().at(0)));
    QSharedPointer<ActiveInterface> interfaceMiddleSecond (new ActiveInterface(channelInstance->getInstanceName(),
        middleComponent->getBusInterfaceNames().at(1)));
    QSharedPointer<ActiveInterface> interfaceSlave (new ActiveInterface(slaveInstance->getInstanceName(), 
        slaveComponent->getBusInterfaceNames().at(0)));

    QSharedPointer<Interconnection> masterMiddleConnect(new Interconnection("masterMirrored", interfaceMaster));
    masterMiddleConnect->getActiveInterfaces()->append(interfaceMiddleFirst);

    QSharedPointer<Interconnection> middleSlaveConnect(new Interconnection("mirroredSlave", interfaceMiddleSecond));
    middleSlaveConnect->getActiveInterfaces()->append(interfaceSlave);

    newDesign->getInterconnections()->append(masterMiddleConnect);
    newDesign->getInterconnections()->append(middleSlaveConnect);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestBridgeComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestBridgeComponent(QString const& componentName,
    QString const& masterBaseAddress, bool bridgeIsOpaque, QString const& bridgeReference)
{
    QSharedPointer<Component> newBridgeComponent (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary",
        componentName, "1.0")));

    QSharedPointer<Register> masterRegister = createTestRegister("bridgeMasterRegister", "4");
    QList<QSharedPointer<Register> > masterRegisterList;
    masterRegisterList.append(masterRegister);

    QSharedPointer<AddressBlock> masterAddressBlock = createTestAddressBlock("bridgeMasterAddressBlock",
        masterRegisterList);
    QList<QSharedPointer<AddressBlock> > masterAddressBlockList;
    masterAddressBlockList.append(masterAddressBlock);

    QSharedPointer<MemoryMap> masterMemory = createTestMemoryMap("bridgeMasterMemory", masterAddressBlockList);

    QSharedPointer<AddressSpace> masterAddressSpace = createTestAddressSpace("bridgetMasterSpace", masterMemory);

    newBridgeComponent->getAddressSpaces()->append(masterAddressSpace);

    // Create master bus interface.
    QSharedPointer<MasterInterface> bridgeMasterInterface(new MasterInterface);
    bridgeMasterInterface->setBaseAddress(masterBaseAddress);
    bridgeMasterInterface->setAddressSpaceRef(masterAddressSpace->name());

    QSharedPointer<BusInterface> bridgeMasterBus(new BusInterface);
    bridgeMasterBus->setName("bridgeMasterBusInterface");
    bridgeMasterBus->setMaster(bridgeMasterInterface);
    bridgeMasterBus->setInterfaceMode(General::MASTER);

    // Create slave bus interface.
    QSharedPointer<SlaveInterface> bridgeSlaveInterface (new SlaveInterface);
    if (!bridgeReference.isEmpty())
    {
        bridgeSlaveInterface->setMemoryMapRef(bridgeReference);
    }

    QSharedPointer<SlaveInterface::Bridge> bridge(new SlaveInterface::Bridge());
    bridge->masterRef_ = bridgeMasterBus->name();
    //bridge->opaque_ = bridgeIsOpaque;

    bridgeSlaveInterface->getBridges()->append(bridge); // setBridges(slaveInterfaceBridges);

    QSharedPointer<BusInterface> bridgeSlaveBus (new BusInterface);
    bridgeSlaveBus->setName("bridgeSlaveBusInterface");
    bridgeSlaveBus->setSlave(bridgeSlaveInterface);
    bridgeSlaveBus->setInterfaceMode(General::SLAVE);

    newBridgeComponent->getBusInterfaces()->append(bridgeSlaveBus);
    newBridgeComponent->getBusInterfaces()->append(bridgeMasterBus);

    return newBridgeComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::addConfigurableElementValueToInstance()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::addConfigurableElementValueToInstance(
    QSharedPointer<ComponentInstance> instance, QString const& referenceID, QString const& configurableValue)
{
    QSharedPointer<ConfigurableElementValue> configurable (
        new ConfigurableElementValue(configurableValue, referenceID));
    instance->getConfigurableElementValues()->append(configurable);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::readOutPutFile()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::readOutPutFile()
{
    QFile outputFile(targetPath_);
    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

//QTEST_APPLESS_MAIN(tst_MemoryMapHeaderGenerator)
QTEST_MAIN(tst_MemoryMapHeaderGenerator)

#include "tst_MemoryMapHeaderGenerator.moc"
