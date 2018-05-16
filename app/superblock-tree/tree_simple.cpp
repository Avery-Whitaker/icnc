#include <cnc/cnc.h>
#include <cnc/debug.h>

#define TREE_SIZE 10

typedef unsigned long long node_type;

struct tree_context;

struct tree_step
{
	int execute(const int &t, tree_context &c) const;
};

struct tree_context : public CnC::context<tree_context>
{
	CnC::step_collection<tree_step> m_steps;
	CnC::item_collection<int, node_type> m_nodes;
	CnC::tag_collection<int> m_tags;

	tree_context()
		: CnC::context<tree_context>(),
		m_steps(*this),
		m_nodes(*this),
		m_tags(*this)
	{
		m_tags.prescribes(m_steps, *this);
		m_steps.consumes(m_nodes);
		m_steps.produces(m_nodes);
	}
};

int tree_step::execute(const int &tag, tree_context &ctxt) const
{
	printf("node: %d\n", tag);
	if(tag < TREE_SIZE/2) {
		ctxt.m_tags.put(tag * 2);
		ctxt.m_tags.put(tag * 2 + 1);
	}

	return CnC::CNC_Success;
}

int main(int argc, char* argv[])
{
	// create context
	tree_context ctxt;

	//CnC::debug::trace(ctxt.m_steps);

	ctxt.m_tags.put(1);

	// wait for completion
	ctxt.wait();

	return 0;
}

