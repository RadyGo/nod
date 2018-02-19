
// ----------------------------------------------------------------------------

#include "nod/createnodedialog.h"
#include "nod/nodefactory.h"

// ----------------------------------------------------------------------------

#include "ui_defaultcreatenodedialog.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

DefaultCreateNodeDialog::DefaultCreateNodeDialog(NodeFactory &factory, QWidget *parent)
    : CreateNodeDialog(parent),
    ui(new Ui::DefaultCreateNodeDialog),
    mFactory(factory)
{
    ui->setupUi(this);

    connect(ui->buttons, &QDialogButtonBox::accepted, this, &DefaultCreateNodeDialog::accept);
    connect(ui->buttons, &QDialogButtonBox::rejected, this, &DefaultCreateNodeDialog::reject);

    connect(ui->nodes, &QTreeWidget::itemDoubleClicked, this, &DefaultCreateNodeDialog::itemDoubleClicked);

    for (int i=0; i<factory.typeCount(); ++i)
    {
        auto type = factory.type(i);

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0, Qt::UserRole + 1, type.id.value);
        item->setData(0, Qt::DecorationRole, type.icon);
        item->setData(0, Qt::DisplayRole, type.name);
        item->setData(0, Qt::ToolTipRole, type.description);
        ui->nodes->addTopLevelItem(item);
    }
}

// ----------------------------------------------------------------------------

DefaultCreateNodeDialog::~DefaultCreateNodeDialog()
{
    delete ui;
}

// ----------------------------------------------------------------------------

void DefaultCreateNodeDialog::setNodeType(const NodeTypeID &type)
{
    for (int i=0; i<ui->nodes->topLevelItemCount(); ++i)
    {
        auto item = ui->nodes->topLevelItem(i);
        if (item->data(0, Qt::UserRole + 1 ).toUuid() == type.value)
        {
            ui->nodes->setCurrentItem(item);
            break;
        }
    }
}

// ----------------------------------------------------------------------------
 NodeTypeID DefaultCreateNodeDialog::nodeType() const
{
    auto item = ui->nodes->currentItem();
    if (!item)
        return NodeTypeID::invalid();

    auto uuid = item->data(0, Qt::UserRole + 1).toUuid();
    return { uuid, { 0 } };
}

// ----------------------------------------------------------------------------

void DefaultCreateNodeDialog::itemDoubleClicked(QTreeWidgetItem *, int)
{
    accept();
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
