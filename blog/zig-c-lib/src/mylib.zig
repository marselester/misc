const std = @import("std");

pub const Database = struct {
    src: []const u8,
    allocator: std.mem.Allocator,

    pub const OpenError = InvalidDatabaseError ||
        FileSystemError ||
        std.mem.Allocator.Error;
    pub const InvalidDatabaseError = error{
        EmptyFile,
    };
    pub const FileSystemError = std.Io.File.OpenError ||
        std.Io.File.LengthError ||
        std.Io.Dir.ReadFileAllocError;

    pub const LookupError = error{
        UnexpectedEndOfData,
    };

    pub fn open(
        allocator: std.mem.Allocator,
        io: std.Io,
        path: []const u8,
    ) OpenError!Database {
        const src = try std.Io.Dir.cwd().readFileAlloc(
            io,
            path,
            allocator,
            .limited(1024),
        );

        if (src.len == 0) {
            return InvalidDatabaseError.EmptyFile;
        }

        return .{
            .src = src,
            .allocator = allocator,
        };
    }

    pub fn close(self: *const Database) void {
        self.allocator.free(self.src);
    }

    pub fn lookup(self: *const Database, T: type) LookupError!T {
        return switch (T) {
            []const u8 => self.src,
            u8, u16, u32, u64, i8, i16, i32, i64 => {
                if (self.src.len < @sizeOf(T)) {
                    return LookupError.UnexpectedEndOfData;
                }

                const bytes = self.src[0..@sizeOf(T)];
                return std.mem.readInt(T, bytes, .big);
            },
            else => @compileError("unsupported type " ++ @typeName(T)),
        };
    }
};

test "lookup string" {
    const db = try Database.open(
        std.testing.allocator,
        std.testing.io,
        "testdata/string",
    );
    defer db.close();

    const gotString = try db.lookup([]const u8);
    try std.testing.expectEqualStrings("test\n", gotString);
}

test "lookup integer" {
    const db = try Database.open(
        std.testing.allocator,
        std.testing.io,
        "testdata/integer",
    );
    defer db.close();

    const tests = .{
        .{ .T = u8, .want = 128 },
        .{ .T = i8, .want = -128 },
        .{ .T = u16, .want = 32768 },
        .{ .T = i16, .want = -32768 },
        .{ .T = u32, .want = 2147483648 },
        .{ .T = i32, .want = -2147483648 },
        .{ .T = u64, .want = 9223372036854775808 },
        .{ .T = i64, .want = -9223372036854775808 },
    };

    inline for (tests) |tc| {
        const gotInteger = try db.lookup(tc.T);
        try std.testing.expectEqual(tc.want, gotInteger);
    }
}

test "lookup error" {
    const db = try Database.open(
        std.testing.allocator,
        std.testing.io,
        "testdata/string",
    );
    defer db.close();

    try std.testing.expectError(
        Database.LookupError.UnexpectedEndOfData,
        db.lookup(u64),
    );
}
