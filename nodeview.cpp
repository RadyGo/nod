
// ----------------------------------------------------------------------------

#include <QCheckBox>
#include <QContextMenuEvent>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QButtonGroup>
#include <QInputDialog>
#include <QMenu>

// ----------------------------------------------------------------------------

#include "nod/connectionitem.h"
#include "nod/nodeitem.h"
#include "nod/nodescene.h"
#include "nod/nodeview.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeView::NodeView(NodeScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
      mScene(scene)
{
    setDragMode(RubberBandDrag);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);

    connect(mScene, &NodeScene::selectionChanged,
            this, &NodeView::selectionChanged);

    QAction *debug = new QAction(tr("&Debug"), this);
    debug->setCheckable(true);
    debug->setChecked(mScene->isDebug());
    connect(debug, &QAction::toggled, this, &NodeView::setDebug);
    mActions[int(Action::DebugEnabled)] = debug;

    auto grid = new QAction(tr("&Grid"), this);
    grid->setCheckable(true);
    grid->setChecked(mScene->drawGrid());
    connect(grid, &QAction::toggled, this, &NodeView::setGrid);
    mActions[int(Action::GridEnabled)] = grid;

    /* align */

    auto align_top = new QAction(tr("&Top"), this);
    connect(align_top, &QAction::triggered, this, &NodeView::alignTop);
    mActions[int(Action::AlignTop)] = align_top;

    auto align_bottom = new QAction(tr("&Bottom"), this);
    connect(align_bottom, &QAction::triggered, this, &NodeView::alignBottom);
    mActions[int(Action::AlignBottom)] = align_bottom;

    auto align_left = new QAction(tr("&Left"), this);
    connect(align_left, &QAction::triggered, this, &NodeView::alignLeft);
    mActions[int(Action::AlignLeft)] = align_left;

    auto align_right = new QAction(tr("&Right"), this);
    connect(align_right, &QAction::triggered, this, &NodeView::alignRight);
    mActions[int(Action::AlignRight)] = align_right;

    auto align_dialog = new QAction(tr("Align ..."), this);
    connect(align_dialog, &QAction::triggered, this, &NodeView::alignDialog);
    mActions[int(Action::AlignDialog)] = align_dialog;

    /* layout */

    auto layout_horiz = new QAction(tr("&Horizontal"), this);
    connect(layout_horiz, &QAction::triggered, this, &NodeView::layoutHoriz);
    mActions[int(Action::LayoutHoriz)] = layout_horiz;

    auto layout_vert = new QAction(tr("&Vertical"), this);
    connect(layout_vert, &QAction::triggered, this, &NodeView::layoutVert);
    mActions[int(Action::LayoutVert)] = layout_vert;

    auto layout_grid = new QAction(tr("&Grid"), this);
    connect(layout_grid, &QAction::triggered, this, &NodeView::layoutGrid);
    mActions[int(Action::LayoutGrid)] = layout_grid;        
}

// ----------------------------------------------------------------------------

QMenu *NodeView::createMenu()
{
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Create"));

    menu->addAction(action(Action::DebugEnabled));
    menu->addAction(action(Action::GridEnabled));

/*    auto align_menu = menu->addMenu(tr("Align"));
    align_menu->addAction(action(Action::AlignLeft));
    align_menu->addAction(action(Action::AlignTop));
    align_menu->addAction(action(Action::AlignRight));*/
    menu->addAction(action(Action::AlignDialog));

    auto layout_menu = menu->addMenu(tr("Layout"));
    layout_menu->addAction(action(Action::LayoutHoriz));
    layout_menu->addAction(action(Action::LayoutVert));
    layout_menu->addAction(action(Action::LayoutGrid));

    return menu;
}

// ----------------------------------------------------------------------------

void NodeView::contextMenuEvent(QContextMenuEvent *event)
{
    QScopedPointer<QMenu> menu(createMenu());
    menu->exec(event->globalPos());
}

// ----------------------------------------------------------------------------

void NodeView::setDebug(bool debug)
{
    mScene->setDebug(debug);
}

// ----------------------------------------------------------------------------

void NodeView::setGrid(bool draw)
{
    mScene->setDrawGrid(draw);
}

// ----------------------------------------------------------------------------

void NodeView::layoutHoriz()
{
    layoutGridColumns(std::numeric_limits<int>::max());
}

// ----------------------------------------------------------------------------

void NodeView::layoutVert()
{
    layoutGridColumns(1);
}

// ----------------------------------------------------------------------------

void NodeView::layoutGrid()
{
    static int columns = 3;

    bool ok;
    int input = QInputDialog::getInt(this, tr("Enter Grid Size"), tr("Columns"), columns, 1, 32, 1, &ok);
    if (ok)
    {
        columns = input;
        layoutGridColumns(columns);
    }
}

// ----------------------------------------------------------------------------

void NodeView::layoutGridColumns(int columns)
{
    if (!scene())
        return;

    auto spacing = LayoutSpacing * mScene->grid().gridSize();

    QRectF bounds;
    for (auto item : mOrderedSelection)
        bounds = bounds.united(item->sceneBoundingRect());

    qDebug() << "NodeView: selection rect" << bounds;

    auto x = bounds.x();
    auto y = bounds.y();

    int column = 0;
    int row_height = 0;

    for (auto item : mOrderedSelection)
    {
        auto item_rc = item->sceneBoundingRect();
        if (item_rc.height() > row_height)
            row_height = item_rc.height();

        if (column == columns)
        {
            column = 0;
            x = bounds.x();
            y += spacing + row_height;
        }

        item->setPos(x, y);
        x += spacing + item_rc.width();

        column++;
    }
}

// ----------------------------------------------------------------------------

void NodeView::align(Qt::Alignment alignment)
{
    if (mOrderedSelection.isEmpty())
        return;

    auto to = mOrderedSelection.back();
    auto to_rc = to->sceneBoundingRect();

    for (int i=0; i<mOrderedSelection.size()-1; ++i)
    {
        auto item = mOrderedSelection[i];
        auto item_rc = item->sceneBoundingRect();

        auto x = item_rc.x();
        auto y = item_rc.y();

        if (alignment & Qt::AlignRight)
        {
            x = to_rc.right();
        }

        item->setPos(x, y);
    }
}

// ----------------------------------------------------------------------------

class AlignDialog : public QDialog
{
public:

    AlignDialog(QWidget *parent)
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

    void                accept() override
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

    Qt::Alignment       alignment() const { return mAlignment; }

private:

    Qt::Alignment       mAlignment = Qt::AlignTop | Qt::AlignLeft;
};

// ----------------------------------------------------------------------------

void NodeView::alignDialog()
{
    AlignDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        align(dlg.alignment());
}

// ----------------------------------------------------------------------------

void NodeView::alignLeft()
{
    align(Qt::AlignLeft);
}

// ----------------------------------------------------------------------------

void NodeView::alignTop()
{
    align(Qt::AlignTop);
}

// ----------------------------------------------------------------------------

void NodeView::alignRight()
{
    align(Qt::AlignRight);
}

// ----------------------------------------------------------------------------

void NodeView::alignBottom()
{
    align(Qt::AlignBottom);
}

// ----------------------------------------------------------------------------

void NodeView::selectionChanged()
{
    auto selected = scene()->selectedItems();
    if (!selected.isEmpty())
    {
        for (auto it=mOrderedSelection.begin(); it!=mOrderedSelection.end(); )
        {
            if (!(*it)->isSelected())
            {
                it = mOrderedSelection.erase(it);
            } else
                ++it;
        }

        for (auto item : selected)
        {
            if (!mOrderedSelection.contains(item))
                mOrderedSelection.append(item);
        }

    } else
        mOrderedSelection.clear();
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
