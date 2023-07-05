/**
 * @file gcc_arithmetics64.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief 64bit arithmetics function for GCC
 *
 */
#include <bl/types.h>

/**
 * @union gcc64
 * @brief Union for cutting 64bit integer
 *
 * @typedef gcc64
 * @brief gcc64 type
 *
 */
typedef union gcc64 {
    /**
     * @brief Unsigned 64bit integer
     *
     */
    uint64_t u64;
    /**
     * @brief Signed 64bit integer
     *
     */
    int64_t s64;
    /**
     * @brief Cutted unsigned 64bit integer
     *
     */
    struct {
        uint32_t hi;
        uint32_t lo;
    } u32;
    /**
     * @brief Cutted signed 64bit integer
     *
     */
    struct {
        int32_t hi;
        int32_t lo;
    } s32;
} gcc64;

/**
 * @brief Divide unsigned 64bit divdend by unsigned 64bit divisor
 *
 * @param [in] dividend The dividend
 * @param [in] divisor The divisor
 * @param [out] quotient Pointer to the output quotient or NULL
 * @param [out] remainder Pointer to the output remainder or NULL
 */
void
unsigned_division64(qword_t dividend,
                    qword_t divisor,
                    qword_t* quotient,
                    qword_t* remainder) {
    /* Based on "Long division" algorithm
     * https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
     */

    int i;
    gcc64 N, D, Q, R;
    N.u64 = dividend;
    D.u64 = divisor;

    /* If divisor is zero cause divide exception */
    if (D.u64 == 0) {
        __asm__ volatile("div %[reg]" : : [reg] "r"(0));
    }
    Q.u64 = 0;
    R.u64 = 0;
    for (i = 64 - 1; i >= 0; --i) {
        R.u64 = R.u64 << 1;
        /* Set the least-significant bit of R equal to bit i of the numerator */
        R.u64 |= !!(N.u64 & (1ULL << i));
        if (R.u64 >= D.u64) {
            R.u64 = R.u64 - D.u64;
            Q.u64 |= 1ULL << i;
        }
    }

    if (quotient) {
        *quotient = Q.u64;
    }
    if (remainder) {
        *remainder = R.u64;
    }
}

/**
 * @brief GCC interface for \ref unsigned_division64 "unsigned_division64"
 *
 * @param [in] a The dividend
 * @param [in] b The divisor
 * @param [out] c Pointer to the output remainder or NULL
 * @return Quotient
 */
qword_t
__udivmoddi4(qword_t a, qword_t b, qword_t* c) {
    qword_t ret;
    unsigned_division64(a, b, &ret, c);
    return ret;
}
