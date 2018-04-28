//
// Created by Avery Whitaker on 4/28/18.
//

#include <cnc/cnc.h>
#define CHUNK_SIZE 2

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
};


struct SuperSmithWatermanStep
{
    int execute(const Tag_t tag, SmithWatermanContext &ctx) const;
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

    SmithWatermanContext(std::string a, std::string b);
};


struct superblock_manager : CnC::tag_collection< Tag_t >::callback_type
{
    CnC::step_collection<SuperSmithWatermanStep> m_sstep;
    CnC::tag_collection<Tag_t> m_stags;
    SmithWatermanContext &context;

    superblock_manager(SmithWatermanContext &context) : context(context), m_sstep(context), m_stags(context) {
        m_stags.prescribes(m_sstep, context);
        m_sstep.consumes(context.items);
        m_sstep.produces(context.items);
    }

    virtual bool on_put( const Tag_t & tag) override {
        if(tag.first >= CHUNK_SIZE && tag.first < context.n - context.n % CHUNK_SIZE
                && tag.second >= CHUNK_SIZE && tag.second < context.m - context.m % CHUNK_SIZE) {

            if( tag.first % CHUNK_SIZE == 0 &&
                    tag.second % CHUNK_SIZE == 0) {
                 std::cout << "do chunk tag" << "(" << tag.first << ", " << tag.second << ")" << std::endl;
                m_stags.put(tag);
            }
             std::cout << "dont exec tag" << "(" << tag.first << ", " << tag.second << ")" << std::endl;
            return true;
        }
         std::cout << "do exec tag "  << "(" << tag.first << ", " << tag.second << ")" <<  std::endl;
        return false;
    }
};

SmithWatermanContext::SmithWatermanContext(std::string a, std::string b)
: CnC::context<SmithWatermanContext>(), steps(*this), items(*this), tags(*this), a(a), b(b) {

    m_super_manager = new superblock_manager(*this);
    tags.on_put(m_super_manager);

    tags.prescribes(steps, *this);
    steps.consumes(items);
    steps.produces(items);

    n = a.length();
    m = b.length();

}

void put(SmithWatermanContext &ctx, int i, int j) {
    if (i <= ctx.n && j <= ctx.m) {
        ctx.tags.put(std::make_pair(i, j));
    }
}

int do_step(int i, int j, SmithWatermanContext &ctx) {

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

}

int SmithWatermanStep::execute(const Tag_t &t, SmithWatermanContext &ctx) const {
    int const &i = t.first;
    int const &j = t.second;

    do_step(i,j, ctx);

    return CnC::CNC_Success;
}

int SuperSmithWatermanStep::execute(const Tag_t tag, SmithWatermanContext &ctx) const
{

    /* Simple version

    for(int i = tag.first; i < tag.first + CHUNK_SIZE; i++) {
        for(int j = tag.second; j < tag.second + CHUNK_SIZE; j++) {
            do_step(i,j, ctx);
        }
    }
     */

    //Version without puts
    int mem[CHUNK_SIZE + 1][CHUNK_SIZE + 1];

    int inI = tag.first - 1;
    int inJ = tag.second - 1;
    int outI = tag.first + CHUNK_SIZE;
    int outJ = tag.second + CHUNK_SIZE;

     ctx.items.get(std::make_pair(inI, inJ), mem[0][0]);

    for(int i = 1; i <= CHUNK_SIZE; i++) {
        ctx.items.get(std::make_pair(inI + i, inJ), mem[i][0]);
    }

    for(int j = 1; j <= CHUNK_SIZE; j++) {
        ctx.items.get(std::make_pair(inI, inJ + j), mem[0][j]);
    }

    for(int i = 1; i <= CHUNK_SIZE; i++) {
        for (int j = 1; j <= CHUNK_SIZE; j++) {

            int up = mem[i][j - 1];
            int di = mem[i - 1][j];
            int le = mem[i-1][j-1];

            int diagScore = di + get_score(ctx.a[inI + i - 1], ctx.b[inJ + j - 1]);
            int topColScore = le + get_score(ctx.a[inI + i - 1], 0);
            int leftRowScore = up + get_score(0, ctx.b[inJ + j - 1]);

            mem[i][j] = std::max(diagScore, std::max(leftRowScore, topColScore));
        }
    }

    for(int i = 1; i < CHUNK_SIZE; i++) {
        ctx.items.put(std::make_pair(inI + i, outJ), mem[i][CHUNK_SIZE]);
        ctx.tags.put(std::make_pair(inI + i, outJ));
    }

    for(int j = 1; j < CHUNK_SIZE; j++) {
        ctx.items.put(std::make_pair(outI, inJ + j), mem[CHUNK_SIZE][j]);
        ctx.tags.put(std::make_pair(outI, inJ + j));
    }

    ctx.tags.put(std::make_pair(outI, outJ));
    ctx.items.put(std::make_pair(outI, outJ), mem[CHUNK_SIZE][CHUNK_SIZE]);

    return CnC::CNC_Success;
}



int main() {
    std::string a = "TAGTAG";
    std::string b = "TAATCTAATC";

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