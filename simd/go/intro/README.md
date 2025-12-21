# Intro to SIMD in avo

Here you can find examples used in
[Intro to SIMD in avo](https://marselester.com/intro-to-simd-in-avo.html) post.

SIMD sum on 131,072 array is ~54% faster than a scalar version.

```sh
$ go generate ./sumv3/asm.go
$ go test -bench=^BenchmarkSum -benchmem -count=10 ./sumv3
goos: darwin
goarch: amd64
pkg: intro/sumv3
cpu: Intel(R) Core(TM) i5-10600 CPU @ 3.30GHz
BenchmarkSum-12       	   30868	     38895 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30957	     39033 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30648	     38902 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30870	     38829 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30607	     38896 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30825	     38911 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   29904	     39349 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30776	     38808 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30836	     38872 ns/op	       0 B/op	       0 allocs/op
BenchmarkSum-12       	   30800	     38901 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   64930	     17594 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   63696	     18119 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   63920	     17990 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   70584	     17532 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   65150	     18114 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   67203	     17858 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   70060	     17358 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   66661	     17807 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   66045	     18127 ns/op	       0 B/op	       0 allocs/op
BenchmarkSumVec-12    	   68854	     17422 ns/op	       0 B/op	       0 allocs/op
PASS
ok  	intro/sumv3	24.212s

$ benchstat old.txt new.txt
name    old time/op    new time/op    delta
Sum-12    38.9µs ± 0%    17.8µs ± 2%  -54.23%  (p=0.000 n=8+10)
```
