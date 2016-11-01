//-----------------------------------------------------------------------------
// File: tst_MemoryMapGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.04.2015
//
// Description:
// Unit test for class MemoryMapGraphItem.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/addressblockgraphitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <editors/ComponentEditor/common/NullParser.h>
#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>

class tst_MemoryMapGraphItem : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapGraphItem();
    
private slots:

    void testConstructor();

    void testMemoryMapWithAddressBlock();
    
    void testMemoryMapWithConsecutiveAddressBlocks();
  
    void testGapBetweenTwoAddressBlocks();

    void testPartiallyOverlappingAddressBlocks();

    void testFullyOverlappingAddressBlocks();

    void testMultipleBlocksOverlapping();

    void testTwoBlocksCompletelyOverlappingThird();

    void testTwoOverlappingBlocksInsideThrid();

    void testIdenticalBlocksOverlappingThird();

    void testConsecutiveBlocksInsideThird();

    void testExpressions();

private:
    MemoryMapGraphItem* createMemoryBlockBase(QSharedPointer<MemoryMap> memoryMap);

    QSharedPointer<AddressBlock> createAddressBlock(QString const& name, QString const& baseAddress, QString const& range, int bitWidth);

    void expandItem(MemoryMapGraphItem* memoryMapItem);

    QList<MemoryGapItem*> findMemoryGaps(MemoryMapGraphItem* memoryMapItem);
   
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::tst_MemoryMapGraphItem()
//-----------------------------------------------------------------------------
tst_MemoryMapGraphItem::tst_MemoryMapGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testConstructor()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testConstructor()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setName("testMap");
    memoryMap->setAddressUnitBits("8");

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QCOMPARE(memoryMapItem->name(), QString("testMap"));
    QCOMPARE(memoryMapItem->getOffset(), quint64(0));
    QCOMPARE(memoryMapItem->getLastAddress(), quint64(0));
    QCOMPARE(memoryMapItem->getDisplayOffset(), quint64(0));
    QCOMPARE(memoryMapItem->getDisplayLastAddress(), quint64(0));

    QCOMPARE(memoryMapItem->getAddressUnitSize(), unsigned int(8));
    QCOMPARE(memoryMapItem->getBitWidth(), 0);

    QCOMPARE(memoryMapItem->pos().y(), qreal(0));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testMemoryMapWithAddressBlock()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testMemoryMapWithAddressBlock()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setName("testMap");
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> addressBlock = createAddressBlock("testAddressBlock", "8", "1", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(addressBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);
    
    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* addressBlockItem = new AddressBlockGraphItem(addressBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(addressBlockItem);
    memoryMapItem->refresh();

    QCOMPARE(memoryMapItem->name(), QString("testMap"));
    QCOMPARE(memoryMapItem->getOffset(), quint64(8));
    QCOMPARE(memoryMapItem->getLastAddress(), quint64(8));
    QCOMPARE(memoryMapItem->getDisplayOffset(), quint64(8));
    QCOMPARE(memoryMapItem->getDisplayLastAddress(), quint64(8));

    QVERIFY(!addressBlockItem->isVisible());

    expandItem(memoryMapItem);

    QVERIFY(addressBlockItem->isVisible());
    QVERIFY(!addressBlockItem->isConflicted());
    QCOMPARE(addressBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(addressBlockItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testMemoryMapWithConsecutiveAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testMemoryMapWithConsecutiveAddressBlocks()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> firstBlock = createAddressBlock("firstAddressBlock", "0", "1", 8);
    QSharedPointer<AddressBlock> secondBlock = createAddressBlock("secondAddressBlock", "1", "1", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(firstBlock);
    addressBlocks.append(secondBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* firstBlockItem = new AddressBlockGraphItem(firstBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* secondBlockItem = new AddressBlockGraphItem(secondBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(firstBlockItem);
    memoryMapItem->addChild(secondBlockItem);
    memoryMapItem->refresh();

    QCOMPARE(memoryMapItem->getOffset(), quint64(0));
    QCOMPARE(memoryMapItem->getLastAddress(), quint64(1));
    QCOMPARE(memoryMapItem->getDisplayOffset(), quint64(0));
    QCOMPARE(memoryMapItem->getDisplayLastAddress(), quint64(1));

    expandItem(memoryMapItem);

    QVERIFY(firstBlockItem->isVisible());
    QCOMPARE(firstBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstBlockItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));

    QVERIFY(secondBlockItem->isVisible());
    QCOMPARE(secondBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondBlockItem->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testGapBetweenTwoAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testGapBetweenTwoAddressBlocks()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> firstBlock = createAddressBlock("firstAddressBlock", "0", "1", 8);
    QSharedPointer<AddressBlock> lastBlock = createAddressBlock("lastAddressBlock", "2", "1", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(firstBlock);
    addressBlocks.append(lastBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* firstBlockItem = new AddressBlockGraphItem(firstBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* secondBlockItem = new AddressBlockGraphItem(lastBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(firstBlockItem);
    memoryMapItem->addChild(secondBlockItem);

    firstBlockItem->refresh();
    secondBlockItem->refresh();

    expandItem(memoryMapItem);

    QVERIFY(firstBlockItem->isVisible());
    QCOMPARE(firstBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstBlockItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(firstBlockItem->getDisplayLastAddress(), quint64(0));

    MemoryGapItem* gap = findMemoryGaps(memoryMapItem).first();

    QVERIFY(gap->isVisible());
    QVERIFY(!gap->isConflicted());
    QCOMPARE(gap->name(), QString("Reserved"));
    QCOMPARE(gap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(gap->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(gap->getDisplayOffset(), quint64(1));
    QCOMPARE(gap->getDisplayLastAddress(), quint64(1));

    QVERIFY(secondBlockItem->isVisible());
    QCOMPARE(secondBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondBlockItem->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondBlockItem->getDisplayOffset(), quint64(2));
    QCOMPARE(secondBlockItem->getDisplayLastAddress(), quint64(2));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testPartiallyOverlappingAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testPartiallyOverlappingAddressBlocks()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> firstBlock = createAddressBlock("firstAddressBlock", "0", "2", 8);
    QSharedPointer<AddressBlock> secondBlock = createAddressBlock("secondAddressBlock", "1", "2", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(firstBlock);
    addressBlocks.append(secondBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* firstBlockItem = new AddressBlockGraphItem(firstBlock, noParser, memoryMapItem);
    firstBlockItem->setAddressableUnitBits(8);

    AddressBlockGraphItem* secondBlockItem = new AddressBlockGraphItem(secondBlock, noParser, memoryMapItem);
    secondBlockItem->setAddressableUnitBits(8);

    memoryMapItem->addChild(firstBlockItem);
    memoryMapItem->addChild(secondBlockItem);

    firstBlockItem->refresh();
    secondBlockItem->refresh();

    expandItem(memoryMapItem);

    QVERIFY(firstBlockItem->isVisible());
    QCOMPARE(firstBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstBlockItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(firstBlockItem->getDisplayLastAddress(), quint64(0));

    MemoryGapItem* overlappingItem = findMemoryGaps(memoryMapItem).first();
    QVERIFY(overlappingItem->isVisible());
    QVERIFY(overlappingItem->isConflicted());
    QCOMPARE(overlappingItem->name(), QString("conflicted"));
    QCOMPARE(overlappingItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(overlappingItem->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(overlappingItem->getDisplayOffset(), quint64(1));
    QCOMPARE(overlappingItem->getDisplayLastAddress(), quint64(1));

    QVERIFY(secondBlockItem->isVisible());
    QCOMPARE(secondBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondBlockItem->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondBlockItem->getDisplayOffset(), quint64(2));
    QCOMPARE(secondBlockItem->getDisplayLastAddress(), quint64(2));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testFullyOverlappingAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testFullyOverlappingAddressBlocks()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> firstBlock = createAddressBlock("firstAddressBlock", "0", "1", 8);
    QSharedPointer<AddressBlock> secondBlock = createAddressBlock("secondAddressBlock", "0", "1", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(firstBlock);
    addressBlocks.append(secondBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* firstBlockItem = new AddressBlockGraphItem(firstBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* secondBlockItem = new AddressBlockGraphItem(secondBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(firstBlockItem);
    memoryMapItem->addChild(secondBlockItem);

    firstBlockItem->refresh();
    secondBlockItem->refresh();

    expandItem(memoryMapItem);

    QVERIFY(firstBlockItem->isVisible());
    QCOMPARE(firstBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstBlockItem->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(firstBlockItem->getDisplayLastAddress(), quint64(0));

    QVERIFY(secondBlockItem->isVisible());
    QCOMPARE(secondBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondBlockItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(secondBlockItem->getDisplayLastAddress(), quint64(0));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testMultipleBlocksOverlapping()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testMultipleBlocksOverlapping()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setName("testMap");
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> largeBlock = createAddressBlock("largeAddressBlock", "0", "4", 8);
    QSharedPointer<AddressBlock> firstOverlappingBlock = createAddressBlock("firstOverlap", "1", "1", 8);
    QSharedPointer<AddressBlock> seconOverlappingBlock = createAddressBlock("secondOverlap", "3", "1", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(largeBlock);
    addressBlocks.append(firstOverlappingBlock);
    addressBlocks.append(seconOverlappingBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* largeAddressBlock = new AddressBlockGraphItem(largeBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* firstOverlap = new AddressBlockGraphItem(firstOverlappingBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* secondOverlap = new AddressBlockGraphItem(seconOverlappingBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(largeAddressBlock);
    memoryMapItem->addChild(firstOverlap);
    memoryMapItem->addChild(secondOverlap);

    largeAddressBlock->refresh();
    firstOverlap->refresh();
    secondOverlap->refresh();

    expandItem(memoryMapItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(memoryMapItem);
    QCOMPARE(conflictedAreas.count(), 0);

    QVERIFY(largeAddressBlock->isVisible());
    QVERIFY(largeAddressBlock->isConflicted());
    QCOMPARE(largeAddressBlock->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(largeAddressBlock->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(largeAddressBlock->getDisplayOffset(), quint64(0));
    QCOMPARE(largeAddressBlock->getDisplayLastAddress(), quint64(3));

    QVERIFY(firstOverlap->isVisible());  
    QVERIFY(largeAddressBlock->isConflicted());
    QCOMPARE(firstOverlap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstOverlap->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstOverlap->getDisplayOffset(), quint64(1));
    QCOMPARE(firstOverlap->getDisplayLastAddress(), quint64(1));

    QVERIFY(secondOverlap->isVisible());
    QVERIFY(secondOverlap->isConflicted());
    QCOMPARE(secondOverlap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondOverlap->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondOverlap->getDisplayOffset(), quint64(3));
    QCOMPARE(secondOverlap->getDisplayLastAddress(), quint64(3));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testTwoBlocksCompletelyOverlappingThird()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testTwoBlocksCompletelyOverlappingThird()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setName("testMap");
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> firstBlock = createAddressBlock("firstAddressBlock", "0", "2", 8);
    QSharedPointer<AddressBlock> overlappedBlock = createAddressBlock("overlappedBlock", "1", "2", 8);
    QSharedPointer<AddressBlock> lastBlock = createAddressBlock("secondAddressBlock", "2", "2", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(firstBlock);
    addressBlocks.append(overlappedBlock);
    addressBlocks.append(lastBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* firstBlockItem = new AddressBlockGraphItem(firstBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* overlappedItem = new AddressBlockGraphItem(overlappedBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* lastBlockItem = new AddressBlockGraphItem(lastBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(firstBlockItem);
    memoryMapItem->addChild(lastBlockItem);
    memoryMapItem->addChild(overlappedItem);

    firstBlockItem->refresh();
    lastBlockItem->refresh();
    overlappedItem->refresh();

    expandItem(memoryMapItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(memoryMapItem);
    QCOMPARE(conflictedAreas.count(), 2);

    QVERIFY(firstBlockItem->isConflicted());
    QCOMPARE(firstBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstBlockItem->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstBlockItem->getDisplayOffset(), quint64(0));
    QCOMPARE(firstBlockItem->getDisplayLastAddress(), quint64(0));

    MemoryGapItem* firstConflict = conflictedAreas.first();
    QVERIFY(firstConflict->isConflicted());
    QCOMPARE(firstConflict->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstConflict->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstConflict->getDisplayOffset(), quint64(1));
    QCOMPARE(firstConflict->getDisplayLastAddress(), quint64(1));

    QVERIFY(overlappedItem->isConflicted());
    QVERIFY(overlappedItem->isCompletelyOverlapped());
    QCOMPARE(overlappedItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(overlappedItem->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));

    MemoryGapItem* secondConflict = conflictedAreas.last();
    QVERIFY(secondConflict->isConflicted());
    QCOMPARE(secondConflict->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondConflict->pos().y(), qreal(4*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondConflict->getDisplayOffset(), quint64(2));
    QCOMPARE(secondConflict->getDisplayLastAddress(), quint64(2));
 
    QCOMPARE(lastBlockItem->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(lastBlockItem->pos().y(), qreal(5*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(lastBlockItem->getDisplayOffset(), quint64(3));
    QCOMPARE(lastBlockItem->getDisplayLastAddress(), quint64(3));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testTwoOverlappingBlocksInsideThrid()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testTwoOverlappingBlocksInsideThrid()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setName("testMap");
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> largeAddressBlock = createAddressBlock("containerBlock", "0", "4", 8);
    QSharedPointer<AddressBlock> firstOverlappingBlock = createAddressBlock("firstOverlap", "1", "2", 8);
    QSharedPointer<AddressBlock> secondOverlappingBlock = createAddressBlock("secondOverlap", "2", "2", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(largeAddressBlock);
    addressBlocks.append(firstOverlappingBlock);
    addressBlocks.append(secondOverlappingBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* containerBlock = new AddressBlockGraphItem(largeAddressBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* firstOverlap = new AddressBlockGraphItem(firstOverlappingBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* secondOverlap = new AddressBlockGraphItem(secondOverlappingBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(containerBlock);
    memoryMapItem->addChild(firstOverlap);
    memoryMapItem->addChild(secondOverlap);

    containerBlock->refresh();
    firstOverlap->refresh();
    secondOverlap->refresh();

    expandItem(memoryMapItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(memoryMapItem);
    QCOMPARE(conflictedAreas.count(), 1);

    QVERIFY(containerBlock->isConflicted());
    QCOMPARE(containerBlock->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(containerBlock->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(containerBlock->getDisplayOffset(), quint64(0));
    QCOMPARE(containerBlock->getDisplayLastAddress(), quint64(3));

    QVERIFY(firstOverlap->isConflicted());
    QCOMPARE(firstOverlap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstOverlap->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));

    MemoryGapItem* overlappingArea = conflictedAreas.first();
    QVERIFY(overlappingArea->isConflicted());
    QCOMPARE(overlappingArea->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(overlappingArea->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(overlappingArea->getDisplayOffset(), quint64(2));
    QCOMPARE(overlappingArea->getDisplayLastAddress(), quint64(2));

    QVERIFY(secondOverlap->isConflicted());    
    QCOMPARE(secondOverlap->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondOverlap->pos().y(), qreal(4*VisualizerItem::DEFAULT_HEIGHT));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testIdenticalBlocksOverlappingThird()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testIdenticalBlocksOverlappingThird()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> uniqueBlock = createAddressBlock("unique", "0", "2", 8);
    QSharedPointer<AddressBlock> twin1 = createAddressBlock("firstTwin", "1", "2", 8);
    QSharedPointer<AddressBlock> twin2 = createAddressBlock("secondTwin", "1", "2", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(uniqueBlock);
    addressBlocks.append(twin1);
    addressBlocks.append(twin2);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* unique = new AddressBlockGraphItem(uniqueBlock, noParser, memoryMapItem);
    unique->setAddressableUnitBits(8);

    AddressBlockGraphItem* firstTwin = new AddressBlockGraphItem(twin1, noParser, memoryMapItem);
    firstTwin->setAddressableUnitBits(8);

    AddressBlockGraphItem* secondTwin = new AddressBlockGraphItem(twin2, noParser, memoryMapItem);
    secondTwin->setAddressableUnitBits(8);

    memoryMapItem->addChild(unique);
    memoryMapItem->addChild(firstTwin);
    memoryMapItem->addChild(secondTwin);

    unique->refresh();
    firstTwin->refresh();
    secondTwin->refresh();

    expandItem(memoryMapItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(memoryMapItem);
    QCOMPARE(conflictedAreas.count(), 1);

    QVERIFY(unique->isConflicted());
    QCOMPARE(unique->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(unique->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(unique->getDisplayOffset(), quint64(0));
    QCOMPARE(unique->getDisplayLastAddress(), quint64(0));

    MemoryGapItem* overlappingArea = conflictedAreas.first();
    QVERIFY(overlappingArea->isConflicted());
    QCOMPARE(overlappingArea->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(overlappingArea->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(overlappingArea->getDisplayOffset(), quint64(1));
    QCOMPARE(overlappingArea->getDisplayLastAddress(), quint64(1));

    QVERIFY(firstTwin->isConflicted());
    QCOMPARE(firstTwin->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstTwin->pos().y(), qreal(4*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstTwin->getDisplayOffset(), quint64(2));
    QCOMPARE(firstTwin->getDisplayLastAddress(), quint64(2));

    QVERIFY(secondTwin->isConflicted());    
    QCOMPARE(secondTwin->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondTwin->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondTwin->getDisplayOffset(), quint64(2));
    QCOMPARE(secondTwin->getDisplayLastAddress(), quint64(2));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testConsecutiveBlocksInsideThird()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testConsecutiveBlocksInsideThird()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> containerBlock = createAddressBlock("container", "0", "5", 8);
    QSharedPointer<AddressBlock> firstInsideBlock = createAddressBlock("firstInsider", "1", "2", 8);
    QSharedPointer<AddressBlock> secondInsideBlock = createAddressBlock("secondInsider", "3", "2", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(containerBlock);
    addressBlocks.append(firstInsideBlock);
    addressBlocks.append(secondInsideBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    MemoryMapGraphItem* memoryMapItem = createMemoryBlockBase(memoryMap);

    QSharedPointer<ExpressionParser> noParser(new NullParser());

    AddressBlockGraphItem* container = new AddressBlockGraphItem(containerBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* firstInsider = new AddressBlockGraphItem(firstInsideBlock, noParser, memoryMapItem);
    AddressBlockGraphItem* secondInsider = new AddressBlockGraphItem(secondInsideBlock, noParser, memoryMapItem);
    memoryMapItem->addChild(container);
    memoryMapItem->addChild(firstInsider);
    memoryMapItem->addChild(secondInsider);

    container->refresh();
    firstInsider->refresh();
    secondInsider->refresh();

    expandItem(memoryMapItem);

    QList<MemoryGapItem*> conflictedAreas = findMemoryGaps(memoryMapItem);
    QCOMPARE(conflictedAreas.count(), 0);

    QVERIFY(container->isConflicted());
    QCOMPARE(container->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(container->pos().y(), qreal(VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(container->getDisplayOffset(), quint64(0));
    QCOMPARE(container->getDisplayLastAddress(), quint64(4));

    QVERIFY(firstInsider->isConflicted());
    QCOMPARE(firstInsider->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(firstInsider->pos().y(), qreal(2*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(firstInsider->getDisplayOffset(), quint64(1));
    QCOMPARE(firstInsider->getDisplayLastAddress(), quint64(2));

    QVERIFY(secondInsider->isConflicted());    
    QCOMPARE(secondInsider->pos().x(), qreal(MemoryVisualizationItem::CHILD_INDENTATION));
    QCOMPARE(secondInsider->pos().y(), qreal(3*VisualizerItem::DEFAULT_HEIGHT));
    QCOMPARE(secondInsider->getDisplayOffset(), quint64(3));
    QCOMPARE(secondInsider->getDisplayLastAddress(), quint64(4));

    delete memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::testExpressions()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::testExpressions()
{
    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    memoryMap->setAddressUnitBits("8");

    QSharedPointer<AddressBlock> parametrizedBlock = createAddressBlock("container", "'h2", "'h2+'h2", 8);

    QList<QSharedPointer<MemoryBlockBase> > addressBlocks;
    addressBlocks.append(parametrizedBlock);
    memoryMap->getMemoryBlocks()->append(addressBlocks);

    QSharedPointer<ExpressionParser> expressionParser(new SystemVerilogExpressionParser());

    MemoryMapGraphItem* memoryMapItem = new MemoryMapGraphItem(memoryMap, memoryMap, expressionParser, 0);

    AddressBlockGraphItem* parametrizedItem = new AddressBlockGraphItem(parametrizedBlock, expressionParser, memoryMapItem);
    memoryMapItem->addChild(parametrizedItem);
    memoryMapItem->refresh();

    QCOMPARE(memoryMapItem->getDisplayOffset(), quint64(2));
    QCOMPARE(memoryMapItem->getDisplayLastAddress(), quint64(5));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::createMemoryBlockBase()
//-----------------------------------------------------------------------------
MemoryMapGraphItem* tst_MemoryMapGraphItem::createMemoryBlockBase(QSharedPointer<MemoryMap> memoryMap)
{
    QSharedPointer<ExpressionParser> expressionParser(new SystemVerilogExpressionParser());
    MemoryMapGraphItem* memoryMapItem = new MemoryMapGraphItem(memoryMap, memoryMap, expressionParser, 0);
    memoryMapItem->refresh();

    return memoryMapItem;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::createAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> tst_MemoryMapGraphItem::createAddressBlock(QString const& name, 
    QString const& baseAddress, QString const& range, int bitWidth)
{
    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    addressBlock->setName(name);
    addressBlock->setBaseAddress(baseAddress);
    addressBlock->setRange(range);
    addressBlock->setWidth(QString::number(bitWidth));

    return addressBlock;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::expandItem()
//-----------------------------------------------------------------------------
void tst_MemoryMapGraphItem::expandItem(MemoryMapGraphItem* memoryMapItem)
{
    GraphicsExpandCollapseItem* expander(0);
    foreach (QGraphicsItem* item, memoryMapItem->childItems())
    {
        if (dynamic_cast<GraphicsExpandCollapseItem*>(item) != 0)
        {
            expander = dynamic_cast<GraphicsExpandCollapseItem*>(item);
            expander->expand();
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapGraphItem::findMemoryGap()
//-----------------------------------------------------------------------------
QList<MemoryGapItem*> tst_MemoryMapGraphItem::findMemoryGaps(MemoryMapGraphItem* memoryMapItem)
{
    QList<MemoryGapItem*> gaps;
    foreach (QGraphicsItem* child, memoryMapItem->childItems())
    {
        if (dynamic_cast<MemoryGapItem*>(child))
        {
            gaps.append(dynamic_cast<MemoryGapItem*>(child));
        }
    }

    return gaps;
}

QTEST_MAIN(tst_MemoryMapGraphItem)

#include "tst_MemoryMapGraphItem.moc"
