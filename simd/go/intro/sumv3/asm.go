//go:build ignore

package main

import (
	asm "github.com/mmcloughlin/avo/build"
	"github.com/mmcloughlin/avo/operand"
)

//go:generate go run asm.go -out sum.s -stubs sum.go

func main() {
	asm.TEXT("SumVec", asm.NOSPLIT, "func(input []int64) int64")

	inputData := asm.GP64()
	inputLen := asm.GP64()
	asm.Load(asm.Param("input").Base(), inputData)
	asm.Load(asm.Param("input").Len(), inputLen)

	sum := asm.GP64()
	index := asm.GP64()
	asm.XORQ(sum, sum)
	asm.XORQ(index, index)

	asm.Comment("If we can't use two YMM vectors (inputLen < 8), fallback to a scalar sum.")
	asm.CMPQ(inputLen, operand.U8(8))
	asm.JL(operand.LabelRef("scalar_loop"))

	asm.Comment("Otherwise keep adding YMM vectors in the loop.")
	vecLeft := asm.YMM()
	vecRight := asm.YMM()
	asm.VMOVDQU(operand.Mem{Base: inputData}, vecLeft)

	// We can efficiently calculate (in 1 cycle) the vector loop end
	// since 4 is a power of 2 like this: inputLen - (inputLen % 4).
	asm.Comment("loopEnd = inputLen - (inputLen % 4)")
	loopEnd := asm.GP64()
	asm.MOVQ(inputLen, loopEnd)
	asm.ANDQ(operand.I8(-4), loopEnd)

	asm.Label("vector_loop")
	asm.Comment("for i += 4; i <= loopEnd; i += 4")
	{
		asm.ADDQ(operand.U32(4), index)
		asm.CMPQ(loopEnd, index)
		asm.JLE(operand.LabelRef("vector_loop_end")) // Exit the vector loop.

		asm.VMOVDQU(
			operand.Mem{
				Base:  inputData,
				Index: index,
				Scale: 8,
			},
			vecRight,
		)
		asm.VPADDQ(vecLeft, vecRight, vecLeft)

		asm.JMP(operand.LabelRef("vector_loop"))
	}
	asm.Label("vector_loop_end")

	asm.Comment("Horizontal reduction.")
	{
		vecRightLow := vecRight.AsX()
		asm.VEXTRACTI128(operand.U8(1), vecLeft, vecRightLow)

		vecLeftLow := vecLeft.AsX()
		asm.VPADDQ(vecLeftLow, vecRightLow, vecLeftLow)

		asm.VPSRLDQ(operand.U8(8), vecLeftLow, vecRightLow)
		asm.VPADDQ(vecLeftLow, vecRightLow, vecLeftLow)

		asm.VMOVQ(vecLeftLow, sum)

		asm.VZEROUPPER()
	}

	asm.Comment("Set i = loopEnd to sum the tail of the array.")
	asm.MOVQ(loopEnd, index)

	asm.Label("scalar_loop")
	asm.Comment("Summarize what we couldn't with SIMD.")
	asm.Comment("for i; i < inputLen; i++")
	{
		asm.CMPQ(inputLen, index)
		asm.JLE(operand.LabelRef("scalar_loop_end"))

		asm.ADDQ(
			operand.Mem{
				Base:  inputData,
				Index: index,
				Scale: 8,
			},
			sum,
		)
		asm.INCQ(index)
		asm.JMP(operand.LabelRef("scalar_loop"))
	}
	asm.Label("scalar_loop_end")

	asm.Store(sum, asm.ReturnIndex(0))
	asm.RET()

	asm.Generate()
}
