
// ----------------------------------------------------------------------------

#include <QContextMenuEvent>
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

    QAction *debug = new QAction(tr("Debug"), this);
    debug->setCheckable(true);
    debug->setChecked(mScene->isDebug());
    connect(debug, &QAction::toggled, this, &NodeView::setDebug);
    mActions[int(Action::DebugEnabled)] = debug;

    auto grid = new QAction(tr("Grid"), this);
    grid->setCheckable(true);
    grid->setChecked(mScene->drawGrid());
    connect(grid, &QAction::toggled, this, &NodeView::setGrid);
    mActions[int(Action::GridEnabled)] = grid;

    auto layout_horiz = new QAction(tr("Horizontal"), this);
    connect(layout_horiz, &QAction::triggered, this, &NodeView::layoutHoriz);
    mActions[int(Action::LayoutHoriz)] = layout_horiz;

    auto layout_vert = new QAction(tr("Vertical"), this);
    connect(layout_vert, &QAction::triggered, this, &NodeView::layoutVert);
    mActions[int(Action::LayoutVert)] = layout_vert;

    auto layout_grid = new QAction(tr("Grid"), this);
    connect(layout_grid, &QAction::triggered, this, &NodeView::layoutGrid);
    mActions[int(Action::LayoutGrid)] = layout_grid;
}

// ----------------------------------------------------------------------------

void NodeView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(tr("Create"));

    menu.addAction(action(Action::DebugEnabled));

    menu.addAction(action(Action::GridEnabled));

    auto layout_menu = menu.addMenu(tr("Layout"));
    layout_menu->addAction(action(Action::LayoutHoriz));
    layout_menu->addAction(action(Action::LayoutVert));
    layout_menu->addAction(action(Action::LayoutGrid));

    menu.exec(event->globalPos());
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

    auto spacing = 2 * mScene->grid().gridSize();
    auto items = scene()->selectedItems();
    auto rect = scene()->selectionArea().boundingRect();

    auto x = rect.x();
    auto y = rect.y();

    int column = 0;
    int row_height = 0;

    for (auto item : items)
    {
        auto item_rc = item->sceneBoundingRect();
        if (item_rc.height() > row_height)
            row_height = item_rc.height();

        if (column == columns)
        {
            column = 0;
            x = rect.x();
            y += spacing + row_height;
        }

        item->setPos(x, y);
        x += spacing + item_rc.width();

        column++;
    }
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
