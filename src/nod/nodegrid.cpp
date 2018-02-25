
// ----------------------------------------------------------------------------

#include <queue>

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
    qDebug() << "NodeGrid: set scene rect" << rc;

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

bool NodeGrid::isCellBlocked(const QPoint &pt) const
{
    return pt.x() < 0 || pt.y() < 0 ||
           pt.x() >= mCells.width() || pt.y() >= mCells.height() ||
           mGrid[pt.x() + pt.y() * mCells.width()].cost > 0;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellBlocked(const QPoint &pt, bool blocked)
{
    if (pt.x() >= 0 && pt.y() >= 0 && pt.x() < mCells.width() && pt.y() < mCells.height())
        mGrid[pt.x() + pt.y() * mCells.width()].cost = blocked;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellsBlocked(const QRect &rc, bool blocked)
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
            col->cost = blocked ? 100 : 0;
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

    int c = 0;
    float y = mOrigin.y();
    for (int j=0; j<mCells.height(); ++j)
    {
        float x = mOrigin.x();
        for (int i=0; i<mCells.width(); ++i)
        {
            if (mGrid[c++].cost > 0)
                painter.fillRect(QRectF(x, y, mGridSize, mGridSize), Qt::red);

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
    setBlocked(bounds, false);

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

PathPlanner::PathPlanner(NodeGrid &grid)
    : mGrid(grid)
{

}

// ----------------------------------------------------------------------------

bool operator<(const GridCell &a, const GridCell &b)
{
    return a.cost > b.cost;
}

// ----------------------------------------------------------------------------

static float l1_norm(int i0, int j0, int i1, int j1)
{
  return std::abs(i0 - i1) + std::abs(j0 - j1);
}

// ----------------------------------------------------------------------------

PathPlanner::Result PathPlanner::plan(QVector<QPointF> &path, const QPointF &p1, const QPointF &p2, std::function<int (const GridCell &)> fn)
{
    /*
    qDebug() << "A* from" << p1 << "to" << p2;

    {
        QDebug d = qDebug();
        for (int j=0; j<mGrid.cells().height(); ++j)
        {
            for (int i=0; i<mGrid.cells().width(); ++i)
            {
                auto cell = mGrid.cell(i + j * mGrid.cells().width());
                d << ' ' << '(' << cell->i << cell->j << ')';
            }
            d << '\n';
        }
    }
    */

    path.clear();

    ++mSearchNo;

    auto line = mGrid.clipCellLine(QLine(mGrid.cellAt(p1), mGrid.cellAt(p2)));
    if (line.isNull())
        return Result::NoPath;

    auto c1 = line.p1();
    auto c2 = line.p2();

    //qDebug() << "A* clipped cells" << c1 << "to" << c2;

    auto cell = mGrid.cell(c1);
    if (!cell)
        return Result::NoPath;

    cell->from = -1;
    cell->visited = mSearchNo;

    mFrontier.clear();
    mFrontier.push_back(mGrid.cellIndex(c1));

    int w = mGrid.cells().width();
    int h = mGrid.cells().height();

    int last_index = -1;
    while (!mFrontier.empty())
    {
        auto from = mFrontier.front();
        auto next = mGrid.cell(from);
        mFrontier.pop_front();
        if (!next)
            continue;

        if (next->i == c2.x() && next->j == c2.y())
        {
            //qDebug() << "A* found" << next->i << next->j;
            last_index = from;
            break;
        }

        int ni[4][2] = {
            { next->i > 0 ? next->i - 1 : -1,     next->j },
            { next->i < w - 1 ? next->i + 1 : -1, next->j },
            { next->i,                            next->j > 0 ? next->j - 1 : -1 },
            { next->i,                            next->j < h - 1 ? next->j + 1 : -1 }
        };

        for (int i=0; i<4; ++i)
        {
            if (ni[i][0] < 0 || ni[i][1] < 0)
                continue;

            auto index = mGrid.cellIndex(QPoint(ni[i][0], ni[i][1]));
            auto ncell = mGrid.cell(index);
            if (!ncell || ncell->visited == mSearchNo)
                continue;

            ncell->from = from;
            ncell->visited = mSearchNo;

            if (ncell->i == c2.x() && ncell->j == c2.y())
            {
                //qDebug() << "A* found" << ncell->i << ncell->j;
                last_index = index;
                mFrontier.clear();
                break;
            }

            int cost = ncell->cost;

            //qDebug() << "A* cost" << ncell->i << ncell->j << cost;

            int user_cost = fn(*ncell);
            if (user_cost < 0)
                continue;

            cost += user_cost;

            if (cost > 0)
                continue;

            last_index = index;

            mFrontier.push_back(last_index);
        }
    }

    if (last_index < 0)
        return Result::NoPath;

    while (last_index >= 0)
    {
        auto cell = mGrid.cell(last_index);
        //qDebug() << "A* raw path" << cell->i << cell->j;
        path.append(mGrid.positionAt(QPoint(cell->i, cell->j)));
        last_index = cell->from;
    }

    for (int i=0, j=int(path.size())-1; i<j; ++i, --j)
        std::swap(path[i], path[j]);

    //qDebug() << "A* path" << path;

    return Result::Found;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
