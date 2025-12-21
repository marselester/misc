//go:build ignore

package main

import (
	asm "github.com/mmcloughlin/avo/build"
	"github.com/mmcloughlin/avo/operand"
)

//go:generate go run asm.go -out sum.s -stubs sum.go

func main() {
	asm.TEXT("SumVec", asm.NOSPLIT, "func(input []int64) int64")

	inputData := asm.GP64() // Base pointer of the slice is in AX.
	inputLen := asm.GP64()  // Number of elements in the slice is in CX.
	// MOVQ input_base+0(FP), AX
	asm.Load(asm.Param("input").Base(), inputData)
	// MOVQ input_len+8(FP), CX
	asm.Load(asm.Param("input").Len(), inputLen)

	vecLeft := asm.YMM() // 256-bit vector register Y0.
	// VMOVDQU (AX), Y0
	asm.VMOVDQU(operand.Mem{Base: inputData}, vecLeft)

	index := asm.GP64() // The array index is stored in register CX.
	// MOVQ $0x00000004, CX
	asm.MOVQ(operand.U32(4), index)

	vecRight := asm.YMM() // 256-bit vector register Y1.
	// VMOVDQU (AX)(CX*8), Y1
	asm.VMOVDQU(
		operand.Mem{
			Base:  inputData, // Array starts at 0xc000054760 address.
			Index: index,     // Array index is 4.
			Scale: 8,         // The multiplier of the index is 8 bytes (int64).
		},
		vecRight,
	)

	// VPADDQ Y0, Y1, Y0
	asm.VPADDQ(vecLeft, vecRight, vecLeft)

	vecRightLow := vecRight.AsX()
	// VEXTRACTI128 $0x01, Y0, X1
	asm.VEXTRACTI128(operand.U8(1), vecLeft, vecRightLow)

	vecLeftLow := vecLeft.AsX()
	// VPADDQ X0, X1, X0
	asm.VPADDQ(vecLeftLow, vecRightLow, vecLeftLow)

	// VPSRLDQ $0x08, X0, X1
	asm.VPSRLDQ(operand.U8(8), vecLeftLow, vecRightLow)
	// VPADDQ X0, X1, X0
	asm.VPADDQ(vecLeftLow, vecRightLow, vecLeftLow)

	sum := asm.GP64()
	// VMOVQ X0, AX
	asm.VMOVQ(vecLeftLow, sum)

	// MOVQ AX, ret+24(FP)
	asm.Store(sum, asm.ReturnIndex(0))
	// RET
	asm.RET()

	asm.Generate()
}
