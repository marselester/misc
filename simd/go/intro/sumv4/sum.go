package sumv4

import (
	"simd/archsimd"
)

//go:noinline
func SumVec(input []int64) (sum int64) {
	i := 0
	inputLen := len(input)

	// If we can't use two YMM vectors, fallback to a scalar sum.
	// Otherwise keep adding YMM vectors in the vector loop.
	if inputLen >= 8 {
		y0 := archsimd.LoadInt64x4Slice(input)
		loopEnd := inputLen - inputLen%4
		for i += 4; i < loopEnd; i += 4 {
			y1 := archsimd.LoadInt64x4Slice(input[i : i+4])
			y0 = y0.Add(y1)
		}

		// Horizontal reduction.
		x0, x1 := y0.GetLo(), y0.GetHi()
		x0 = x0.Add(x1)
		sum = x0.GetElem(0) + x0.GetElem(1)
	}

	// Summarize what we couldn't with SIMD.
	for ; i < inputLen; i++ {
		sum += input[i]
	}

	return sum
}
