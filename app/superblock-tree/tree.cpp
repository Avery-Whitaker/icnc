#include <cnc/cnc.h>
#include <cnc/debug.h>

#define CHUNK_SIZE 500
#define TREE_SIZE 50000000

typedef unsigned long long node_type;

struct tree_context;
struct superblock_manager;

struct tree_step
{
    int execute(const int &t, tree_context &c) const;
};

struct super_tree_step
{
    int execute(const int &t, tree_context &c) const;
};

struct tree_context : public CnC::context<tree_context>
{
    CnC::step_collection<tree_step> m_steps;
    CnC::item_collection<int, node_type> m_nodes;
    CnC::tag_collection<int> m_tags;

    superblock_manager *m_super_manager;

    tree_context();
};

struct superblock_manager : CnC::tag_collection< int >::callback_type
{
    CnC::step_collection<super_tree_step> m_sstep;
    CnC::tag_collection<int> m_stags;
    tree_context &context;

    superblock_manager(tree_context &context) : context(context), m_sstep(context, "sstep"), m_stags(context, "stags")  {
     m_stags.prescribes(m_sstep, context);
     m_sstep.consumes(context.m_nodes);
     m_sstep.produces(context.m_nodes);
}
    virtual bool on_put(const int & tag) override {
//TODO this wont work
      // if(tag < TREE_SIZE/2 - CHUNK_SIZE) {
      //     if(tag % CHUNK_SIZE == 1) {
      //         m_stags.put(tag);
      //     }
      //     return false;
      // }
       return false;
    }
};

void super_tree_step_helper(const int tag, const int depth, tree_context &ctx) {
    if(depth != 0) {
        super_tree_step_helper(tag * 2, depth - 1, ctx);
        super_tree_step_helper(tag * 2 + 1, depth - 1, ctx);
    } else {
        ctx.m_tags.put(tag * 2);
        ctx.m_tags.put(tag * 2 + 1);
    }
}

int super_tree_step::execute(const int &tag, tree_context &ctx) const
{
    //printf("supernode: %d\n", tag);

    super_tree_step_helper(tag, CHUNK_SIZE, ctx);

    return CnC::CNC_Success;
}

int tree_step::execute(const int &tag, tree_context &ctxt) const
{
    //printf("node: %d\n", tag);
    if (tag < TREE_SIZE/2) {
        ctxt.m_tags.put(tag * 2);
        ctxt.m_tags.put(tag * 2 + 1);
    }

    return CnC::CNC_Success;
}

tree_context::tree_context()
        : CnC::context<tree_context>(),
          m_steps(*this),
          m_nodes(*this),
          m_tags(*this)
    {
	m_super_manager = new superblock_manager(*this);
	m_tags.on_put(m_super_manager);

        m_tags.prescribes(m_steps, *this);
        m_steps.consumes(m_nodes);
        m_steps.produces(m_nodes);
    }


int main( int argc, char* argv[] )
{
    // create context
    tree_context ctxt;

    //CnC::debug::trace(ctxt.m_steps);

    ctxt.m_tags.put(1);

    // wait for completion
    ctxt.wait(); 

    return 0;
}

