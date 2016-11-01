//-----------------------------------------------------------------------------
// File: QuartusProjectGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.06.2015
//
// Description:
// The plugin for generating quartus projects containing quartus project files and quartus settings files.
//-----------------------------------------------------------------------------

#ifndef QUARTUSPROJECTGENERATOR_H
#define QUARTUSPROJECTGENERATOR_H

#include "QuartusProjectGenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

#include <IPXACTmodels/Component/Component.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QIcon>
#include <QFileInfo>
#include <QSettings>

class IPluginUtility;
class Design;
class DesignConfiguration;

//-----------------------------------------------------------------------------
// !The plugin for generating quartus projects containing quartus project files and quartus settings files.
//-----------------------------------------------------------------------------
class QUARTUSPROJECTGENERATOR_EXPORT QuartusProjectGenerator : public QObject, public IGeneratorPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.QuartusProjectGenerator" FILE "QuartusProjectGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

    /*!
	 *  The constructor.
	 */
	QuartusProjectGenerator();
	
	/*!
	 *  The destructor.
	 */
	virtual ~QuartusProjectGenerator();

    /*!
     *  Gets the name of the plugin.
     *
     *      @return The name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Gets the version of the plugin.
     *
     *      @return The version of the plugin
     */
    virtual QString getVersion() const;

    /*!
     *  Gets the description of the plugin.
     *
     *      @return The description of the plugin.
     */
    virtual QString getDescription() const;

    /*!
     *  Gets the vendor of the plugin.
     *
     *      @return The vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Gets the license of the plugin.
     *
     *      @return The license of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Returns the holder of the license of the plugin.
     *
     *      @return The holder of the license of the plugin.
     */
    virtual QString getLicenceHolder() const;

    /*!
     *  Returns the settings widget.
     *
     *      @return The settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     *
     *      @return The icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp     The library component for which to check support.
     *      @param [in] libDesConf  The design configuration if the generator is run on a hierarchical component.
     *      @param [in] libDes      The design if the generator is run on a hierarchical component.
     *
     *      @return True, if the generator supports the given component, otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Document const> libComp,
        QSharedPointer<Document const> libDesConf = QSharedPointer<Document const>(),
        QSharedPointer<Document const> libDes = QSharedPointer<Document const>()) const;

    /*!
     *  Runs the generator.
     *
     *      @param [in] utility     The plugin utility interface.
     *      @param [in] libComp     The component for which the generator is run.
     *      @param [in] libDesConf  The design configuration if the generator is run on a hierarchical component.
     *      @param [in] libDes      The design if the generator is run on a hierarchical component.
     */
    virtual void runGenerator(IPluginUtility* utility, QSharedPointer<Document> libComp,
        QSharedPointer<Document> libDesConf = QSharedPointer<Document>(),
        QSharedPointer<Document> libDes = QSharedPointer<Document>());

    /*!
     *  Get a list of external program requirements of the plugin.
     *
     *      @return A list of external program requirements of the plugin.
     */
    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private slots:

    /*!
     *  Handles the error messages of the quartus generator.
     *
     *      @param [in] message     The error message.
     */
    void onErrorMessage(QString const& message);

    /*!
     *  Handles the notification messages of the quartus generator.
     *
     *      @param [in] message     The notification message.
     */
    void onNoticeMessage(QString const& message);

private:

	//! No copying
	QuartusProjectGenerator(const QuartusProjectGenerator& other);

	//! No assignment
	QuartusProjectGenerator& operator=(const QuartusProjectGenerator& other);

    /*!
     *  Gets the view of the current design or design configuration.
	 *
	 *      @param [in] component   The component for which this plugin is run for.
     *      @param [in] desConf		The design configuration object of a hierarchical component.
     *      @param [in] design      The design object of a hierarchical component.
     *
     *      @return The view referencing to the open design.
     */
	QSharedPointer<View> getOpenView(QSharedPointer<Component> component,
		QSharedPointer<DesignConfiguration> desConf, QSharedPointer<Design> design);

	//! The plugin utility.
	IPluginUtility* utility_;
};

#endif // QUARTUSPROJECTGENERATOR_H
