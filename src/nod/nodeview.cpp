
// ----------------------------------------------------------------------------

#include <QCheckBox>
#include <QContextMenuEvent>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QButtonGroup>
#include <QInputDialog>
#include <QMenu>
#include <QUndoStack>

// ----------------------------------------------------------------------------

#include "nod/aligndialog.h"
#include "nod/connectionitem.h"
#include "nod/createnodedialog.h"
#include "nod/nodeitem.h"
#include "nod/nodeitemfactory.h"
#include "nod/nodescene.h"
#include "nod/nodeview.h"
#include "nod/undo.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeView::NodeView(QUndoStack *undo_stack, NodeScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
      mUndo(undo_stack),
      mScene(scene)
{
    setDragMode(RubberBandDrag);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);

    connect(mScene, &NodeScene::selectionChanged,
            this, &NodeView::selectionChanged);

    // TODO: add more keyboard shortcuts

    QAction *debug = new QAction(tr("&Debug"), this);
    debug->setCheckable(true);
    debug->setChecked(mScene->isDebug());
    connect(debug, &QAction::toggled, this, &NodeView::setDebug);
    mActions[int(Action::DebugEnabled)] = debug;

    auto grid = new QAction(tr("&Grid"), this);
    grid->setCheckable(true);
    grid->setChecked(mScene->drawGrid());
    connect(grid, &QAction::toggled, this, &NodeView::setGrid);
    mActions[int(Action::GridEnabled)] = grid;

    /* create / delete */

    auto create = new QAction(tr("&Create ..."), this);
    create->setShortcut(Qt::Key_Insert);
    connect(create, &QAction::triggered, this, &NodeView::createNode);
    mActions[int(Action::CreateNode)] = create;

    auto del = new QAction(tr("&Delete"), this);
    del->setShortcut(Qt::Key_Delete);
    connect(del, &QAction::triggered, this, &NodeView::deleteSelection);
    mActions[int(Action::DeleteSelection)] = del;

    /* align */

    auto align_top = new QAction(tr("&Top"), this);
    connect(align_top, &QAction::triggered, this, &NodeView::alignTop);
    mActions[int(Action::AlignTop)] = align_top;

    auto align_bottom = new QAction(tr("&Bottom"), this);
    connect(align_bottom, &QAction::triggered, this, &NodeView::alignBottom);
    mActions[int(Action::AlignBottom)] = align_bottom;

    auto align_left = new QAction(tr("&Left"), this);
    connect(align_left, &QAction::triggered, this, &NodeView::alignLeft);
    mActions[int(Action::AlignLeft)] = align_left;

    auto align_right = new QAction(tr("&Right"), this);
    connect(align_right, &QAction::triggered, this, &NodeView::alignRight);
    mActions[int(Action::AlignRight)] = align_right;

    auto align_dialog = new QAction(tr("Align ..."), this);
    connect(align_dialog, &QAction::triggered, this, &NodeView::alignDialog);
    mActions[int(Action::AlignDialog)] = align_dialog;

    /* layout */

    auto layout_horiz = new QAction(tr("&Horizontal"), this);
    connect(layout_horiz, &QAction::triggered, this, &NodeView::layoutHoriz);
    mActions[int(Action::LayoutHoriz)] = layout_horiz;

    auto layout_vert = new QAction(tr("&Vertical"), this);
    connect(layout_vert, &QAction::triggered, this, &NodeView::layoutVert);
    mActions[int(Action::LayoutVert)] = layout_vert;

    auto layout_grid = new QAction(tr("&Grid"), this);
    connect(layout_grid, &QAction::triggered, this, &NodeView::layoutGrid);
    mActions[int(Action::LayoutGrid)] = layout_grid;        


    /* cut, copy, paste */

    auto cut = new QAction(tr("&Cut"), this);
    cut->setShortcut(Qt::CTRL + Qt::Key_X);
    connect(cut, &QAction::triggered, this, &NodeView::cut);
    mActions[int(Action::Cut)] = cut;

    auto copy = new QAction(tr("C&opy"), this);
    copy->setShortcut(Qt::CTRL + Qt::Key_C);
    connect(copy, &QAction::triggered, this, &NodeView::copy);
    mActions[int(Action::Copy)] = copy;

    auto paste = new QAction(tr("&Paste"), this);
    paste->setShortcut(Qt::CTRL + Qt::Key_V);
    connect(copy, &QAction::triggered, this, &NodeView::paste);
    mActions[int(Action::Paste)] = paste;

    /* undo, redo */

    auto undo = undo_stack->createUndoAction(this);
    undo->setShortcut(Qt::CTRL + Qt::Key_Z);
    connect(undo, &QAction::triggered, this, &NodeView::paste);
    mActions[int(Action::Undo)] = undo;

    auto redo = undo_stack->createRedoAction(this);
    redo->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
    connect(redo, &QAction::triggered, this, &NodeView::redo);
    mActions[int(Action::Redo)] = redo;

}

// ----------------------------------------------------------------------------

QMenu *NodeView::createMenu()
{
    QMenu *menu = new QMenu(this);

    menu->addAction(action(Action::CreateNode));
    menu->addAction(action(Action::DeleteSelection));

    menu->addSeparator();

    menu->addAction(action(Action::Cut));
    menu->addAction(action(Action::Copy));
    menu->addAction(action(Action::Paste));

    menu->addSeparator();

    menu->addAction(action(Action::AlignDialog));

    menu->addSeparator();

    auto layout_menu = menu->addMenu(tr("Layout"));
    layout_menu->addAction(action(Action::LayoutHoriz));
    layout_menu->addAction(action(Action::LayoutVert));
    layout_menu->addAction(action(Action::LayoutGrid));

    menu->addSeparator();

    menu->addAction(action(Action::DebugEnabled));
    menu->addAction(action(Action::GridEnabled));

    menu->addSeparator();

    return menu;
}

// ----------------------------------------------------------------------------

AlignDialog *NodeView::createAlignDialog()
{
    return new DefaultAlignDialog(this);
}

// ----------------------------------------------------------------------------

CreateNodeDialog *NodeView::createCreateNodeDialog()
{
    return new DefaultCreateNodeDialog(scene()->itemFactory().nodeFactory(), this);
}

// ----------------------------------------------------------------------------

void NodeView::contextMenuEvent(QContextMenuEvent *event)
{
    QScopedPointer<QMenu> menu(createMenu());
    menu->exec(event->globalPos());
}

// ----------------------------------------------------------------------------

void NodeView::setDebug(bool debug)
{
    mScene->setDebug(debug);
}

// ----------------------------------------------------------------------------

void NodeView::setGrid(bool draw)
{
    mScene->setDrawGrid(draw);
}

// ----------------------------------------------------------------------------

void NodeView::layoutHoriz()
{
    layoutGridColumns(std::numeric_limits<int>::max());
}

// ----------------------------------------------------------------------------

void NodeView::layoutVert()
{
    layoutGridColumns(1);
}

// ----------------------------------------------------------------------------

void NodeView::layoutGrid()
{
    static int columns = 3;

    bool ok;
    int input = QInputDialog::getInt(this, tr("Enter Grid Size"), tr("Columns"), columns, 1, 32, 1, &ok);
    if (ok)
    {
        columns = input;
        layoutGridColumns(columns);
    }
}

// ----------------------------------------------------------------------------

void NodeView::layoutGridColumns(int columns)
{
    if (!scene())
        return;

    auto spacing = LayoutSpacing * mScene->grid().gridSize();

    QRectF bounds;
    for (auto item : mOrderedSelection)
        bounds = bounds.united(item->sceneBoundingRect());

    qDebug() << "NodeView: selection rect" << bounds;

    auto x = bounds.x();
    auto y = bounds.y();

    int column = 0;
    int row_height = 0;

    for (auto item : mOrderedSelection)
    {
        auto item_rc = item->sceneBoundingRect();
        if (item_rc.height() > row_height)
            row_height = item_rc.height();

        if (column == columns)
        {
            column = 0;
            x = bounds.x();
            y += spacing + row_height;
        }

        item->setPos(x, y);
        x += spacing + item_rc.width();

        column++;
    }
}

// ----------------------------------------------------------------------------

void NodeView::align(Qt::Alignment alignment)
{
    if (mOrderedSelection.isEmpty())
        return;

    auto to = mOrderedSelection.back();
    auto to_rc = to->sceneBoundingRect();

    for (int i=0; i<mOrderedSelection.size()-1; ++i)
    {
        auto item = mOrderedSelection[i];
        auto item_rc = item->sceneBoundingRect();

        auto x = item_rc.x();
        auto y = item_rc.y();

        if (alignment & Qt::AlignRight)
        {
            x = to_rc.right();
        }

        item->setPos(x, y);
    }
}

// ----------------------------------------------------------------------------

void NodeView::alignDialog()
{
    QScopedPointer<AlignDialog> dlg(createAlignDialog());
    if (dlg && dlg->exec() == QDialog::Accepted)
        align(dlg->alignment());
}

// ----------------------------------------------------------------------------

void NodeView::alignLeft()
{
    align(Qt::AlignLeft);
}

// ----------------------------------------------------------------------------

void NodeView::alignTop()
{
    align(Qt::AlignTop);
}

// ----------------------------------------------------------------------------

void NodeView::alignRight()
{
    align(Qt::AlignRight);
}

// ----------------------------------------------------------------------------

void NodeView::alignBottom()
{
    align(Qt::AlignBottom);
}

// ----------------------------------------------------------------------------

void NodeView::selectionChanged()
{
    // note that it is safe even with multiple views, all UI code runs in the
    // same thread and a signle thread cannot call selectionChanged() twice.
    // actually it can, that's what guard is for.


    qDebug() << "NodeView: selection changed";

    static bool guard = false;
    if (!guard)
    {
        auto selected = scene()->selectedItems();

        bool has_nodes = false;
        bool has_conns = false;

        for (auto item : selected)
        {
            if (qgraphicsitem_cast<NodeItem*>(item))
            {
                has_nodes = true;
            } else
            if (qgraphicsitem_cast<ConnectionItem*>(item))
                has_conns = true;
        }

        bool sync = false;

        for (auto it=mOrderedSelection.begin(); it!=mOrderedSelection.end(); )
        {
            bool remove = !(*it)->isSelected();
            if (!remove)
            {
                if (has_nodes)
                {
                    if (!qgraphicsitem_cast<NodeItem*>(*it))
                        remove = true;
                } else
                if (has_conns)
                {
                    if (!qgraphicsitem_cast<ConnectionItem*>(*it))
                        remove = true;
                }

                if (remove)
                    sync = true;
            }

            if (remove)
            {
                it = mOrderedSelection.erase(it);
            } else
                ++it;
        }

        for (auto item : selected)
        {
            if (!mOrderedSelection.contains(item))
                mOrderedSelection.append(item);
        }

        if (sync)
        {
            guard = true;

            qDebug() << "NodeView: sync selected";

            mScene->clearSelection();

            for (auto item : selected)
                item->setSelected(true);

            guard = false;
        }

        bool has_selection = !mOrderedSelection.isEmpty();
        action(Action::AlignBottom)->setEnabled(has_selection);
        action(Action::AlignLeft)->setEnabled(has_selection);
        action(Action::AlignRight)->setEnabled(has_selection);
        action(Action::AlignTop)->setEnabled(has_selection);
        action(Action::AlignDialog)->setEnabled(has_selection);

        action(Action::DeleteSelection)->setEnabled(has_selection);

        action(Action::LayoutGrid)->setEnabled(has_selection);
        action(Action::LayoutHoriz)->setEnabled(has_selection);
        action(Action::LayoutVert)->setEnabled(has_selection);

        action(Action::Cut)->setEnabled(has_selection);
        action(Action::Copy)->setEnabled(has_selection);
    }
}

// ----------------------------------------------------------------------------

void NodeView::createNode()
{
    QScopedPointer<CreateNodeDialog> dlg(createCreateNodeDialog());
    if (dlg->exec() != QDialog::Accepted)
        return;

    auto type = dlg->nodeType();
    if (type.isValid())
    {
        // TODO: use command
        scene()->itemFactory().nodeFactory().createNode(*scene()->model(), type);
    }
}

// ----------------------------------------------------------------------------

void NodeView::deleteSelection()
{
    if (scene())
        mUndo->push(new DeleteSelectionCommand(*scene()));
}

// ----------------------------------------------------------------------------

void NodeView::cut()
{

}

// ----------------------------------------------------------------------------

void NodeView::copy()
{

}

// ----------------------------------------------------------------------------

void NodeView::paste()
{

}

// ----------------------------------------------------------------------------

void NodeView::redo()
{
    if (mUndo)
        mUndo->redo();
}

// ----------------------------------------------------------------------------

void NodeView::undo()
{
    if (mUndo)
        mUndo->undo();
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
