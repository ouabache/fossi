/* 
 *	Created on:	28.3.2013
 *	Author:		Antti Kamppi
 *	File name:	viewselector.h
 *	Project:		Kactus 2
*/

#ifndef VIEWSELECTOR_H
#define VIEWSELECTOR_H

#include <IPXACTmodels/Component/Component.h>
#include <common/Global.h>

#include <QComboBox>
#include <QSharedPointer>

/*! \brief This combo box can be used to select a view within a component.
 *
 */
class KACTUS2_API ViewSelector : public QComboBox {
	Q_OBJECT

public:

	//! \brief Specifies what kind of views can be selected in the combo box.
	enum ViewType {
		FLAT_HW_VIEWS = 0,
		HIER_HW_VIEWS,
		BOTH_HW_VIEWS,
		SW_VIEWS,
		SYSTEM_VIEWS,
		MODE_COUNT
	};

	/*! \brief The constructor
	 *
	 * Method: 		ViewSelector
	 * Full name:	ViewSelector::ViewSelector
	 * Access:		private 
	 *
	 * \param mode Specifies which kind of views can be selected.
	 * \param component Pointer to the component which's views are being selected.
	 * \param parent Pointer to the owner of the combo box.
	 * \param allowEmpty If true then empty option is added to allow unselecting a view.
	 *
	*/
	ViewSelector(ViewType mode, QSharedPointer<Component> component, QWidget *parent, bool allowEmpty = true);
	
	//! \brief The destructor.
	virtual ~ViewSelector();

public slots:

	//! \brief Refresh the items to be selected in the combo box.
	void refresh();

	//! \brief Select the named view in the combo box.
	void selectView(const QString& viewName);

signals:

	//! \brief Emitted when a view changes.
	void viewSelected(const QString& viewName);

private slots:

		//! \brief Handler for selected index changes.
		void onIndexChange(int newIndex);

private:
	
	//! \brief No copying
	ViewSelector(const ViewSelector& other);

	//! \brief No assignment
	ViewSelector& operator=(const ViewSelector& other);

	//! \brief Specifies what kind of views can be selected in the combo box.
	ViewType mode_;

	//! \brief Pointer to the component, which's views are selected.
	QSharedPointer<Component> component_;

	//! \brief Specified whether empty option is available in the combo box.
	bool allowEmpty_;
};

#endif // VIEWSELECTOR_H
