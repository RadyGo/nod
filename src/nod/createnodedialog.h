#ifndef NOD_CREATENODEDIALOG_H
#define NOD_CREATENODEDIALOG_H

// ----------------------------------------------------------------------------

#include <QDialog>
#include <QTreeWidgetItem>

// ----------------------------------------------------------------------------

namespace Ui {
class DefaultCreateNodeDialog;
}

// ----------------------------------------------------------------------------

#include "nod/nodefactory.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class CreateNodeDialog : public QDialog
{
    Q_OBJECT
public:
    using QDialog::QDialog;

    virtual void        setNodeType(const NodeTypeID &type)=0;

    virtual NodeTypeID  nodeType() const=0;
};

// ----------------------------------------------------------------------------

class DefaultCreateNodeDialog : public CreateNodeDialog
{
    Q_OBJECT
public:
    explicit DefaultCreateNodeDialog(NodeFactory &factory, QWidget *parent = 0);
    ~DefaultCreateNodeDialog();

    void                setNodeType(const NodeTypeID &type);

    NodeTypeID          nodeType() const;

protected slots:

    virtual void        itemDoubleClicked(QTreeWidgetItem *item, int column);

private:

    Ui::DefaultCreateNodeDialog *ui;

    NodeFactory         &mFactory;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_CREATENODEDIALOG_H

// ----------------------------------------------------------------------------
