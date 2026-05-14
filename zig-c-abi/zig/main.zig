const std = @import("std");

// Calculates (a + b) - (c + d) + 7.
extern fn AddSubI32_a(a: i32, b: i32, c: i32, d: i32) callconv(.c) i32;

pub fn main() void {
    const r = AddSubI32_a(1, 2, 3, 4);
    std.debug.print("Zig says {d}\n", .{r});
}
