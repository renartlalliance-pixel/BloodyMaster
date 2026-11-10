// Test M1 : utilitaires dB / sanitize. Critere : toutes les assertions passent (code retour 0).
#include "DbUtils.h"
#include <cstdio>
#include <limits>

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("ECHEC ligne %d : %s\n", __LINE__, #c); ++failures; } } while (0)
static bool approx(float a, float b, float tol) { return std::fabs(a - b) <= tol; }

int main()
{
    using namespace bm::util;
    const float inf = std::numeric_limits<float>::infinity();
    const float nan = std::numeric_limits<float>::quiet_NaN();

    CHECK(approx(dbToGain(0.0f), 1.0f, 1e-6f));
    CHECK(approx(dbToGain(-6.0206f), 0.5f, 1e-4f));
    CHECK(approx(dbToGain(20.0f), 10.0f, 1e-4f));
    CHECK(dbToGain(nan) == 1.0f);
    CHECK(dbToGain(inf) == 1.0f);

    CHECK(approx(gainToDb(1.0f), 0.0f, 1e-5f));
    CHECK(approx(gainToDb(0.5f), -6.0206f, 1e-3f));
    CHECK(gainToDb(0.0f) == kSilenceDb);
    CHECK(gainToDb(-1.0f) == kSilenceDb);
    CHECK(gainToDb(nan) == kSilenceDb);
    CHECK(gainToDb(1e-12f) == kSilenceDb);

    // Aller-retour sur une plage utile
    for (float db = -60.0f; db <= 12.0f; db += 0.5f)
        CHECK(approx(gainToDb(dbToGain(db)), db, 1e-3f));

    CHECK(sanitize(nan) == 0.0f);
    CHECK(sanitize(inf) == 0.0f);
    CHECK(sanitize(-inf) == 0.0f);
    CHECK(sanitize(0.25f) == 0.25f);

    if (failures) std::printf("%d echec(s)\n", failures); else std::printf("OK\n");
    return failures ? 1 : 0;
}
