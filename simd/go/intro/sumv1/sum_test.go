package sumv1

import "testing"

func TestSumVec(t *testing.T) {
	input := []int64{1, 2, 3, 4, 5, 6, 7, 8}

	var want int64 = 0
	if got := SumVec(input); got != want {
		t.Fatalf("expected %d got %d", want, got)
	}
}
