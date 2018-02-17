
// ----------------------------------------------------------------------------

#ifndef NOD_NODEVIEW_H
#define NOD_NODEVIEW_H

// ----------------------------------------------------------------------------

#include <QGraphicsView>
#include <QMenu>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class NodeView : public QGraphicsView
{
    Q_OBJECT
public:

    enum
    {
        LayoutSpacing             = 2
    };

    enum class Action
    {
        DebugEnabled,
        GridEnabled,

        LayoutGrid,
        LayoutHoriz,
        LayoutVert,

        AlignLeft,
        AlignRight,
        AlignTop,
        AlignBottom,
        AlignDialog,

        Count
    };

    NodeView(NodeScene *scene, QWidget *parent=nullptr);

    QAction                     *action(Action action) { return mActions[int(action)]; }

    virtual QMenu               *createMenu();

    /* QGraphicsView */

    void                        contextMenuEvent(QContextMenuEvent *event) override;

public slots:

    virtual void                setDebug(bool debug);

    virtual void                setGrid(bool draw);

    virtual void                layoutHoriz();

    virtual void                layoutVert();

    virtual void                layoutGrid();

    virtual void                layoutGridColumns(int columns);

    // align to last item in the selection
    virtual void                align(Qt::Alignment alignment);

    virtual void                alignDialog();

    virtual void                alignLeft();

    virtual void                alignTop();

    virtual void                alignRight();

    virtual void                alignBottom();

    virtual void                selectionChanged();

private:

    NodeScene                   *mScene;

    QAction                     *mActions[int(Action::Count)];

    QVector<QGraphicsItem*>     mOrderedSelection;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEVIEW_H

// ----------------------------------------------------------------------------
