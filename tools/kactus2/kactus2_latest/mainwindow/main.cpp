//-----------------------------------------------------------------------------
// File: main.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 24.5.2011
//
// Description:
// Kactus2 main entry point.
//-----------------------------------------------------------------------------

#include "mainwindow.h"
#include "SplashScreen.h"

#include <VersionHelper.h>

#include <QApplication>
#include <QPalette>
#include <QTimer>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(kactus);
    QApplication application(argc, argv);

    // Set the palette to use nice pastel colors.
    QPalette palette = application.palette();
    palette.setColor(QPalette::Active, QPalette::Highlight, QColor(33, 135, 237));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(166, 200, 234));
    palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(166, 200, 234));

    application.setPalette(palette);

	// Create the main window and close the splash after 1.5 seconds.
	MainWindow w;
		
	// the release mode
	#ifdef NDEBUG
	
	// Show the splash screen.
	SplashScreen splash(VersionHelper::createVersionString());
    splash.show();
    splash.showMessage("");

	application.processEvents();

	QTimer::singleShot(1500, &splash, SLOT(close()));
	QTimer::singleShot(1500, &w, SLOT(show()));
	QTimer::singleShot(1700, &w, SLOT(onLibrarySearch()));

	// the debug mode
	#else
    QTimer::singleShot(200, &w, SLOT(onLibrarySearch()));
	w.show();    
	#endif    
    
	return application.exec();
}
