package sumv3

import (
	"fmt"
	"math/rand"
	"testing"
)

func Sum(input []int64) int64 {
	var sum int64
	for _, v := range input {
		sum += v
	}

	return sum
}

func TestSumVec(t *testing.T) {
	if got := SumVec(nil); got != 0 {
		t.Fatalf("expected 0 got %d: nil", got)
	}

	if got := SumVec([]int64{}); got != 0 {
		t.Fatalf("expected 0 got %d: nil", got)
	}

	for n := range 100 {
		t.Run(fmt.Sprintf("sum of %d", n), func(t *testing.T) {
			input := make([]int64, n)
			for i := range n {
				input[i] = int64(i)
			}

			want := Sum(input)
			if got := SumVec(input); got != want {
				t.Fatalf("expected %d got %d: %v", want, got, input)
			}
		})
	}
}

const inputSize = 128 << 10

func BenchmarkSum(b *testing.B) {
	input := make([]int64, inputSize)
	for i := range input {
		input[i] = rand.Int63()
	}
	want := Sum(input)

	b.ResetTimer()
	for b.Loop() {
		if got := Sum(input); got != want {
			b.Fatalf("expected %d got %d: %v", want, got, input)
		}
	}
}

func BenchmarkSumVec(b *testing.B) {
	input := make([]int64, inputSize)
	for i := range input {
		input[i] = rand.Int63()
	}
	want := Sum(input)

	b.ResetTimer()
	for b.Loop() {
		if got := SumVec(input); got != want {
			b.Fatalf("expected %d got %d: %v", want, got, input)
		}
	}
}
