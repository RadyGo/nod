
// ----------------------------------------------------------------------------

#include <QButtonGroup>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVariant>
#include <QVBoxLayout>

// ----------------------------------------------------------------------------

#include "nod/aligndialog.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

AlignDialog::AlignDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Align Nodes"));

    QVBoxLayout *vlayout = new QVBoxLayout(this);

    QButtonGroup *group = new QButtonGroup(this);

    QGridLayout *grid = new QGridLayout(this);
    vlayout->addLayout(grid);

    QCheckBox *tl = new QCheckBox(QObject::tr("Top Left"), this);
    tl->setProperty("align", int(Qt::AlignLeft | Qt::AlignTop));
    grid->addWidget(tl, 0, 0);
    group->addButton(tl);

    QCheckBox *t = new QCheckBox(QObject::tr("Top"), this);
    t->setProperty("align", int(Qt::AlignHCenter | Qt::AlignTop));
    grid->addWidget(t, 0, 1);
    group->addButton(t);

    QCheckBox *tr = new QCheckBox(QObject::tr("Top Right"), this);
    tr->setProperty("align", int(Qt::AlignRight | Qt::AlignTop));
    grid->addWidget(tr, 0, 2);
    group->addButton(tr);

    QCheckBox *l = new QCheckBox(QObject::tr("Left"), this);
    l->setProperty("align", int(Qt::AlignLeft | Qt::AlignVCenter));
    grid->addWidget(l, 1, 0);
    group->addButton(l);

    QCheckBox *c = new QCheckBox(QObject::tr("Center"), this);
    c->setProperty("align", int(Qt::AlignHCenter | Qt::AlignVCenter));
    grid->addWidget(c, 1, 1);
    group->addButton(c);

    QCheckBox *r = new QCheckBox(QObject::tr("Right"), this);
    r->setProperty("align", int(Qt::AlignRight | Qt::AlignVCenter));
    grid->addWidget(r, 1, 2);
    group->addButton(r);

    QCheckBox *bl = new QCheckBox(QObject::tr("Bottom Left"), this);
    bl->setProperty("align", int(Qt::AlignLeft | Qt::AlignBottom));
    grid->addWidget(bl, 2, 0);
    group->addButton(bl);

    QCheckBox *b = new QCheckBox(QObject::tr("Bottom"), this);
    b->setProperty("align", int(Qt::AlignHCenter | Qt::AlignBottom));
    grid->addWidget(b, 2, 1);
    group->addButton(b);

    QCheckBox *br = new QCheckBox(QObject::tr("Bottom Right"), this);
    br->setProperty("align", int(Qt::AlignRight | Qt::AlignBottom));
    grid->addWidget(br, 2, 2);
    group->addButton(br);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    vlayout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &AlignDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &AlignDialog::reject);

    setLayout(vlayout);
}

// ----------------------------------------------------------------------------

void DefaultAlignDialog::accept()
{
    auto items = findChildren<QCheckBox*>();
    for (auto item : items)
    {
        if (item->isChecked())
        {
            mAlignment = Qt::Alignment(item->property("align").toInt());
            break;
        }
    }

    QDialog::accept();
}

// ----------------------------------------------------------------------------

void DefaultAlignDialog::setAlignment(Qt::Alignment alignment)
{
    mAlignment = alignment;

    auto items = findChildren<QCheckBox*>();
    for (auto item : items)
    {
        auto item_alignment = Qt::Alignment(item->property("align").toInt());
        if (item_alignment == alignment)
        {
            item->setChecked(true);
        } else
            item->setChecked(false);
    }

}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
