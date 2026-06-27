// zig run ./zig/ch08_01.zig ./llvm/ch08_01.s
const std = @import("std");

const XmmVal = extern struct {
    data: extern union {
        m_I8: [16]i8,
        m_I16: [8]i16,
        m_I32: [4]i32,
        m_I64: [2]i64,
        m_U8: [16]u8,
        m_U16: [8]u16,
        m_U32: [4]u32,
        m_U64: [2]u64,
        m_F32: [4]f32,
        m_F64: [2]f64,
    } align(16),
};

extern fn AddI16_avx(c1: *XmmVal, c2: *XmmVal, a: *const XmmVal, b: *const XmmVal) void;

pub fn main() void {
    const a = XmmVal{
        .data = .{
            .m_I16 = [8]i16{ 10, 200, 30, -32766, 50, 60, 32000, -32000 },
        },
    };
    const b = XmmVal{
        .data = .{
            .m_I16 = [8]i16{ 100, -200, 32760, -400, 500, -600, 1200, -950 },
        },
    };
    var c1 = XmmVal{ .data = .{ .m_I16 = undefined } };
    var c2 = XmmVal{ .data = .{ .m_I16 = undefined } };

    AddI16_avx(&c1, &c2, &a, &b);
    std.debug.print("Zig says\nc1={any}\nc2={any}\n", .{ c1.data.m_I16, c2.data.m_I16 });
}
