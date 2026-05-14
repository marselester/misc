.intel_syntax noprefix
.text
.global _AddSubI32_a

# Calculates (a + b) - (c + d) + 7.
_AddSubI32_a:
    add edi, esi    # edi = edi + esi = a + b
    add edx, ecx    # edx = edx + ecx = c + d
    sub edi, edx    # edi = edi - edx
    add edi, 7      # edi = edi + 7
    mov eax, edi    # eax = edi
    ret             # return to caller
