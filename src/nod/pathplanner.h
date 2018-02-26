
// ----------------------------------------------------------------------------

#ifndef NOD_PATHPLANNER_H
#define NOD_PATHPLANNER_H

// ----------------------------------------------------------------------------

#include <deque>
#include <functional>

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

/**
 * Costs:
 *  * cell occupied by a connection
 *  * cell occupied by a node
 *  * number of bends in the path
 *  * distance from start
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
    std::deque<int>             mFrontier;

    bool                        isBlocked(const GridCell &cell) const;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_PATHPLANNER_H

// ----------------------------------------------------------------------------

