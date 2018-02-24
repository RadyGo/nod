
// ----------------------------------------------------------------------------

#include "nod/nodegrid.h"

// ----------------------------------------------------------------------------

#include "nod/connectionshape.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

ConnectionShape::ConnectionShape(NodeGrid &grid)
    : mGrid(grid)
{

}

// ----------------------------------------------------------------------------

ConnectionShape::~ConnectionShape()
{

}

// ----------------------------------------------------------------------------

void ConnectionShape::updatePath(const QPointF &start, const QPointF &end)
{
    // TODO: use A* planner
#if 1
    mPath.clear();
    mGrid.planner().plan(mPath, start, end, [] (const GridCell &cell) -> int {
        return 0;
    });
#else
    mPath.clear();
    mPath.push_back(start);
    mPath.push_back(end);
#endif

    updateShape();
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
