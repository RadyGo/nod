
// ----------------------------------------------------------------------------

#ifndef NOD_NODEVIEW_H
#define NOD_NODEVIEW_H

// ----------------------------------------------------------------------------

#include <QGraphicsView>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class NodeView : public QGraphicsView
{
    Q_OBJECT
public:

    NodeView(NodeScene *scene, QWidget *parent=nullptr);

    /* QGraphicsView */

    void                        contextMenuEvent(QContextMenuEvent *event) override;

protected slots:

    virtual void                setDebug(bool debug);

    virtual void                setGrid(bool draw);

private:

    NodeScene                   *mScene;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEVIEW_H

// ----------------------------------------------------------------------------
