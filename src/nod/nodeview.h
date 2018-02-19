
// ----------------------------------------------------------------------------

#ifndef NOD_NODEVIEW_H
#define NOD_NODEVIEW_H

// ----------------------------------------------------------------------------

#include <QGraphicsView>
#include <QMenu>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

class QUndoStack;

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class AlignDialog;
class CreateNodeDialog;

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

        CreateNode,

        DeleteSelection,

        LayoutGrid,
        LayoutHoriz,
        LayoutVert,

        AlignLeft,
        AlignRight,
        AlignTop,
        AlignBottom,
        AlignDialog,

        Cut,
        Copy,
        Paste,

        Undo,
        Redo,

        Count
    };

    NodeView(QUndoStack *undo, NodeScene *scene, QWidget *parent=nullptr);

    NodeScene                   *scene() const { return mScene; }

    QAction                     *action(Action action) { return mActions[int(action)]; }

    QUndoStack                  *undoStack() const { return mUndo; }

    virtual QMenu               *createMenu();

    virtual AlignDialog         *createAlignDialog();

    virtual CreateNodeDialog    *createCreateNodeDialog();

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

    virtual void                createNode();

    virtual void                deleteSelection();

    virtual void                cut();

    virtual void                copy();

    virtual void                paste();

    virtual void                redo();

    virtual void                undo();

private:

    QUndoStack                  *mUndo = nullptr;

    NodeScene                   *mScene;

    QAction                     *mActions[int(Action::Count)];

    QVector<QGraphicsItem*>     mOrderedSelection;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEVIEW_H

// ----------------------------------------------------------------------------
