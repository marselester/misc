const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "main",
        .root_module = b.createModule(.{
            .root_source_file = b.path("zig/main.zig"),
            .target = target,
            .optimize = optimize,
        }),
    });

    exe.root_module.addAssemblyFile(b.path("llvm/ch02_01_intel.s"));

    b.installArtifact(exe);

    const lib = b.addLibrary(.{
        .linkage = .static,
        .name = "ch02_01",
        .root_module = b.createModule(.{
            .root_source_file = b.path("zig/ch02_01.zig"),
            .target = target,
            .optimize = optimize,
        }),
    });

    b.installArtifact(lib);
}
