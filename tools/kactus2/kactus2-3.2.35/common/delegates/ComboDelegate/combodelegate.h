/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: combodelegate.h
 *		Project: Kactus 2
 */

#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H

#include <QStyledItemDelegate>

/*! \brief ComboDelegate is a base class to provide combo editors for widgets.
 * 
 * This can be sub classed to provide more specific combo widgets.
 * In subclasses the createEditor() function can be reimplemented to set the 
 * selectable items in the combo box.
 */
class ComboDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The minimum height set for the editor.
	static const int MINIMUM_EDITOR_HEIGHT = 20;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ComboDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~ComboDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private:
	//! \brief No copying
	ComboDelegate(const ComboDelegate& other);

	//! \brief No assignment
	ComboDelegate& operator=(const ComboDelegate& other);
};

#endif // COMBODELEGATE_H
