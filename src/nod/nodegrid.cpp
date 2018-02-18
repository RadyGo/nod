
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
           mGrid[pt.x() + pt.y() * mCells.width()].cost > 0;
}

// ----------------------------------------------------------------------------

void NodeGrid::setCellBlocked(const QPoint &pt, bool blocked)
{
    if (pt.x() >= 0 && pt.y() >= 0 && pt.x() < mCells.width() && pt.y() < mCells.height())
        mGrid[pt.x() + pt.y() * mCells.width()].cost = 100;
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

struct Node
{
    //int     index;
    int     i, j;
    float   cost;
    bool    visited;
};

// ----------------------------------------------------------------------------

bool operator<(const Node &a, const Node &b)
{
    return a.cost > b.cost;
}

// ----------------------------------------------------------------------------

static float l1_norm(int i0, int j0, int i1, int j1)
{
  return std::abs(i0 - i1) + std::abs(j0 - j1);
}

// ----------------------------------------------------------------------------

NodeGrid::PathResult NodeGrid::path(QVector<QPointF> &path, const QPointF &p1, const QPointF &p2)
{
    if (mCells.isEmpty())
        return PathResult::NoPath;

    ++mSearchNo;

    // TODO: https://github.com/hjweide/a-star/blob/master/astar.cpp

    auto c1 = cellAt(p1);
    auto c2 = cellAt(p2);

    //auto start = c1.x() + c1.y() * mCells.width();
    //auto end = c2.x() + c2.y() * mCells.width();
    auto x0 = c1.x() < c2.x() ? c1.x() : c2.x();
    auto y0 = c1.y() < c2.y() ? c1.y() : c2.y();

    std::deque<Node> frontier;
    std::vector<Node> visited;

    frontier.push_back({ x0, y0, 0.0f, false });

    int w = mCells.width();
    int h = mCells.height();

    while (!frontier.empty())
    {
        auto &node = frontier.back();
        frontier.pop_back();

        int n[4][2] = {
            { node.i > 0 ? node.i - 1 : -1,     node.j },
            { node.i < w - 1 ? node.i + 1 : -1, node.j },
            { node.i,                           node.j > 0 ? node.j - 1 : -1 },
            { node.i,                           node.j < h - 1 ? node.j + 1 : -1 },
        };

        for (int i=0; i<4; ++i)
        {
            auto ni = n[i];
            if (ni[0] < 0 || ni[1] < 0)
                continue;

            auto &cell = mGrid[ni[0] + ni[1] * w];
            if (cell.visited == mSearchNo)
                continue;

            cell.visited = mSearchNo;
        }

        /*
        int n[4] = {
            node.j > 0 ? node.j .index - mCells.width() : -1,
            node.i > 0 ? curr.index - 1 : -1,
            (node.i + 1 < mCells.width()) ? curr.index + 1 : -1,
            (node.j + 1 < mCells.height()) ? curr.index + mCells.width() : -1,
        };
        */
    }



#if 0
    auto start = c1.x() + c1.y() * mCells.width();
    auto end = c2.x() + c2.y() * mCells.width();

    std::vector<float> costs;
    costs.resize(mCells.width() * mCells.height());

    for (int i=0; i<costs.size(); ++i)
        costs[i] = std::numeric_limits<float>::infinity();
    costs[start] = 0.0f;

    std::priority_queue<Node> next;
    next.push({ start, 0.0f });

    while (!next.empty())
    {
        auto &curr = next.top();
        if (curr.index == end)
        {
            break;
        }

        next.pop();

        int row = curr.index / mCells.width();
        int col = curr.index % mCells.width();

        int n[4] = {
            row > 0 ? curr.index - mCells.width() : -1,
            col > 0 ? curr.index - 1 : -1,
            (col + 1 < mCells.width()) ? curr.index + 1 : -1,
            (row + 1 < mCells.height()) ? curr.index + mCells.width() : -1,
        };

        for (int i=0; i<4; ++i)
        {
            if (n[i] < 0)
                continue;

            float new_cost = costs[curr.index] + cellWeight(n[i]);
            if (new_cost >= costs[n[i]])
                continue;

            float hcost = l1_norm(n[i] / mCells.width(), n[i] % mCells.width(),
                                  end / mCells.width(), end % mCells.width());

            float prio = new_cost + hcost;
            next.push({ n[i], prio });

            costs[n[i]] = new_cost;
            //paths[n[i]] = curr.index;
        }
    }
#endif

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
            auto rc = item->sceneBoundingRect();
            if (bounds.contains(rc))
                setBlocked(rc.adjusted(0, 0, -1, -1), true);

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
