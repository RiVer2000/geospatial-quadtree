#ifndef NODE_HPP
#define NODE_HPP

#include <coordinates.hpp>

struct Node 
{
    coordinates location;
    int data;

    Node(coordinates loc, int d) : location(loc), data(d) {}
    Node() : location(coordinates()), data(0) {}
};

#endif // NODE_HPP
