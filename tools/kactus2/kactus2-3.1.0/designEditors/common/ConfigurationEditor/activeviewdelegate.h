/* 
 *  	Created on: 26.8.2011
 *      Author: Antti Kamppi
 * 		filename: activeviewdelegate.h
 *		Project: Kactus 2
 */

#ifndef ACTIVEVIEWDELEGATE_H
#define ACTIVEVIEWDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief Delegate the provides editors to edit the active view of a component instance.
 * 
 */
class ActiveViewDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ActiveViewDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~ActiveViewDelegate();

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
	ActiveViewDelegate(const ActiveViewDelegate& other);

	//! \brief No assignment
	ActiveViewDelegate& operator=(const ActiveViewDelegate& other);
};

#endif // ACTIVEVIEWDELEGATE_H
