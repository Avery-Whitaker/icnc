//
// Created by Srdan Milakovic on 4/27/18.
//


#include <cnc/cnc.h>

typedef std::pair<int, int> Tag_t;
typedef int ItemValue_t;

int M[5][5] = {{-8, -2, -2, -2, -2},
             {-4,  5,  2,  2,  2},
             {-4,  2,  5,  2,  2},
             {-4,  2,  2,  5,  2},
             {-4,  2,  2,  2,  5}};

int get_score(char a, char b)
{
    int A = 0;
    int B = 0;

    switch (a) {
        case '_':
        case 0:
            A = 0;
            break;
        case 'A':
        case 'a':
            A = 1;
            break;
        case 'C':
        case 'c':
            A = 2;
            break;
        case 'G':
        case 'g':
            A = 3;
            break;
        case 'T':
        case 't':
            A = 4;
            break;
        default:
            CNC_ASSERT(false);
            break;
    }

    switch (b) {
        case '_':
        case 0:
            B = 0;
            break;
        case 'A':
        case 'a':
            B = 1;
            break;
        case 'C':
        case 'c':
            B = 2;
            break;
        case 'G':
        case 'g':
            B = 3;
            break;
        case 'T':
        case 't':
            B = 4;
            break;
        default:
            CNC_ASSERT(false);
            break;
    }

    return M[A][B];
}

struct SmithWatermanContext;

struct SmithWatermanStep {
    int execute(const Tag_t &t, SmithWatermanContext &ctx) const;
    void put(SmithWatermanContext &ctx, int i, int j) const ;
};

struct SmithWatermanContext : public CnC::context<SmithWatermanContext> {
    CnC::step_collection<SmithWatermanStep> steps;
    CnC::item_collection<Tag_t, ItemValue_t> items;
    CnC::tag_collection<Tag_t> tags;

    size_t n;
    size_t m;

    std::string a;
    std::string b;

    SmithWatermanContext(std::string a, std::string b)
            : CnC::context<SmithWatermanContext>(), steps(*this), items(*this), tags(*this), a(a), b(b) {
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

    int up = 0;
    int di = 0;
    int le = 0;

    ctx.items.get(std::make_pair(i - 1, j), up);
    ctx.items.get(std::make_pair(i, j - 1), le);
    ctx.items.get(std::make_pair(i - 1, j - 1), di);

    int diagScore = di + get_score(ctx.a[i], ctx.b[j]);
    int topColScore = le + get_score(ctx.a[i], 0);
    int leftRowScore = up + get_score(0, ctx.b[j]);

    int res = std::max(diagScore, std::max(leftRowScore, topColScore));
    ctx.items.put(std::make_pair(i,j), res);

    //printf("%2d\t%2d\t%2d\t%d+%d\t%d+%d\t%d+%d\n", i, j, res, up, get_score(0, ctx.b[j]), di, get_score(ctx.a[i], ctx.b[j]), le, get_score(ctx.a[i], 0));
    printf("%2d %2d %2d %3d %3d %3d %3d %3d %3d\n", i, j, res, up, di, le, get_score(0, ctx.b[j]), get_score(ctx.a[i], ctx.b[j]), get_score(ctx.a[i], 0));

    if (i == 1) {
        put(ctx, 1, j + 1);
    }

    if (j == 1) {
        put(ctx, i + 1, 1);
    }

    put(ctx, i + 1, j + 1);

    return CnC::CNC_Success;
}

void SmithWatermanStep::put(SmithWatermanContext &ctx, int i, int j) const {
    if (i <= ctx.n && j <= ctx.m) {
        ctx.tags.put(std::make_pair(i, j));
    }
}



int main() {
    std::string a = "TACGGT";
    std::string b = "ACCT";

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

    return 0;
}