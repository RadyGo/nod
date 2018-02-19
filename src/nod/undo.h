
// ----------------------------------------------------------------------------

#ifndef NOD_UNDO_H
#define NOD_UNDO_H

// ----------------------------------------------------------------------------

#include <QUndoCommand>

// ----------------------------------------------------------------------------

#include "nod/nodescene.h"
#include "nod/serialized.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class CreateNodeCommand : public QUndoCommand
{
public:

    CreateNodeCommand(NodeScene &scene, const NodeTypeID &type, const NodeID &id, const QPoint &grid_position);

    void                redo() override;

    void                undo() override;

private:

    NodeTypeID          mType;
    NodeID              mNodeID;
    QPointF             mPosition;
};

// ----------------------------------------------------------------------------

class DeleteSelectionCommand : public QUndoCommand
{
public:

    DeleteSelectionCommand(NodeScene &scene);

    void                redo() override;

    void                undo() override;

private:

    Serialized          mData;
};

// ----------------------------------------------------------------------------

class UpdateNodeDataCommand : public QUndoCommand
{
public:

    UpdateNodeDataCommand(NodeScene &scene, const NodeID &node, const QVariant &value, DataRole role);

    void                redo() override;

    void                undo() override;

private:

    NodeID              mNodeID;
    DataRole            mRole;
    QVariant            mOld;
    QVariant            mNew;
};

// ----------------------------------------------------------------------------

class UpdatePortDataCommand : public QUndoCommand
{
public:

    UpdatePortDataCommand(NodeScene &scene, const NodeID &node, const PortID &port, const QVariant &value, DataRole role);

    void                redo() override;

    void                undo() override;

private:

    NodeID              mNodeID;
    PortID              mPortID;
    DataRole            mRole;
    QVariant            mOld;
    QVariant            mNew;
};

// ----------------------------------------------------------------------------

class CreateConnectionCommand : public QUndoCommand
{
public:

private:

    Connection          mConnection;
};

// ----------------------------------------------------------------------------

class DeleteConnectionCommand : public QUndoCommand
{
public:

private:

    Connection          mConnection;
};

// ----------------------------------------------------------------------------

class SelectionCommand : public QUndoCommand
{
public:

private:

    Connection          mConnection;
};

// ----------------------------------------------------------------------------

class AlignNodesCommand : public QUndoCommand
{

};

// ----------------------------------------------------------------------------

class LayoutNodesCommand : public QUndoCommand
{

};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_UNDO_H

// ----------------------------------------------------------------------------
