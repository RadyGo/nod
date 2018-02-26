
// ----------------------------------------------------------------------------

#include <QLine>

// ----------------------------------------------------------------------------

#include "nod/nodegrid.h"
#include "nod/pathplanner.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

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

            if (isBlocked(*ncell))
                continue;

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

bool PathPlanner::isBlocked(const GridCell &cell) const
{
    return cell.usage == CellUsage::Node || cell.usage == CellUsage::Solid;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
