
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
#if 0
    mPath.clear();
    mGrid.path(mPath, start, end);
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
