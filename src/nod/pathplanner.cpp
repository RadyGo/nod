
// ----------------------------------------------------------------------------

#include <iomanip>
#include <sstream>

// ----------------------------------------------------------------------------

#include <QDebug>
#include <QLine>

// ----------------------------------------------------------------------------

#include "nod/nodegrid.h"
#include "nod/pathplanner.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

inline bool operator<(const CellRef &a, const CellRef &b)
{
    return b.priority < a.priority;
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

static int manhatten(const GridCell &a, const GridCell &b)
{
    return std::abs<int>(a.i - b.i) + std::abs<int>(a.j - b.j);
}

// ----------------------------------------------------------------------------

static void dumpCosts(const NodeGrid &grid, int searchno)
{
    std::stringstream s;

    int cw = 4;

    s << std::setw(cw) << ' ' << ' ';
    for (int i=0; i<grid.cells().width(); ++i)
        s << std::setw(cw) << i << ' ';
    s << '\n';

    s << std::setw(cw) << ' ' << '+';
    s << std::setfill('-');
    for (int i=0; i<grid.cells().width(); ++i)
        s << std::setw(cw) << '-' << '-';
    s << std::setfill(' ');

    s << '\n';

    for (int j=0; j<grid.cells().height(); ++j)
    {
        s << std::setw(cw) << j << '|';

        for (int i=0; i<grid.cells().width(); ++i)
        {
            auto cell = grid.cell(i + j * grid.cells().width());
            if (cell->usage == CellUsage::Node || cell->usage == CellUsage::Solid)
            {
                s << std::setw(cw) << 'b' << ' ';
            } else
            if (cell->visited != searchno)
            {
                s << std::setw(cw) << 'n' << ' ';
            } else
                s << std::setw(cw) << cell->cost << ' ';
        }
        s << '\n';
    }

    qDebug() << qPrintable(s.str().c_str());

    /*
    QDebug d = qDebug();
    d << '\n';
    for (int j=0; j<grid.cells().height(); ++j)
    {
        for (int i=0; i<grid.cells().width(); ++i)
        {
            auto cell = grid.cell(i + j * grid.cells().width());
            if (cell->usage == CellUsage::Node || cell->usage == CellUsage::Solid)
            {
                d << ' ' << 'b';
            } else
                d << ' ' << cell->cost;
        }
        d << '\n';
    }
    */
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

#if 1
    path.clear();

    ++mSearchNo;

    auto line = mGrid.clipCellLine(QLine(mGrid.cellAt(p1), mGrid.cellAt(p2)));
    if (line.isNull())
        return Result::NoPath;

    auto c1 = line.p1();
    auto c2 = line.p2();

    qDebug() << "A* from cell" << c1 << "to" << c2;

    auto cell = mGrid.cell(c1);
    if (!cell)
        return Result::NoPath;

    cell->from = -1;
    cell->visited = mSearchNo;

    mFrontier = std::priority_queue<CellRef>();
    mFrontier.push({ 0, mGrid.cellIndex(c1) });

    int w = mGrid.cells().width();
    int h = mGrid.cells().height();

    int iteration = 0;
    int last_index = -1;
    while (!mFrontier.empty())
    {
        if (iteration++ > 500)
            break;


        qDebug() << "A* frontier pop " << mFrontier.top().cell_index << mFrontier.top().priority << "size:" << mFrontier.size();

        auto from = mFrontier.top();
        auto cell = mGrid.cell(from.cell_index);
        mFrontier.pop();
        if (!cell)
            continue;

        last_index = from.cell_index;

        if (cell->i == c2.x() && cell->j == c2.y())
        {
            qDebug() << "A* found" << cell->i << cell->j;
            break;
        }

        dumpCosts(mGrid, mSearchNo);

        int ni[4][2] =
        {
            { cell->i > 0 ? cell->i - 1 : -1,     cell->j },
            { cell->i < w - 1 ? cell->i + 1 : -1, cell->j },
            { cell->i,                            cell->j > 0 ? cell->j - 1 : -1 },
            { cell->i,                            cell->j < h - 1 ? cell->j + 1 : -1 }
        };

        for (int i=0; i<4; ++i)
        {
            if (ni[i][0] < 0 || ni[i][1] < 0)
                continue;

            auto index = mGrid.cellIndex(QPoint(ni[i][0], ni[i][1]));
            auto ncell = mGrid.cell(index);
            if (!ncell)
                continue;

            if (ncell->visited != mSearchNo)
            {
                ncell->cost = -1; //std::numeric_limits<int>::max();
                ncell->visited = mSearchNo;
            } else
            if (ncell->cost != -1)
                continue;

            if (isBlocked(*ncell))
                continue;

            int cost = cell->cost;

            qDebug() << "A* cost" << ncell->i << ncell->j << cost;

            int user_cost = fn(*ncell);
            if (user_cost < 0)
                continue;

            cost += user_cost;

            // TODO + graph cost

            /*
            if (cost < least_cost)
            {
                least_cost = cost;
                least_index = i;
            }
            */
#if 1
            if (ncell->cost == -1 || cost < ncell->cost)
            {
                cost += manhatten(*cell, *ncell);
                ncell->from = from.cell_index;
                ncell->cost = cost;

                mFrontier.push({ cost, index });
            }
#endif
        }

#if 0
        if (least_index < 0)
            continue;

        auto index = mGrid.cellIndex(QPoint(ni[least_index][0], ni[least_index][1]));
        auto ncell = mGrid.cell(index);
        if (!ncell)
            continue;

//        if (cost < ncell->cost)
//        {
            qDebug() << "A* manhatten" << ncell->i << ncell->j << manhatten(*cell, *ncell);

            int prio = ncell->cost + manhatten(*cell, *ncell);
            ncell->from = from.cell_index;
            ncell->cost = prio;
//        }

        mFrontier.push({ ncell->cost, index });
#endif
    }

    if (last_index < 0)
        return Result::NoPath;

    while (last_index >= 0)
    {
        auto cell = mGrid.cell(last_index);
        //qDebug() << "A* raw path" << cell->i << cell->j;
        path.append(mGrid.positionAt(QPoint(cell->i, cell->j)));
        last_index = cell->from;

        if (path.size() > 1000)
        {
            qDebug() << "A* path loop";
            break;
        }
    }

    for (int i=0, j=int(path.size())-1; i<j; ++i, --j)
        std::swap(path[i], path[j]);

    //qDebug() << "A* path" << path;

    return Result::Found;
#if 0
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

    mFrontier = std::priority_queue<CellRef>();
    mFrontier.push({ 0, mGrid.cellIndex(c1) });

    int w = mGrid.cells().width();
    int h = mGrid.cells().height();

    int last_index = -1;
    while (!mFrontier.empty())
    {
        auto from = mFrontier.top();
        auto next = mGrid.cell(from.cell_index);
        mFrontier.pop();
        if (!next)
            continue;

        if (next->i == c2.x() && next->j == c2.y())
        {
            //qDebug() << "A* found" << next->i << next->j;
            last_index = from.cell_index;
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

            ncell->from = from.cell_index;
            ncell->visited = mSearchNo;

            if (ncell->i == c2.x() && ncell->j == c2.y())
            {
                //qDebug() << "A* found" << ncell->i << ncell->j;
                last_index = index;
                mFrontier = std::priority_queue<CellRef>();
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

            last_index = index;

            mFrontier.push({ cost, index });
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
#endif
#else
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
#endif

#if 0
#if 0
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

    qDebug() << "A* clipped cells" << c1 << "to" << c2;

    auto cell = mGrid.cell(c1);
    if (!cell)
        return Result::NoPath;

    cell->cost = 0;
    cell->from = -1;
    cell->visited = mSearchNo;

    mFrontier = std::priority_queue<GridCell>();
    mFrontier.push(*cell);

    int w = mGrid.cells().width();
    int h = mGrid.cells().height();

    int last_index = -1;
    while (!mFrontier.empty())
    {
        auto &from = mFrontier.top();
        auto from_index = mGrid.cellIndex(QPoint(from.i, from.j));
        const auto cell = mGrid.cell(from_index);
        mFrontier.pop();
        if (!cell)
            continue;

        qDebug() << "A* visit" << cell->i << cell->j;

        if (cell->i == c2.x() && cell->j == c2.y())
        {
            qDebug() << "A* found" << cell->i << cell->j;
            last_index = from_index;
            break;
        }

        int ni[4][2] = {
            { cell->i > 0 ? cell->i - 1 : -1,     cell->j },
            { cell->i < w - 1 ? cell->i + 1 : -1, cell->j },
            { cell->i,                            cell->j > 0 ? cell->j - 1 : -1 },
            { cell->i,                            cell->j < h - 1 ? cell->j + 1 : -1 }
        };

        for (int i=0; i<4; ++i)
        {
#if 1
            if (ni[i][0] < 0 || ni[i][1] < 0)
                continue;

            auto index = mGrid.cellIndex(QPoint(ni[i][0], ni[i][1]));
            auto ncell = mGrid.cell(index);
            if (!ncell)
                continue;

#if 1
            int user_cost = fn(*ncell);
            if (user_cost < 0)
                continue;
#else
            int user_cost = 0;
#endif

            if (isBlocked(*ncell))
                continue;

            int new_cost = cell->cost + user_cost;
            qDebug() << "A* cost" << cell->i << cell->j << cell->cost;
            if (ncell->visited != mSearchNo || new_cost < ncell->cost)
            {
                ncell->from = from_index;
                ncell->cost = new_cost;
                ncell->visited = mSearchNo;

                last_index = index;

                mFrontier.push(*ncell);
            }
#else 
#endif
        }
    }

    if (last_index < 0)
        return Result::NoPath;

    while (last_index >= 0)
    {
        auto cell = mGrid.cell(last_index);
        qDebug() << "A* raw path" << cell->i << cell->j;
        path.append(mGrid.positionAt(QPoint(cell->i, cell->j)));
        Q_ASSERT(last_index != cell->from);
        last_index = cell->from;
    }

    for (int i=0, j=int(path.size())-1; i<j; ++i, --j)
        std::swap(path[i], path[j]);

    //qDebug() << "A* path" << path;

    return Result::Found;
#else

    path.clear();

    ++mSearchNo;

    auto line = mGrid.clipCellLine(QLine(mGrid.cellAt(p1), mGrid.cellAt(p2)));
    if (line.isNull())
        return Result::NoPath;

    auto c1 = line.p1();
    auto c2 = line.p2();

    //qDebug() << "A* clipped cells" << c1 << "to" << c2;

    auto cell_index = mGrid.cellIndex(c1);
    auto cell = mGrid.cell(cell_index);
    if (!cell)
        return Result::NoPath;

    cell->from = -1;
    cell->visited = mSearchNo;

    //mFrontier = std::priority_queue<CellRef>();
    mFrontier.clear();
    //mFrontier.push({ 0, cell_index });
    mFrontier.push_back({ 0, cell_index });

    int w = mGrid.cells().width();
    int h = mGrid.cells().height();

    int last_index = -1;
    while (!mFrontier.empty())
    {
        //auto from = mFrontier.top();
        auto from = mFrontier.back();
        auto from_index = from.cell_index;
        auto next = mGrid.cell(from_index);
        //mFrontier.pop();
        mFrontier.pop_back();
        if (!next)
            continue;

        if (next->i == c2.x() && next->j == c2.y())
        {
            //qDebug() << "A* found" << next->i << next->j;
            last_index = from_index;
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

            ncell->from = from_index;
            ncell->visited = mSearchNo;

            if (ncell->i == c2.x() && ncell->j == c2.y())
            {
                //qDebug() << "A* found" << ncell->i << ncell->j;
                last_index = index;
                mFrontier.clear();
//                mFrontier = std::priority_queue<CellRef>();
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

            //mFrontier.push({ cost, index });
            mFrontier.push_back({ cost, index });
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
#endif
#endif
}

// ----------------------------------------------------------------------------

bool PathPlanner::isBlocked(const GridCell &cell) const
{
    return cell.usage == CellUsage::Node || cell.usage == CellUsage::Solid;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
