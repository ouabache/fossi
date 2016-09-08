/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * File name:	alterabspgenerator.h
 * Project:	Kactus 2
*/

#ifndef ALTERABSPGENERATOR_H
#define ALTERABSPGENERATOR_H

#include "alterabspgenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QFileInfo>

class IPluginUtility;

/*! \brief The plugin to generate C-headers of component memory maps.
 *
 */
class ALTERABSPGENERATOR_EXPORT AlteraBSPGenerator : public QObject, public IGeneratorPlugin {

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.AlteraBSPGenerator" FILE "alteraBSPGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

	/*! \brief The constructor
	 *
	 * Method: 		AlteraBSPGenerator
	 * Full name:	AlteraBSPGenerator::AlteraBSPGenerator
	 * Access:		public 
	 *
	*/
	AlteraBSPGenerator();
	
	//! \brief The destructor
	virtual ~AlteraBSPGenerator();

	//! \brief Returns the name of the plugin.
    virtual QString getName() const;

	//! \brief Returns the version of the plugin.
    virtual QString getVersion() const;

	//! \brief Returns the description of the plugin.
    virtual QString getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString getLicenceHolder() const;

	//! \brief Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*! \brief Checks whether the generator supports generation for the given library component.
     *
     * Method: 		checkGeneratorSupport
     * Full name:	MemoryMapHeaderGenerator::checkGeneratorSupport
     * Access:		virtual public 
     *
     * \param libComp The library component for which to check support.
     * \param libDesConf The design configuration object if the generator is run on a hierarchical component.
     * \param libDes The design object if the generator is run on a hierarchical component.
     *
     * \return True, if the generator supports the given component. Otherwise false.
    */
	 virtual bool checkGeneratorSupport(QSharedPointer<Document const> libComp,
		 QSharedPointer<Document const> libDesConf = QSharedPointer<Document const>(),
		 QSharedPointer<Document const> libDes = QSharedPointer<Document const>()) const;

    /*! \brief Runs the generator.
     *
     * Method: 		runGenerator
     * Full name:	MemoryMapHeaderGenerator::runGenerator
     * Access:		virtual public 
     *
     * \param utility The plugin utility interface.
     * \param libComp The component for which the generator is run.
     * \param libDesConf The design configuration object if the generator is run on a hierarchical component.
     * \param libDes The design object if the generator is run on a hierarchical component.
     *
    */
	 virtual void runGenerator(IPluginUtility* utility, 
		 QSharedPointer<Document> libComp, 
		 QSharedPointer<Document> libDesConf = QSharedPointer<Document>(), 
		 QSharedPointer<Document> libDes = QSharedPointer<Document>());

	 //! \brief Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private:

	//! \brief No copying
	AlteraBSPGenerator(const AlteraBSPGenerator& other);

	//! \brief No assignment
	AlteraBSPGenerator& operator=(const AlteraBSPGenerator& other);

	/*! \brief Parse the entries in the file system and add them to specified file set.
	 *
	 * Method: 		addEntry
	 * Full name:	AlteraBSPGenerator::addEntry
	 * Access:		private 
	 *
	 * \param entry The entry to add.
	 * \param xmlPath Path to the top component xml file.
	 * \param fileSet Pointer to the file set to add the entries to.
	 * \param settings Contains the file type settings.
	 *
	*/
	void addEntry(const QFileInfo &entry, 
		const QString& xmlPath,
		QSharedPointer<FileSet> fileSet,
		QSettings& settings);

	//! The plugin utility.
	IPluginUtility* utility_;
};

#endif // ALTERABSPGENERATOR_H
