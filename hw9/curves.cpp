#include "common.h"

// Evaluate curve c at t and return the result.  If sub_a or sub_b are not null,
// perform subdivision on curve c at t as well.  If sub_a is not null, assign it
// the first piece (so that sub_a and c agree at their endpoint A.)  If sub_b is
// not null, assign it the second piece (so that sub_b and c agree at their
// endpoint D.)
vec2 Evaluate(const Curve& c, double t, Curve* sub_a, Curve* sub_b)
{
    vec2 AB = (1 - t) * c.A + t * c.B;
    vec2 BC = (1 - t) * c.B + t * c.C;
    vec2 CD = (1 - t) * c.C + t * c.D;

    vec2 ABC = (1 - t) * AB + t * BC;
    vec2 BCD = (1 - t) * BC + t * CD;

    vec2 P = (1 - t) * ABC + t * BCD;

    if (sub_a)
    {
        sub_a->A = c.A;
        sub_a->B = AB;
        sub_a->C = ABC;
        sub_a->D = P;
    }

    if (sub_b)
    {
        sub_b->A = P;
        sub_b->B = BCD;
        sub_b->C = CD;
        sub_b->D = c.D;
    }
    return P;
}

