
// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

bool Connection::isEqual(const Connection &c) const
{
    return (node1.value == c.node1.value && port1.value == c.port1.value &&
            node2.value == c.node2.value && port2.value == c.port2.value) ||
           (node1.value == c.node2.value && port1.value == c.port2.value &&
            node2.value == c.node1.value && port2.value == c.port1.value)
           ;
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------
