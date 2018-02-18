
// ----------------------------------------------------------------------------

#ifndef NOD_DEFAULTCONNECTIONSHAPE_H
#define NOD_DEFAULTCONNECTIONSHAPE_H

// ----------------------------------------------------------------------------

#include <QPainterPath>

// ----------------------------------------------------------------------------

#include "nod/connectionshape.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class DefaultConnectionShape : public ConnectionShape
{
public:

    DefaultConnectionShape(NodeGrid &grid);

    /* ConnectionShape */

    QRectF                      boundingRect() const override { return mBounds; }

    void                        draw(QPainter &painter) override;

    void                        updateGrid() override;

protected:

    void                        updateShape() override;

private:

    QRectF                      mBounds;
    QPainterPath                mCurve;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_DEFAULTCONNECTIONSHAPE_H

// ----------------------------------------------------------------------------
