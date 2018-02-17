
// ----------------------------------------------------------------------------

#ifndef NOD_COMMON_H
#define NOD_COMMON_H

// ----------------------------------------------------------------------------

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QUuid>

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

Q_NAMESPACE

// ----------------------------------------------------------------------------

enum class Direction
{
    Input,
    Output
};

Q_ENUM_NS(Direction);

// ----------------------------------------------------------------------------

template <int T>
struct ID
{
    /// Uniquely identifies this node, must always be set. The framework uses
    /// this to manage connections.
    QUuid               value;

    union
    {
        /// User can store an index for quick access.
        qint64          ivalue;
        /// User can store a pointer for quick access.
        void            *pvalue;
        /// User can store a pointer for quick access.
        QObject         *qvalue;
    };

    static ID<T>        invalid() { return { QUuid(), { 0 } }; }

    bool                isValid() const { return !value.isNull(); }

    bool                operator==(const ID<T> &other) const { return value == other.value; }

    bool                operator!=(const ID<T> &other) const { return !operator==(other); }
};

// ----------------------------------------------------------------------------

using AttributeID       = ID<0>;
using NodeID            = ID<1>;
using NodeTypeID        = ID<2>;
using PortID            = ID<3>;

// ----------------------------------------------------------------------------

enum class DataRole
{
    Display,
    Name,
    Icon,
    Tooltip,

    PortColor,

    Maximum,
    Minimum,
    SingleStep,
    PageStep,

    SelectedIndex,

    Position,
    Size,

    Value,

    User                = 64
};

// ----------------------------------------------------------------------------

enum class NodeFlag
{
    None                = 0x00000000,
    Editable            = 0x00000001
};

// ----------------------------------------------------------------------------

Q_DECLARE_FLAGS(NodeFlags, nod::NodeFlag)
Q_FLAG_NS(NodeFlags)

// ----------------------------------------------------------------------------

enum class EditorHint
{
    None                = 0x00000000,
    Static              = 0x00000001, // read-only representation
    LineEdit            = 0x00000002,
    TextEdit            = 0x00000004,
    FilePath            = 0x00000008,
    DirPath             = 0x00000010,
    SpinBox             = 0x00000020,
    ListBox             = 0x00000040,
    Slider              = 0x00000080,
    ComboBox            = 0x00000100
};

// ----------------------------------------------------------------------------

Q_DECLARE_FLAGS(EditorHints, nod::EditorHint)
Q_FLAG_NS(EditorHints)

// ----------------------------------------------------------------------------

struct Connection
{
    NodeID                  node1;
    PortID                  port1;
    NodeID                  node2;
    PortID                  port2;

    bool                    isEqual(const Connection &c) const;

    bool                    contains(const NodeID &node) const { return (node.value == node1.value) || (node.value == node2.value); }

    bool                    contains(const NodeID &node, const PortID &port) const { return (node.value == node1.value && port.value == port1.value) || (node.value == node2.value && port.value == port2.value); }
};

// ----------------------------------------------------------------------------

class NodeModel;

// ----------------------------------------------------------------------------

namespace qgs {

// ----------------------------------------------------------------------------

Q_NAMESPACE

// ----------------------------------------------------------------------------

class ConnectionItem;
class NodeScene;
class NodeGrid;
class NodeItem;
class NodeItemFactory;

// ----------------------------------------------------------------------------

} // namespace qgs

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------

Q_DECLARE_OPERATORS_FOR_FLAGS(nod::NodeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(nod::EditorHints)

// ----------------------------------------------------------------------------

#endif // COMMON_H

// ----------------------------------------------------------------------------
