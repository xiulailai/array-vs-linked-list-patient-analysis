// Node structure used by the linked list.
#ifndef NODE_HPP
#define NODE_HPP

#include "Patient.hpp"

struct Node {
    Patient data;
    Node* next;
};

#endif