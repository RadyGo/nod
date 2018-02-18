
// ----------------------------------------------------------------------------

#ifndef NOD_UNDO_H
#define NOD_UNDO_H

// ----------------------------------------------------------------------------

#include <QUndoCommand>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class CreateNodeCommand : public QUndoCommand
{
public:

    CreateNodeCommand(NodeModel &model, const NodeID &id, const QPoint &grid_position);

    void                redo() override;

    void                undo() override;

private:

    Serialized          mData;
};

// ----------------------------------------------------------------------------

class DeleteNodeCommand : public QUndoCommand
{
public:

    DeleteNodeCommand(NodeModel &model, const NodeID &id, const QPoint &grid_position);

    void                redo() override;

    void                undo() override;

private:

    Serialized          mData;
};

// ----------------------------------------------------------------------------

class UpdateNodeDataCommand : public QUndoCommand
{
public:

    UpdateNodeDataCommand(NodeModel &model, const NodeID &node, const QVariant &value, DataRole role);

    void                redo() override;

    void                undo() override;

private:

    Serialized          mData;
};

// ----------------------------------------------------------------------------

class CreateConnectionCommand : public QUndoCommand
{
public:

};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_UNDO_H

// ----------------------------------------------------------------------------
