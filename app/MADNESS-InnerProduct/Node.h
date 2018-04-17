#ifndef NODE_H_
#define NODE_H_

#include "Vector.h"
#include <utility>
#include <sstream>


class Node {
public:
   int n;
   int l;
   Vector s;
   Vector d;
   bool has_children;

   int k;   

   Node() : n(0), l(0),  k(0),  s(k), d(k), has_children(false) {}
  
   Node(int n, int l, int k, const Vector &s_input, const Vector &d_input, bool has_children)
   : n(n), l(l), k(k), s(s_input), d(d_input), has_children(has_children) {}

   Node(const Node &node)
   : n(node.n), l(node.l), k(node.k), s(node.s), d(node.d), has_children(node.has_children) {}

   std::string toString() {
      std::ostringstream output;
      output << "Node with key(" << n << ", " << l << "), S Vector: " << s << ", d Vector: " << d << ", has_children: " << has_children;
      return output.str();
   }
   
};
#endif
