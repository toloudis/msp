# CoreTests data

Files the tests read. Don't regenerate them: they record formats older
code wrote, which newer code must still read.

## OldFormatStrings*.bin

Binary chunk files with strings in the old format, from before the `ch`
readers and writers moved to UTF-8
(`docs/plans/2026-10-28-itstring-to-std-string.md`). Written on
2026-10-28 by `chBinWriter` (wide strings as null-terminated UTF-16LE),
through `WriteStrings` in `Ch/chStringTest.cpp`, which reads them back
(`chString.OldFormatFilesLoad`) and checks their bytes
(`chString.OldFormatFilesAreUTF16`).

- `OldFormatStringsHeader.bin`: the `gfFileBin` header (`TWFB`, mode 2,
  version 0.0.0, as documents write it), then the chunks.
- `OldFormatStringsNoHeader.bin`: the chunks only.

The chunks: a container `Strs` (version 1) holding one `Strg` chunk
(version 1) per string, in this order: `plain ASCII`,
`Crème brûlée à la façon`, `東京の夜景`, `🎬 clap 🎥` and the empty
string. Each `Strg` holds:

| Field | Written with | Bytes |
|---|---|---|
| index | `Write(int32_t)` | 4, little-endian |
| the string, wide | `Write(const envType::WChar*)` | UTF-16LE code units, then a 16-bit NUL |
| the string, narrow | `Write(const char*)` | UTF-8, then a NUL |
| sentinel `0x5A5A5A5A` | `Write(int32_t)` | 4 |
