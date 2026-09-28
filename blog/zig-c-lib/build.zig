const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const mod = b.addModule("mylib", .{
        .root_source_file = b.path("src/mylib.zig"),
        .target = target,
    });

    const c_abi = b.createModule(.{
        .root_source_file = b.path("src/c_abi.zig"),
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    c_abi.addIncludePath(b.path("include"));

    const mylib = b.addLibrary(.{
        .name = "mylib",
        .root_module = c_abi,
    });
    mylib.installHeader(b.path("include/mylib.h"), "mylib.h");

    b.installArtifact(mylib);

    const mod_tests = b.addTest(.{
        .root_module = mod,
    });

    const run_mod_tests = b.addRunArtifact(mod_tests);

    const test_step = b.step("test", "Run tests");
    test_step.dependOn(&run_mod_tests.step);
}
