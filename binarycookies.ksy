# Apple Binary Cookies format (Safari / WebKit Cookies.binarycookies)
# Based on bincookie.h and parsing logic in this directory.
# File is big-endian; magic is "cook".

meta:
  id: binarycookies
  endian: be
  title: Apple Binary Cookies
  file-extension: binarycookies
  license: MIT

seq:
  - id: magic
    contents: [0x63, 0x6f, 0x6f, 0x6b] # "cook"
  - id: num_pages
    type: u4
  - id: page_sizes
    type: u4
    repeat: expr
    repeat-expr: num_pages

instances:
  page_starts:
    value: (0..num_pages - 1).map { |i| 4 + 4 + 4 * num_pages + (0..i - 1).map { |j| page_sizes[j] }.reduce(0, :+) }
  pages:
    value: (0..num_pages - 1).map { |i| _io.seek(page_starts[i]); Page.new(_io.substream(page_sizes[i]), self) }

types:
  page:
    params:
      - id: _parent
        type: binarycookies
    seq:
      - id: unk1
        size: 4
        doc: Unknown; often 0x00 0x00 0x01 0x00
      - id: num_cookies
        type: u4
      - id: cookie_offsets
        type: u4
        repeat: expr
        repeat-expr: num_cookies
        doc: Offset of each cookie from start of this page
    instances:
      cookies:
        value: (0..num_cookies - 1).map { |i| _io.seek(cookie_offsets[i]); sz = _io.read_u4be; _io.seek(cookie_offsets[i]); Cookie.new(_io.substream(sz), _parent) }

  cookie:
    params:
      - id: _parent
        type: binarycookies
    seq:
      - id: size
        type: u4
        doc: Total size of this cookie record in bytes
      - id: unk1
        size: 4
      - id: flags
        type: u4
        enum: cookie_flags
        doc: secure=1, http_only=4
      - id: unk2
        size: 4
      - id: domain_offset
        type: u4
        doc: Offset of domain string from start of this cookie
      - id: name_offset
        type: u4
      - id: path_offset
        type: u4
      - id: value_offset
        type: u4
      - id: unk
        size: 8
      - id: expiry_date_epoch
        type: f8
        doc: Expiry in Apple epoch (seconds since 2001-01-01)
      - id: create_date_epoch
        type: f8
        doc: Creation in Apple epoch
    instances:
      domain:
        value: (_pos = _io.pos; _io.seek(domain_offset); v = _io.read_bytes_term(0, false, true); _io.seek(_pos); v.force_encoding("UTF-8"))
      name:
        value: (_pos = _io.pos; _io.seek(name_offset); v = _io.read_bytes_term(0, false, true); _io.seek(_pos); v.force_encoding("UTF-8"))
      path:
        value: (_pos = _io.pos; _io.seek(path_offset); v = _io.read_bytes_term(0, false, true); _io.seek(_pos); v.force_encoding("UTF-8"))
      value:
        value: (_pos = _io.pos; _io.seek(value_offset); v = _io.read_bytes_term(0, false, true); _io.seek(_pos); v.force_encoding("UTF-8"))

enums:
  cookie_flags:
    1: secure
    4: http_only
