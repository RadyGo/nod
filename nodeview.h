
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

    enum class Action
    {
        DebugEnabled,
        GridEnabled,

        LayoutGrid,
        LayoutHoriz,
        LayoutVert,

        Count
    };

    NodeView(NodeScene *scene, QWidget *parent=nullptr);

    QAction                     *action(Action action) { return mActions[int(action)]; }

    /* QGraphicsView */

    void                        contextMenuEvent(QContextMenuEvent *event) override;

public slots:

    virtual void                setDebug(bool debug);

    virtual void                setGrid(bool draw);

    virtual void                layoutHoriz();

    virtual void                layoutVert();

    virtual void                layoutGrid();

    virtual void                layoutGridColumns(int columns);

private:

    NodeScene                   *mScene;

    QAction                     *mActions[int(Action::Count)];
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEVIEW_H

// ----------------------------------------------------------------------------
