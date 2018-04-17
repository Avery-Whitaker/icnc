#ifndef _TREE_NODE_INFO
#define _TREE_NODE_INFO

class TreeNodeInfo {
public:
   int t;
   int n;
   int l;

   TreeNodeInfo(int tInput, int nInput, int lInput): t(tInput), n(nInput), l(lInput) {}
   TreeNodeInfo(const TreeNodeInfo &node): t(node.t), n(node.n), l(node.l) {}
 
};

#endif
