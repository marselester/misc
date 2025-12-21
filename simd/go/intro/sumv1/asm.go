//go:build ignore

package main

import asm "github.com/mmcloughlin/avo/build"

//go:generate go run asm.go -out sum.s -stubs sum.go

func main() {
	asm.TEXT("SumVec", asm.NOSPLIT, "func(input []int64) int64")
	sum := asm.GP64()
	asm.XORQ(sum, sum)
	asm.Store(sum, asm.ReturnIndex(0))
	asm.RET()

	asm.Generate()
}
