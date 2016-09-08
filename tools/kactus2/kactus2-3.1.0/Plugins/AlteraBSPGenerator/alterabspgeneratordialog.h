/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	alterabspgeneratordialog.h
 *	Project:		Kactus 2
*/

#ifndef ALTERABSPGENERATORDIALOG_H
#define ALTERABSPGENERATORDIALOG_H

#include <commandLineGeneratorDialog/commandlinegeneratordialog.h>
#include <common/widgets/viewSelector/viewselector.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>

class LibraryInterface;

/*! \brief The dialog to generate the BSP for a CPU component.
 *
 */
class AlteraBSPGeneratorDialog : public CommandLineGeneratorDialog {
	Q_OBJECT

public:

	//! \brief The indexes for variables
	enum VariableIndex {
		WIN_PATH = 0,
		LINUX_PATH,
		VARIABLE_COUNT
	};

	//! \brief Defines the names for the required variables.
	static const QString VARIABLE_NAMES[VARIABLE_COUNT];

	//! \brief Used to remember where and to which sw view the generator is run.
	struct GenerationOptions {

		//! \brief The path to the directory used as base for the generator.
		QString dirPath_;

		//! \brief The name of the SW view the generator is run for.
		QString swViewName_;

		//! \brief The constructor.
		GenerationOptions(const QString& path, const QString& view);
	};

	//! \brief The name of the script to run when running the generator.
	static const QString PROCESS_START_COMMAND;

	/*! \brief The constructor.
	 *
	 * Method: 		AlteraBSPGeneratorDialog
	 * Full name:	AlteraBSPGeneratorDialog::AlteraBSPGeneratorDialog
	 * Access:		private 
	 *
	 * \param component Pointer to the component to generate BSP for.
	 * \param parent Pointer to the owner of the dialog.
	 *
	*/
	AlteraBSPGeneratorDialog(LibraryInterface* handler, 
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor.
	virtual ~AlteraBSPGeneratorDialog();

	/*! \brief Get list of the directories where the generator was run.
	 *
	 * Method: 		getCreatedDirs
	 * Full name:	AlteraBSPGeneratorDialog::getCreatedDirs
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the absolute paths to the directories.
	*/
	const QList<GenerationOptions>& getCreatedDirs() const;

protected slots:

		//! \brief Handler for clicks on run button.
		virtual void onRunClicked();

private slots:

	//! \brief Handler for changes in SW view selector.
	void onViewChange(const QString& viewName);

	//! \brief Handler for clicks on output dir button.
	void onSelectOutput();

private:
	
	//! \brief No copying
	AlteraBSPGeneratorDialog(const AlteraBSPGeneratorDialog& other);

	//! \brief No assignment
	AlteraBSPGeneratorDialog& operator=(const AlteraBSPGeneratorDialog& other);

	//! \brief Update the command to generate the BSP.
	void updateCommand();

	//! \brief Run the generator for windows.
	void runWindowsCommands();

	//! \brief Run the generator for Linux-based systems.
	void runOtherCommands();

	//! \brief Runs the cygwin-commands which are common for all systems.
	void runCygwinCommands();

	//! \brief The instance which manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component to generate the BSP for.
	QSharedPointer<Component> component_;

	//! \brief Combo box to select a SW view of the component.
	ViewSelector* viewSelector_;

	//! \brief The button for user to select the output directory.
	QPushButton* dirButton_;

	//! \brief Displays the command to be run.
	QLabel* commandLabel_;

	//! \brief Displays the output directory
	QLabel* outPutDirLabel_;

	//! \brief Displays the source file path
	QLabel* sourceLabel_;

	//! \brief Displays the arguments for the command.
	QLabel* argLabel_;

	//! \brief The path to the directory to generate the BSP to.
	QString targetDir_;

	//! \brief The currently selected SW view.
	QSharedPointer<SWView> currentView_;

	//! \brief Contains the paths to directories where the generator is run.
	QList<GenerationOptions> generatedPaths_;
};

#endif // ALTERABSPGENERATORDIALOG_H
