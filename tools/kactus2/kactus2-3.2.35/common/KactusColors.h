//-----------------------------------------------------------------------------
// File: KactusColors.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 6.6.2012
//
// Description:
// Common colors used in Kactus2.
//-----------------------------------------------------------------------------

#ifndef KACTUSCOLORS_H
#define KACTUSCOLORS_H

#include <QColor>

namespace KactusColors
{
    //! Color used for selections in design diagrams.
    QColor const DIAGRAM_SELECTION = Qt::blue;

    //! Color used for broken/invalid connections in design diagrams.
    QColor const BROKEN_CONNECTION = Qt::red;

    //! Special color used for COM connections.
    QColor const COM_CONNECTION = QColor(49, 132, 61);

    //! Color used for allowed visualization in design diagrams.
    QColor const DIAGRAM_ALLOWED_INTERFACE = Qt::magenta;

    //! Colors used for component.
    QColor const MISSING_COMPONENT = QColor(0xe8, 0xc5, 0xc5);
    QColor const DRAFT_COMPONENT = QColor(217, 217, 217);
    QColor const HW_BUS_COMPONENT = QColor(0xce,0xdf,0xff);
    QColor const HW_COMPONENT = QColor(0xa5,0xc3,0xef);
    QColor const SW_COMPONENT = QColor(0xc5,0xff, 0xab);

    //! Colors used for interfaces in design.
    QColor const MASTER_INTERFACE(0x32,0xcb,0xcb);
    QColor const SLAVE_INTERFACE(0x32,0x99,0x64);
    QColor const MIRROREDMASTER_INTERFACE(0xcb,0xfd,0xfd);
    QColor const MIRROREDSLAVE_INTERFACE(0x00,0xfd,00);
    QColor const SYSTEM_INTERFACE(0xf9,0x11,0x11);
    QColor const MIRROREDSYSTEM_INTERFACE(0xf9,0x9d,0xcb);
    QColor const MONITOR_INTERFACE(0xfd,0xfd,0xfd);

    //! Colors used for memory/address sections in the memory designer.
    QColor const MEMORY_BLOCK = QColor(0xd6, 0xba, 0xff);
    QColor const ADDRESS_SEGMENT = QColor(0xc6,0xef,0xd7);
    QColor const ADDRESS_SEGMENT_UNSEGMENTED = QColor(217, 217, 217);

    QColor const MEM_MAP_COLOR =  HW_COMPONENT;
    QColor const ADDR_BLOCK_COLOR = QColor(194, 214, 244);
    QColor const REGISTER_COLOR = QColor(216, 229, 248);
    QColor const FIELD_COLOR = QColor(235, 241, 251);

}

//-----------------------------------------------------------------------------

#endif // KACTUSCOLORS_H
