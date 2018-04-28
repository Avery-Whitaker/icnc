//
// Created by Avery Whitaker on 4/28/18.
//

#include <cnc/cnc.h>
#define CHUNK_SIZE 7

struct superblock_manager;

typedef std::pair<int, int> Tag_t;
typedef int ItemValue_t;

int M[5][5] = {{-8, -2, -2, -2, -2},
             {-4,  5,  2,  2,  2},
             {-4,  2,  5,  2,  2},
             {-4,  2,  2,  5,  2},
             {-4,  2,  2,  2,  5}};

int char_to_idx(char c) {
    switch (c) {
        case '_':
        case 0:
            return 0;
        case 'A':
        case 'a':
            return 1;
        case 'C':
        case 'c':
            return 2;
        case 'G':
        case 'g':
            return 3;
        case 'T':
        case 't':
            return 4;
    }
    CNC_ASSERT(false);
}

int get_score(char a, char b)
{
    return M[char_to_idx(a)][char_to_idx(b)];
}

struct SmithWatermanContext;

struct SmithWatermanStep {
    int execute(const Tag_t &t, SmithWatermanContext &ctx) const;
    void put(SmithWatermanContext &ctx, int i, int j) const ;
};

struct superblock_manager : CnC::tag_collection< int >::callback_type
{
    CnC::step_collection<SmithWatermanStep> m_sstep;
    CnC::tag_collection<int> m_stags;
    SmithWatermanContext &context;

    superblock_manager(SmithWatermanContext &context) : context(context), m_sstep(context, "sstep"), m_stags(context, "stags") {
        m_stags.prescribes(m_sstep, context);
        m_sstep.consumes(context.m_fibs);
        m_sstep.produces(context.m_fibs);
    }

    virtual bool on_put( const int & tag) override {
        if(tag >= CHUNK_SIZE && tag < context.n - context.n % CHUNK_SIZE) {
            if( tag % CHUNK_SIZE == 0) {
                // std::cout << "do chunk tag" << tag << std::endl;
                m_stags.put(tag);
            }
            //std::cout << "dont exec tag" << tag << std::endl;
            return true;
        }
        // std::cout << "do exec tag" << tag << std::endl;
        return false;
    }
};

struct SuperSmithWatermanStep
{
    int execute(const int tag, fib_context &ctx) const;
};


struct SmithWatermanContext : public CnC::context<SmithWatermanContext> {
    CnC::step_collection<SmithWatermanStep> steps;
    CnC::item_collection<Tag_t, ItemValue_t> items;
    CnC::tag_collection<Tag_t> tags;

    size_t n;
    size_t m;

    std::string a;
    std::string b;

    superblock_manager *m_super_manager;

    SmithWatermanContext(std::string a, std::string b)
            : CnC::context<SmithWatermanContext>(), steps(*this), items(*this), tags(*this), a(a), b(b) {

        m_super_manager = new superblock_manager(*this);
        tags.on_put(m_super_manager);

        tags.prescribes(steps, *this);
        steps.consumes(items);
        steps.produces(items);

        n = a.length();
        m = b.length();

    }
};


int SmithWatermanStep::execute(const Tag_t &t, SmithWatermanContext &ctx) const {
    int const &i = t.first;
    int const &j = t.second;

    int up, di, le;

    ctx.items.get(std::make_pair(i, j - 1), up);
    ctx.items.get(std::make_pair(i - 1, j), le);
    ctx.items.get(std::make_pair(i - 1, j - 1), di);

    int diagScore = di + get_score(ctx.a[i - 1], ctx.b[j - 1]);
    int topColScore = le + get_score(ctx.a[i - 1], 0);
    int leftRowScore = up + get_score(0, ctx.b[j - 1]);

    int res = std::max(diagScore, std::max(leftRowScore, topColScore));
    ctx.items.put(std::make_pair(i,j), res);

    //printf("%2d %2d %c %c %2d %3d %3d %3d\n", i, j, ctx.a[i - 1], ctx.b[j - 1], res, topColScore, diagScore, leftRowScore);

    if (i == 1) {
        put(ctx, 1, j + 1);
    }

    if (j == 1) {
        put(ctx, i + 1, 1);
    }

    put(ctx, i + 1, j + 1);

    return CnC::CNC_Success;
}

int SuperSmithWatermanStep::execute(const int tag, fib_context &ctx) const
{
    /* fib_type mem[CHUNK_SIZE + 3];
     ctx.m_fibs.get(tag - 2, mem[0]);
     ctx.m_fibs.get(tag - 1, mem[1]);

     for(int i = 2; i < CHUNK_SIZE + 2; i++)
         mem[i] = mem[i-1] + mem[i-2];

     ctx.m_fibs.put(tag + CHUNK_SIZE - 2, mem[CHUNK_SIZE]);
     ctx.m_fibs.put(tag + CHUNK_SIZE - 1, mem[CHUNK_SIZE + 1]);*/

    // std::cout << "superblock: " << tag << std::endl;
    return CnC::CNC_Success;
}

void SmithWatermanStep::put(SmithWatermanContext &ctx, int i, int j) const {
    if (i <= ctx.n && j <= ctx.m) {
        ctx.tags.put(std::make_pair(i, j));
    }
}



int main() {
    std::string a = "TACGACCTGTTACACCTGGTACCTTACGGT";
    std::string b = "TAGATATAGGAGGGATATTTAGAGAGGAGAAGGATAGAGGGATTT";

    SmithWatermanContext ctx(a, b);

    ctx.items.put(std::make_pair(0, 0), 0);

    for (int i = 1; i <= a.length(); i++) {
        ctx.items.put(std::make_pair(i, 0), -4 * i);
    }

    for (int i = 1; i <= b.length(); i++) {
        ctx.items.put(std::make_pair(0, i), -2 * i);
    }

    ctx.tags.put(std::make_pair(1, 1));
    ctx.wait();

    int sol;

    ctx.items.get(std::make_pair(a.length(),b.length()), sol);

    //should be 81
    printf("sol is %d\n", sol);

    return 0;
}