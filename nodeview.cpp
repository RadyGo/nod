
// ----------------------------------------------------------------------------

#include <QContextMenuEvent>
#include <QMenu>

// ----------------------------------------------------------------------------

#include "nod/nodescene.h"
#include "nod/nodeview.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeView::NodeView(NodeScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
      mScene(scene)
{
}

void NodeView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(tr("Create"));

    auto debug = menu.addAction(tr("Debug"));
    debug->setCheckable(true);
    debug->setChecked(mScene->isDebug());
    connect(debug, &QAction::toggled, this, &NodeView::setDebug);

    auto grid = menu.addAction(tr("Grid"));
    grid->setCheckable(true);
    grid->setChecked(mScene->drawGrid());
    connect(grid, &QAction::toggled, this, &NodeView::setGrid);

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

} } // namespaces

// ----------------------------------------------------------------------------
