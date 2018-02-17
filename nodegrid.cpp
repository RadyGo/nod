
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QPainter>

// ----------------------------------------------------------------------------

#include "nod/nodegrid.h"
#include "nod/nodescene.h"
#include "nod/nodeitem.h"
#include "nod/connectionitem.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeGrid::NodeGrid(NodeScene &scene)
    : mScene(scene)
{
}

// ----------------------------------------------------------------------------

void NodeGrid::setSceneRect(const QRectF &rc)
{
    qDebug() << "NodeGrid: set scene rect" << rc;

    mOrigin = rc.topLeft();
    mSize = rc.size();

    mCells = QSize(ceilf(rc.width() / mGridSize), ceilf(rc.height() / mGridSize));
    mGrid.resize(mCells.width() * mCells.height());

    updateGrid();

    mScene.invalidate(rc, QGraphicsScene::ForegroundLayer);
}

// ----------------------------------------------------------------------------

bool NodeGrid::isCellBlocked(const QPoint &pt) const
{
    return pt.x() < 0 || pt.y() < 0 ||
           pt.x() >= mCells.width() || pt.y() >= mCells.height() ||
           mGrid[pt.x() + pt.y() * mCells.width()] != false;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellBlocked(const QPoint &pt, bool blocked)
{
    if (pt.x() >= 0 && pt.y() >= 0 && pt.x() < mCells.width() && pt.y() < mCells.height())
        mGrid[pt.x() + pt.y() * mCells.width()] = blocked;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellsBlocked(const QRect &rc, bool blocked)
{
    QRect bounds(0, 0, mCells.width(), mCells.height());

    QRect clip = bounds.intersected(rc);

    auto ptr = &mGrid[clip.topLeft().x() + clip.topLeft().y() * mCells.width()];

    auto w = clip.width();
    auto h = clip.height();
    auto pitch = mCells.width();
    for (int y=0; y<h; ++y)
    {
        memset(ptr, blocked ? 1 : 0, w);
        ptr += pitch;
    }
}

// ----------------------------------------------------------------------------

QPoint NodeGrid::cellAt(const QPointF &pt) const
{
    int cx = int(floorf((pt.x() - mOrigin.x()) / mGridSize));
    int cy = int(floorf((pt.y() - mOrigin.y()) / mGridSize));
    return { cx, cy };
}

// ----------------------------------------------------------------------------

void NodeGrid::setBlocked(const QRectF &rc, bool blocked)
{
    auto tl = cellAt(rc.topLeft());
    auto br = cellAt(rc.bottomRight());
    setCellsBlocked(QRect(tl, QSize(br.x() - tl.x() + 1, br.y() - tl.y() + 1)), blocked);
}

// ----------------------------------------------------------------------------

QPointF NodeGrid::positionAt(const QPoint &cell, bool center) const
{
    float px = mOrigin.x() + cell.x() * mGridSize + (center ? mGridSize / 2 : 0);
    float py = mOrigin.y() + cell.y() * mGridSize + (center ? mGridSize / 2 : 0);
    return { px, py };
}

// ----------------------------------------------------------------------------

NodeGrid::PathResult NodeGrid::path(QVector<QPointF> &path, const QPointF &p1, const QPointF &p2)
{
    // TODO: https://github.com/hjweide/a-star/blob/master/astar.cpp

    auto c1 = cellAt(p1);
    if (isCellBlocked(c1))
        return PathResult::NoPath;

    auto c2 = cellAt(p2);

    // path always starts at p1
    path.append(p1);

    auto c = c1;

    int steps = 10000;
    while (steps-- >= 0)
    {
        if (isBlocked(c))
        {
            // blocking cell position is last on path
            path.append(positionAt(c));
            break;
        }

        if (c == c2)
        {
            // reached end, always ends at p2
            path.append(p2);
            return PathResult::Finished;
        }

        // TODO: step c
    }

    return PathResult::Blocked;
}

// ----------------------------------------------------------------------------

void NodeGrid::debugDraw(QPainter &painter)
{
    painter.setClipRect(mScene.sceneRect());
    painter.setOpacity(0.5f);

    int c = 0;
    float y = mOrigin.y();
    for (int j=0; j<mCells.height(); ++j)
    {
        float x = mOrigin.x();
        for (int i=0; i<mCells.width(); ++i)
        {
            if (mGrid[c++])
                painter.fillRect(QRectF(x, y, mGridSize, mGridSize), Qt::red);

            x += mGridSize;
        }

        y += mGridSize;
    }
}

// ----------------------------------------------------------------------------

void NodeGrid::draw(QPainter &painter)
{
    painter.setClipRect(mScene.sceneRect());
    // TODO: add to style
    painter.setPen(QColor(0xac, 0xac, 0xac));
    painter.fillRect(mScene.sceneRect(), QColor(0xbf, 0xbf, 0xbf));

    {
        float x = mOrigin.x();
        for (int i=0; i<mCells.width(); ++i)
        {
            painter.drawLine(x,
                             mOrigin.y(),
                             x,
                             mOrigin.y() + mCells.height() * mGridSize - 1);
            x += mGridSize;
        }

        float y = mOrigin.y();
        for (int i=0; i<mCells.height(); ++i)
        {
            painter.drawLine(mOrigin.x(),
                             y,
                             mOrigin.x() + mCells.width() * mGridSize - 1,
                             y);
            y += mGridSize;
        }
    }
}

// ----------------------------------------------------------------------------

void NodeGrid::updateGrid()
{
    updateGrid(QRectF());
}

// ----------------------------------------------------------------------------

void NodeGrid::updateGrid(const QRectF &area)
{
    if (mGrid.isEmpty())
        return;

    QRectF bounds = area.isNull() ? mScene.sceneRect() : area;
    setBlocked(bounds, false);

    auto items = mScene.items();
    for (auto item : items)
    {
        auto node = qgraphicsitem_cast<NodeItem*>(item);
        if (node)
        {
            auto rc = item->sceneBoundingRect();
            if (bounds.contains(rc))
                setBlocked(rc.adjusted(0, 0, -1, -1), true);

            continue;
        }

        auto connection = qgraphicsitem_cast<ConnectionItem*>(item);
        if (connection)
            connection->updateGrid(*this);
    }
}

// ----------------------------------------------------------------------------

} } // namespaces
