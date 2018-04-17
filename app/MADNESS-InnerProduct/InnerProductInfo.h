#ifndef _INNER_PRODUCT_INFO
#define _INNER_PRODUCT_INFO


class InnerProductInfo {
public:

   int i;
   int j;

   int n;
   int l;

   InnerProductInfo(int iInput, int jInput, int nInput, int lInput): i(iInput), j(jInput), n(nInput), l(lInput) {}
   InnerProductInfo(const InnerProductInfo &node): i(node.i), j(node.j), n(node.n), l(node.l) {}
 
};

#endif
