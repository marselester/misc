const std = @import("std");

const lib = @import("mylib.zig");
const header = @cImport(@cInclude("mylib.h"));

const allocator = std.heap.c_allocator;
var threaded: std.Io.Threaded = init: {
    var t: std.Io.Threaded = .init_single_threaded;
    t.allocator = allocator;
    break :init t;
};

const Error = lib.Database.OpenError ||
    lib.Database.LookupError ||
    error{
        InvalidArgument,
    };

fn errToStatus(err: Error) header.db_status_t {
    return switch (err) {
        error.OutOfMemory => header.DB_ERR_OUT_OF_MEMORY,
        error.EmptyFile,
        error.StreamTooLong,
        error.UnexpectedEndOfData,
        => header.DB_ERR_INVALID_DB,
        error.InvalidArgument => header.DB_ERR_INVALID_ARGUMENT,
        else => header.DB_ERR_IO,
    };
}

export fn db_open(
    path: [*c]const u8,
    path_len: usize,
    out: ?*?*lib.Database,
) header.db_status_t {
    const o = out orelse {
        return header.DB_ERR_INVALID_ARGUMENT;
    };
    o.* = null;

    if (path == null or path_len == 0) {
        return header.DB_ERR_INVALID_ARGUMENT;
    }

    const db = allocator.create(lib.Database) catch {
        return header.DB_ERR_OUT_OF_MEMORY;
    };
    db.* = lib.Database.open(allocator, threaded.io(), path[0..path_len]) catch |err| {
        allocator.destroy(db);
        return errToStatus(err);
    };

    o.* = db;

    return header.DB_OK;
}

export fn db_close(db: ?*lib.Database) void {
    if (db) |d| {
        d.close();
        allocator.destroy(d);
    }
}

export fn db_lookup_string(
    db: ?*const lib.Database,
    out: ?*[*]const u8,
    out_len: ?*usize,
) header.db_status_t {
    const d = db orelse {
        return header.DB_ERR_INVALID_ARGUMENT;
    };
    const o = out orelse {
        return header.DB_ERR_INVALID_ARGUMENT;
    };
    const o_len = out_len orelse {
        return header.DB_ERR_INVALID_ARGUMENT;
    };

    const s = d.lookup([]const u8) catch |err| {
        return errToStatus(err);
    };

    o.* = s.ptr;
    o_len.* = s.len;

    return header.DB_OK;
}

export fn db_lookup_u8(db: ?*const lib.Database, out: ?*u8) header.db_status_t {
    return lookupInteger(u8, db, out);
}

export fn db_lookup_u16(db: ?*const lib.Database, out: ?*u16) header.db_status_t {
    return lookupInteger(u16, db, out);
}

export fn db_lookup_u32(db: ?*const lib.Database, out: ?*u32) header.db_status_t {
    return lookupInteger(u32, db, out);
}

export fn db_lookup_u64(db: ?*const lib.Database, out: ?*u64) header.db_status_t {
    return lookupInteger(u64, db, out);
}

export fn db_lookup_i8(db: ?*const lib.Database, out: ?*i8) header.db_status_t {
    return lookupInteger(i8, db, out);
}

export fn db_lookup_i16(db: ?*const lib.Database, out: ?*i16) header.db_status_t {
    return lookupInteger(i16, db, out);
}

export fn db_lookup_i32(db: ?*const lib.Database, out: ?*i32) header.db_status_t {
    return lookupInteger(i32, db, out);
}

export fn db_lookup_i64(db: ?*const lib.Database, out: ?*i64) header.db_status_t {
    return lookupInteger(i64, db, out);
}

fn lookupInteger(
    comptime T: type,
    db: ?*const lib.Database,
    out: ?*T,
) header.db_status_t {
    const d = db orelse {
        return header.DB_ERR_INVALID_ARGUMENT;
    };
    const o = out orelse {
        return header.DB_ERR_INVALID_ARGUMENT;
    };

    o.* = d.lookup(T) catch |err| {
        return errToStatus(err);
    };

    return header.DB_OK;
}
