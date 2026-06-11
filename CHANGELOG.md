# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

## [0.1.9] - 2026-06-11

### Security

- Fixed a heap buffer overflow (GHSA-22pc-xxw4-q849, CWE-787) in `bincookie_init_file()`. The
  function previously trusted the file-controlled `num_pages`, page sizes, and cookie offsets
  without bounds-checking them against the actual file length, so a crafted `.binarycookies` file
  could trigger out-of-bounds reads and writes on the heap. The header size is now validated,
  `num_pages` is bounded before the byte-swap loop, and a new structural validation pass
  (`bincookie_validate_pages`) checks every page header, cookie offset table, cookie, and cookie
  string offset. Malformed files are now rejected (returning `NULL` and setting `errno` to `EIO`)
  instead of corrupting the heap. Reported by @d00xy-hash.

## [0.1.8] - 2026-05-02

### Added

- Kaitai Struct definition (`binarycookies.ksy`) describing the Apple binary cookies format, so the
  format can be parsed in any language supported by Kaitai Struct.

## [0.1.2]

### Changed

- Miscellaneous changes to documentation.

## [0.1.1]

### Fixed

- Fixed argument order in `fread()`. This did not cause any bugs due to the arguments used but it was
  still incorrect usage.

[unreleased]: https://github.com/Tatsh/bincookie/compare/v0.1.9...HEAD
[0.1.9]: https://github.com/Tatsh/bincookie/compare/v0.1.8...v0.1.9
[0.1.8]: https://github.com/Tatsh/bincookie/compare/v0.1.7...v0.1.8
[0.1.2]: https://github.com/Tatsh/bincookie/compare/v0.1.1...v0.1.2
[0.1.1]: https://github.com/Tatsh/bincookie/compare/v0.1.0...v0.1.1
