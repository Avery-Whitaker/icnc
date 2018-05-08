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

struct SmithWatermanContext : public CnC::context<SmithWatermanContext> {
    CnC::step_collection<SmithWatermanStep> steps;
    CnC::item_collection<Tag_t, ItemValue_t> items;
    CnC::tag_collection<Tag_t> tags;

    size_t n, m;

    std::string a, b;

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

    int up, di, le;

    ctx.items.get(std::make_pair(i, j - 1), up);
    ctx.items.get(std::make_pair(i - 1, j), le);
    ctx.items.get(std::make_pair(i - 1, j - 1), di);

    int diagScore = di + get_score(ctx.a[i - 1], ctx.b[j - 1]);
    int topColScore = le + get_score(ctx.a[i - 1], 0);
    int leftRowScore = up + get_score(0, ctx.b[j - 1]);

    int res = std::max(diagScore, std::max(leftRowScore, topColScore));
    ctx.items.put(std::make_pair(i,j), res);

    return CnC::CNC_Success;
}


int main() {
    std::string a = "TAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACA";
    std::string b = "TAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAGTAGAAGGAGAGTAGAGATAGACGACCCTAGCGATCAGATACGATACACACGATTACGGTACACAGGTACATAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATCTAATCTAATAGACAGATACGGTACAACTCCTAGCCATCAATATC";

    SmithWatermanContext ctx(a, b);

    ctx.items.put(std::make_pair(0, 0), 0);

    for (int i = 1; i <= a.length(); i++) {
        ctx.items.put(std::make_pair(i, 0), -4 * i);
    }

    for (int i = 1; i <= b.length(); i++) {
        ctx.items.put(std::make_pair(0, i), -2 * i);
    }

    for (int i = 1; i <= a.length(); i++) {
        for (int j = 1; j <= b.length(); j++) {
            ctx.tags.put(std::make_pair(i, j));
        }
    }

    ctx.wait();

    int sol;

    ctx.items.get(std::make_pair(a.length(),b.length()), sol);

    printf("sol is %d\n", sol);

    return 0;
}