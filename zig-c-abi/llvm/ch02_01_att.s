.text
.global _AddSubI32_a

# Calculates (a + b) - (c + d) + 7.
_AddSubI32_a:
    addl %esi, %edi    # edi = edi + esi
    addl %ecx, %edx    # edx = edx + ecx
    subl %edx, %edi    # edi = edi - edx
    addl $7, %edi      # edi = edi + 7
    movl %edi, %eax    # eax = edi
    ret                # return to caller
