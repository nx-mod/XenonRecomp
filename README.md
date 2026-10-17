# xenonrecomp-nx

A fork of [XenonRecomp](https://github.com/hedge-dev/XenonRecomp) that recompiles Xbox 360
executables to C++ for AArch64, built with GCC, mainly for Nintendo Switch homebrew. It is the
recompiler behind [halo3-nx](https://github.com/nx-mod/halo3-nx).

XenonRecomp only converts game code. A runtime has to provide the kernel, memory, GPU and
everything else, and making the game work is that runtime's job.

## What's different from upstream

- **GCC and devkitA64:** the output and the headers build with GCC for the Switch. That means
  shims for the Clang builtins, `cntpct_el0` for the time base, no anonymous aggregate in
  `XXOVERLAPPED`, and `MemoryMappedFile` reading into memory.
- **Jump tables switch on the register's low word.** The guard before the table (`cmplwi`) and
  the table index (`rlwinm`) only look at those 32 bits. Switching on the whole register reaches
  `__builtin_unreachable` for negative values, and GCC compiles that into a hang.
- **`symbol_prefix`:** a config option that prefixes an image's entry point (`_xstart`) and its
  register save/restore helpers. That lets a game and the DLLs it loads link into one program.
- **Imports from other DLLs** are named `__imp__<library>_<ordinal>`. A runtime can then resolve
  them through the loaded module's exports. Upstream recompiles them as empty stubs.
- **More instructions:** `lvxl128`, `stvlxl128`, `vandc`, `vsel128`, `vctuxs`, `vpkuwus`,
  `vpkswss`, `lhbrx`, `frsqrte`, `eqv`, `mulhdu`, `dcbst` and `blrl`. Also `vupkd3d128` formats
  2-5 and `vpkd3d128` formats 2-3, ported from the ReXGlue SDK (BSD-3).
- `ppc_context.h` keeps a `PPC_FUNC_PROLOGUE` defined before it, for call tracing.
- `Image::dataSize` reports the bytes actually loaded.

## Usage

The same as upstream: `XenonAnalyse` finds jump tables, and `XenonRecomp <config.toml>
<ppc_context.h>` writes the C++. The config options are described in
[upstream's README](https://github.com/hedge-dev/XenonRecomp#usage). The one new option:

```toml
[main]
symbol_prefix = "MyDll_"   # MyDll__xstart, MyDll___savegprlr_14, ...
```

## Building

The tools build with CMake 3.20+ and Clang 18+ on the host. Clone with `--recursive`. The
recompiled output builds with Clang, or with GCC through devkitA64.

## License

MIT, as upstream. Built on XenonRecomp by hedge-dev and contributors, which credits
[Xenia](https://github.com/xenia-project/xenia) for its PPC translation and
[Xenia Canary](https://github.com/xenia-canary/xenia-canary) for XEX patching.
