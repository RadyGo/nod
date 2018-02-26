
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
    : mScene(scene),
      mPlanner(*this)
{
}

// ----------------------------------------------------------------------------

void NodeGrid::setSceneRect(const QRectF &rc)
{
    //qDebug() << "NodeGrid: set scene rect" << rc;

    mOrigin = rc.topLeft();
    mSize = rc.size();

    mCells = QSize(ceilf(rc.width() / mGridSize), ceilf(rc.height() / mGridSize));
    mGrid.resize(mCells.width() * mCells.height());    

    for (int j=0; j<mCells.height(); ++j)
    {
        for (int i=0; i<mCells.width(); ++i)
        {
            auto &cell = mGrid[i + j * mCells.width()];
            cell.i = i;
            cell.j = j;
            cell.visited = -1;
        }
    }

    updateGrid();

    mScene.invalidate(rc, QGraphicsScene::ForegroundLayer);
}

// ----------------------------------------------------------------------------

CellUsage NodeGrid::cellUsage(const QPoint &pt) const
{
    if (pt.x() < 0 || pt.y() < 0 || pt.x() >= mCells.width() || pt.y() >= mCells.height())
        return CellUsage::Solid;


    return mGrid[pt.x() + pt.y() * mCells.width()].usage;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellUsage(const QPoint &pt, CellUsage usage)
{
    if (pt.x() >= 0 && pt.y() >= 0 && pt.x() < mCells.width() && pt.y() < mCells.height())
        mGrid[pt.x() + pt.y() * mCells.width()].usage = usage;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellUsage(const QRect &rc, CellUsage usage)
{
    QRect bounds(0, 0, mCells.width(), mCells.height());

    QRect clip = bounds.intersected(rc);

    auto row = &mGrid[clip.topLeft().x() + clip.topLeft().y() * mCells.width()];

    auto w = clip.width();
    auto h = clip.height();
    auto pitch = mCells.width();
    for (int y=0; y<h; ++y)
    {
        auto col = row;
        for (int x=0; x<w; ++x)
        {
            col->usage = usage;
            col++;
        }

        row += pitch;
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

int NodeGrid::cellIndex(const QPoint &cell)
{
    if (cell.x() < 0 || cell.x() >= mCells.width() ||
        cell.y() < 0 || cell.y() >= mCells.height())
        return -1;

    return cell.x() + cell.y() * mCells.width();
}

// ----------------------------------------------------------------------------

QLine NodeGrid::clipCellLine(const QLine &line)
{
    auto p1 = line.p1();
    auto p2 = line.p2();

    // sort by x / y
    auto x0 = p1.x() < p2.x() ? p1.x() : p2.x();
    auto y0 = p1.y() < p2.y() ? p1.y() : p2.y();

    auto x1 = p1.x() > p2.x() ? p1.x() : p2.x();
    auto y1 = p1.y() > p2.y() ? p1.y() : p2.y();


    QPointF pt;
    QLineF clip(line);
    clip.intersect(QLineF(x0, y0, x1, y0), &pt);
    clip.intersect(QLineF(x1, y0, x1, y1), &pt);
    clip.intersect(QLineF(x0, y1, x1, y1), &pt);
    clip.intersect(QLineF(x0, y0, x0, y1), &pt);

    return clip.toLine();
}

// ----------------------------------------------------------------------------

GridCell *NodeGrid::cell(const QPoint &pt)
{
    auto index = cellIndex(pt);
    return cell(index);
}

// ----------------------------------------------------------------------------

GridCell *NodeGrid::cell(int index)
{
    return index >= 0 ? (&mGrid[index]) : nullptr;
}

// ----------------------------------------------------------------------------

void NodeGrid::setUsage(const QRectF &rc, CellUsage usage)
{
    auto tl = cellAt(rc.topLeft());
    auto br = cellAt(rc.bottomRight());
    setCellUsage(QRect(tl, QSize(br.x() - tl.x() + 1, br.y() - tl.y() + 1)), usage);
}

// ----------------------------------------------------------------------------

QPointF NodeGrid::positionAt(const QPoint &cell, bool center) const
{
    float px = mOrigin.x() + cell.x() * mGridSize + (center ? mGridSize / 2 : 0);
    float py = mOrigin.y() + cell.y() * mGridSize + (center ? mGridSize / 2 : 0);
    return { px, py };
}

// ----------------------------------------------------------------------------

QRectF NodeGrid::snap(const QRectF &rc, bool center)
{
    auto p1 = rc.topLeft();
    auto p2 = rc.bottomRight();

    p1 = snapAt(p1, center);
    p2 = snapAt(p2, center);

    return QRectF(p1, p2);
}

// ----------------------------------------------------------------------------

void NodeGrid::debugDraw(QPainter &painter)
{
    painter.setClipRect(mScene.sceneRect());
    painter.setOpacity(0.5f);

    static const QColor usage_color[4] =
    {
        QColor(Qt::lightGray),
        QColor(Qt::black),
        QColor(Qt::blue),
        QColor(Qt::green)
    };

    float y = mOrigin.y();
    for (int j=0; j<mCells.height(); ++j)
    {
        float x = mOrigin.x();
        for (int i=0; i<mCells.width(); ++i)
        {
            auto &cell = mGrid[i + j * mCells.width()];
            painter.fillRect(QRectF(x, y, mGridSize, mGridSize), usage_color[int(cell.usage)]);

            x += mGridSize;
        }

        y += mGridSize;
    }
}

// ----------------------------------------------------------------------------

int NodeGrid::cellWeight(int index)
{
    return mGrid[index].cost;
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
    setUsage(bounds, CellUsage::Empty);

    auto items = mScene.items();
    for (auto item : items)
    {
        auto node = qgraphicsitem_cast<NodeItem*>(item);
        if (node)
        {
            auto rc = node->sceneBoundingRect();
            if (bounds.contains(rc))
                node->updateGrid();
            continue;
        }

        auto connection = qgraphicsitem_cast<ConnectionItem*>(item);
        if (connection)
        {
            connection->updatePath();
            connection->updateGrid();
        }
    }
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
