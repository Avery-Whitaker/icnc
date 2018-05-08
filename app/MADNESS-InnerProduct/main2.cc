#include <cnc/cnc.h>
#include <utility>
#include <iostream>

#include <cmath>
#include <stdlib.h>

#include <vector>
	
/*#include <cnc/debug.h>*/

#include <sched.h>
#include <stdio.h>

#include "Vector.h"
#include "Matrix.h"
#include "TreeNodeInfo.h"
#include "InnerProductInfo.h"
#include "Node.h"
#include "Twoscalecoeffs.h"
#include "Quadrature.h"
#include "tbb/concurrent_vector.h"

using namespace std;


struct MADNESSCnCContext;

struct Project {
   Vector sValues(int nInput, int lInput, int l, MADNESSCnCContext &context) const;
   int execute(const TreeNodeInfo &node, MADNESSCnCContext &context) const;
};

struct Compress {
   int execute(const TreeNodeInfo &node, MADNESSCnCContext &context) const;
};

struct InnerProduct {
   int execute(const InnerProductInfo &node, MADNESSCnCContext &context) const;
};

struct MADNESSCnCContext : public CnC::context<MADNESSCnCContext> {
   
   // Required Data Members for the initialization

   int k;
   double thresh;
   Matrix * hg;
   Matrix * hg0;
   Matrix * hg1;
   Matrix * hgT;
   Matrix * rm;
   Matrix * r0;
   Matrix * rp;
   Vector * quad_w;
   Vector * quad_x;
   int quad_npt;
   Matrix * quad_phi;
   Matrix * quad_phiT;
   Matrix * quad_phiw;
   int max_level;
   int n;

   // https://stackoverflow.com/questions/15128444/c-calling-a-function-from-a-vector-of-function-pointers-inside-a-class-where-t
   vector< double (*)(double)> functions;
   // It is nxn matrix, containing the coefficients for A[i][j]
   tbb::concurrent_vector<double> results;

   // collections
   // step_collections
   CnC::step_collection<Project> project_step;
   CnC::step_collection<Compress> compress_step;
   CnC::step_collection<InnerProduct> innerProduct_step;

   // item_collections
   CnC::item_collection<std::pair<int, std::pair<int, int>>, Node> leaves;
   CnC::item_collection<std::pair<int, std::pair<int, int>>, Node> nodes;

   // tag_collections
   CnC::tag_collection<TreeNodeInfo> project_tag;
   CnC::tag_collection<TreeNodeInfo> compress_tag;
   CnC::tag_collection<InnerProductInfo> innerProduct_tag;

   MADNESSCnCContext(int k, double thresh, int max_level, vector< double (*)(double)> functions)
   : CnC::context<MADNESSCnCContext>(), k(k), thresh(thresh), max_level(max_level),
     n(functions.size()), functions(functions), results(n * n, 0.0),
     project_step(*this, "project_step"), compress_step(*this, "compress_step"),
     innerProduct_step(*this, "innerProduct_step"), leaves(*this), nodes(*this), project_tag(*this), compress_tag(*this), innerProduct_tag(*this) {

      project_tag.prescribes(project_step, *this);
      compress_tag.prescribes(compress_step, *this);
      innerProduct_tag.prescribes(innerProduct_step, *this);

      project_step.produces(leaves);
      project_step.produces(nodes);

      compress_step.consumes(leaves);
      compress_step.produces(leaves);
      compress_step.produces(nodes);

      innerProduct_step.consumes(nodes);
      innerProduct_step.produces(nodes);

      init_twoscale(k);
      init_quadrature(k);
      make_dc_periodic();
   }
    
   void init_twoscale(int k) {
      double  (*hgInput)[22] = twoscalecoeffs(k);

      hg = new Matrix(2*k, 2*k);
      hg0 = new Matrix(2*k, k);
      hg1 = new Matrix(2*k, k);
      hgT = new Matrix(2*k, 2*k);

      for (int i = 0; i < 2 * k; ++i) {
         for (int j = 0; j < 2 * k; ++j) {
            hg->set_item(i, j, hgInput[i][j]);
            hgT->set_item(i, j, hgInput[j][i]);
         }
      }

      for (int i = 0; i < 2 * k; ++i) {
         for (int j = 0; j < k; ++j) {
            hg0->set_item(i, j, hgInput[i][j]);
            hg1->set_item(i, j, hgInput[i][j+k]);
         }
      }
   }

   void init_quadrature(int order) {
      double *x = gauss_legendre_point(order);
      double *w = gauss_legendre_weight(order);

      quad_w = new Vector(w, 0, order);
      quad_x = new Vector(x, 0, order);

      int npt = order;
      quad_npt = npt;

      quad_phi = new Matrix(npt, k);
      quad_phiT = new Matrix(k, npt);
      quad_phiw = new Matrix(npt, k);

      for (int i = 0; i < npt; ++i) {
         double * p = phi((*quad_x)[i], k);
         for (int m = 0; m < k; ++m) {
            quad_phi->set_item(i, m, p[m]);
            quad_phiT->set_item(m, i, p[m]);
            quad_phiw->set_item(i, m, w[i] * p[m]);
         }
      }
   }

   void make_dc_periodic() {
      rm = new Matrix(k, k);
      r0 = new Matrix(k, k);
      rp = new Matrix(k, k);

      double iphase = 1.0;
      for (int i = 0; i < k; ++i) {
         double jphase = 1.0;

         for (int j = 0; j < k; ++j) {
            double gammaij = sqrt(( 2 * i + 1) * ( 2 * j + 1));
            double Kij;
            if ((( i -  j ) > 0) && (((i - j ) % 2) == 1 )) {
               Kij = 2.0;
            } else {
               Kij = 0.0;
            }

            r0->set_item(i, j, (0.5 * (1.0 - iphase * jphase - 2.0 * Kij) * gammaij));
            rm->set_item(i, j, (0.5 * jphase * gammaij));
            rp->set_item(i, j, (-0.5 * iphase * gammaij));

	    jphase = -1 * jphase;
         }
         iphase = -1 * iphase;
      }
   }

};

Vector Project::sValues(int nInput, int lInput, int l, MADNESSCnCContext &context) const {
   Vector s(context.k);
   Vector &quad_x_ref = *(context.quad_x);
   Matrix &quad_phiw_ref = *(context.quad_phiw);

   double h = pow(0.5, nInput);
   double scale = sqrt(h);
   for (int mu = 0; mu < context.quad_npt; ++mu) {
      double x = (lInput + quad_x_ref[mu]) * h;
      double fValue = context.functions[l](x);
      for (int i = 0; i < (context.k); ++i) {
         s[i] = s[i] + (scale * fValue * (quad_phiw_ref.get_item(mu, i)));
      }
   }
   return s;
}

int Project::execute(const TreeNodeInfo &node, MADNESSCnCContext &context) const {
    //printf("Project: tag %d, core %d\n", node.l, sched_getcpu());
    Vector s0 = sValues(node.n + 1, 2 * node.l, node.t, context);
    Vector s1 = sValues(node.n + 1, 2 * node.l + 1, node.t, context);

   int k = context.k;
   Vector s(s0 | s1);
   Vector d(s * (* context.hgT));

   // if the error is less than the threshhold or we have reached max_level
   if (d.normf(k, 2*k) < context.thresh ||  node.n >= context.max_level - 1) {
printf("reached depth of %d\n", node.n);
      context.leaves.put(make_pair(node.t, make_pair(node.n + 1, node.l * 2)), Node(node.n + 1, node.l * 2, k, s0, Vector(), false));
      context.leaves.put(make_pair(node.t, make_pair(node.n + 1, node.l * 2 + 1)), Node(node.n + 1, node.l * 2 + 1, k, s1, Vector(), false));

      context.nodes.put(make_pair(node.t, make_pair(node.n + 1, node.l * 2)), Node(node.n + 1, node.l * 2, k, Vector(), Vector(), false));
      context.nodes.put(make_pair(node.t, make_pair(node.n + 1, node.l * 2 + 1)), Node(node.n + 1, node.l * 2, k, Vector(), Vector(), false));
            
      for (int j = node.t; j < context.n; ++j) {
          context.innerProduct_tag.put(InnerProductInfo(node.t, j, node.n + 1, node.l * 2));
          context.innerProduct_tag.put(InnerProductInfo(node.t, j, node.n + 1, node.l * 2 + 1));
      }
      
      context.compress_tag.put(TreeNodeInfo(node.t, node.n, node.l));
    }
    else {
       context.project_tag.put(TreeNodeInfo(node.t, node.n + 1, node.l * 2));
       context.project_tag.put(TreeNodeInfo(node.t, node.n + 1, node.l * 2 + 1));
    }

    return CnC::CNC_Success;
}

int Compress::execute(const TreeNodeInfo &node, MADNESSCnCContext &context) const {

    //printf("Compress: tag %d, core %d\n", node.l, sched_getcpu());
   Node left;
   Node right;
   int k = context.k;
   context.leaves.get(make_pair(node.t, make_pair(node.n + 1, node.l * 2)), left);
   context.leaves.get(make_pair(node.t, make_pair(node.n + 1, node.l * 2 + 1)), right);
   

   Vector s(left.s | right.s);
   Vector d = s * (*context.hgT);

   Vector sValue(d.data, 0, k);
   Vector dValue(d.data, k, 2 * k);

   if (node.n == 0) { // if it is the root
      context.nodes.put(make_pair(node.t, make_pair(node.n, node.l)), Node(node.n, node.l, k, sValue, dValue, true));
   }
   else {

      context.nodes.put(make_pair(node.t, make_pair(node.n, node.l)), Node(node.n, node.l, k, Vector(), dValue, true));
      context.leaves.put(make_pair(node.t, make_pair(node.n, node.l)), Node(node.n, node.l, k, sValue, Vector(), true));
      
      if (node.l % 2 == 0) { // only left child wakes up it's parent to be trigerred
         context.compress_tag.put(TreeNodeInfo(node.t, node.n-1, node.l/2));
      }
   }

   for (int j = node.t; j < context.n; ++j) {
      context.innerProduct_tag.put(InnerProductInfo(node.t, j, node.n, node.l));
   }

   return CnC::CNC_Success;
}

int InnerProduct::execute(const InnerProductInfo &node, MADNESSCnCContext &context) const {

   Node left;
   Node right;

   context.nodes.get(make_pair(node.i, make_pair(node.n, node.l)), left);
   context.nodes.get(make_pair(node.j, make_pair(node.n, node.l)), right);

   int k = context.k;

   if (left.has_children && right.has_children) {
      //lock_guard<mutex> guard(context.locks[node.i][node.j]);
      context.results[node.i * context.n + node.j] += left.d.inner(right.d);

      if (node.n == 0) { // It is the root
         context.results[node.i * context.n + node.j] += left.s.inner(left.s);
      }
   }
   else if (left.has_children) {

      context.nodes.put(make_pair(node.j, make_pair(node.n + 1, node.l * 2)), Node(node.n + 1, node.l * 2, k, Vector(), Vector(), false));
      context.nodes.put(make_pair(node.j, make_pair(node.n + 1, node.l * 2 + 1)), Node(node.n + 1, node.l * 2 + 1, k, Vector(), Vector(), false));

      // No Need to trigger anything as it will be automatically triggered !
      
   }
   else if (right.has_children) {
      
      context.nodes.put(make_pair(node.i, make_pair(node.n + 1, node.l * 2)), Node(node.n + 1, node.l * 2, k, Vector(), Vector(), false));
      context.nodes.put(make_pair(node.i, make_pair(node.n + 1, node.l * 2 + 1)), Node(node.n + 1, node.l * 2 + 1, k, Vector(), Vector(), false));

      // Required to be triggered !
      context.innerProduct_tag.put(InnerProductInfo(node.i, node.j, node.n + 1, node.l * 2));
      context.innerProduct_tag.put(InnerProductInfo(node.i, node.j, node.n + 1, node.l * 2 + 1));
   }
}

double gaussian(double x, double a, double coeff) {
    return coeff*exp(-a*x*x);
}

double test1(double x) {
    static const int N = 100;
    static double a[N], X[N], c[N];
    static bool initialized = false;

    if (!initialized) {
        for (int i=0; i<N; i++) {
            a[i] = 1000*drand48();
            X[i] = drand48();
            c[i] = pow(2*a[i]/M_PI,0.25); 
        }
        initialized = true;
    }

    double sum = 0.0;
    for (int i=0; i<N; i++) sum += gaussian(x-X[i], a[i], c[i]);
    return sum;
}

double test2(double x) {
    static const int N = 100;
    static double a[N], X[N], c[N];
    static bool initialized = false;

    if (!initialized) {
        for (int i=0; i<N; i++) {
            a[i] = 1000*drand48();
            X[i] = drand48();
            c[i] = pow(2*a[i]/M_PI,0.25); 
        }
        initialized = true;
    }

    double sum = 0.0;
    for (int i=0; i<N; i++) sum += gaussian(x-X[i], a[i], c[i]);
    return sum;
}

int main(int argc, char* argv[]) {

   int k = 6;
   int npt = 20;
   //int max_level = atoi(argv[1]);
   int max_level = 100000000;
   //double thresh = atof(argv[2]);
   double thresh = 0.000000000001;

   int N = 1;

   //high_resolution_clock::time_point t1 = high_resolution_clock::now();

   vector<double (*)(double)> functions;

   for(int i = 0; i < N; i++) {
       if(drand48() > 0.5)
           functions.push_back(test1);
       else
           functions.push_back(test2);
   }

   // int k, double thresh, int max_level, int n, vector< double (*)(double)> functions
   //

   MADNESSCnCContext context(k, thresh, max_level, functions);

   for (int i = 0; i < context.n; ++i) {
       context.project_tag.put(TreeNodeInfo(i, 0, 0));
   }

   context.wait();

	
   //high_resolution_clock::time_point t2 = high_resolution_clock::now();
   //auto duration = duration_cast<microseconds>( t2 - t1 ).count();
   //cout << duration/1000000.0 << endl;

}

/*
export CNC_NUM_THREADS=4
source /opt/intel/cnc/1.0.100/bin/cncvars.sh
g++ -std=c++11 -pthread -O3 Vector.cpp Matrix.cpp Function.cpp -o test1 -L/opt/intel/cnc/1.0.100/lib/intel64 -lcnc -lrt -ltbb -ltbbmalloc

*/
