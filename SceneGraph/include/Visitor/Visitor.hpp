/*
* @Author: gpinchon
* @Date:   2021-06-26 00:56:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:45
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Node;
class NodeGroup;
class Renderable;
class Object;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Visitor {
public:
    virtual void operator()(Object&) {};
    virtual void operator()(Node&) {};
    virtual void operator()(NodeGroup&) {};
    virtual void operator()(Renderable&) {};
};
};