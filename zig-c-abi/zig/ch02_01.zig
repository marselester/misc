export fn AddSubI32_a(a: i32, b: i32, c: i32, d: i32) callconv(.c) i32 {
    return (a + b) - (c + d) + 7;
}
