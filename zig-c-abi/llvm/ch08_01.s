.intel_syntax noprefix
.text
.global _AddI16_avx

# Calculates *c1 = *a + *b (wraparound), *c2 = *a + *b (saturated).
# AddI16_avx(c1: *XmmVal, c2: *XmmVal, a: *const XmmVal, b: *const XmmVal) void;
_AddI16_avx:
    vmovdqa xmm0, [rdx]         # xmm0 = *a
    vmovdqa xmm1, [rcx]         # xmm1 = *b

    vpaddw xmm2, xmm0, xmm1     # xmm2 = xmm0 + xmm1 (wraparound)
    vpaddsw xmm3, xmm0, xmm1    # xmm3 = xmm0 + xmm1 (saturated)

    vmovdqa [rdi], xmm2         # *c1 = xmm2
    vmovdqa [rsi], xmm3         # *c2 = xmm3
    ret
