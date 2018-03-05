
// ----------------------------------------------------------------------------

#ifndef NOD_PATHPLANNER_H
#define NOD_PATHPLANNER_H

// ----------------------------------------------------------------------------

#include <deque>
#include <functional>
#include <queue>

// ----------------------------------------------------------------------------

#include <QPointF>
#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

struct GridCell;
class NodeGrid;

// ----------------------------------------------------------------------------

struct CellRef
{
    int                         priority;
    int                         cell_index;
};

// ----------------------------------------------------------------------------

/**
 * Costs:
 *  * negative cost if close to node (less ankward paths)
 *  * cell occupied by a connection (minimal cost, so that lines overlap when needed)
 *  * number of bends in the path (moderate cost to avoid zig-zag lines)
 *  * distance from start (don't be stupid)
 *  * cell occupied by a node (infinite cost, can't cross)
 */
class PathPlanner
{
public:

    enum class Result
    {
        NoPath, // no points
        Blocked, // last path point is blocker
        Found // last point is p2
    };

    PathPlanner(NodeGrid &grid);

    Result                      plan(QVector<QPointF> &path, const QPointF &p1, const QPointF &p2, std::function<int (const GridCell &)> fn);

private:

    NodeGrid                    &mGrid;
    int                         mSearchNo = 0;

    std::priority_queue<CellRef> mFrontier;
    //std::deque<CellRef> mFrontier;
    //std::deque<int>             mFrontier;

    bool                        isBlocked(const GridCell &cell) const;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_PATHPLANNER_H

// ----------------------------------------------------------------------------

