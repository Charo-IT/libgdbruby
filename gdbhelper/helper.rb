#coding:ascii-8bit
require "tempfile"

def tmpfile(&block)
  Tempfile.create("libgdbruby-", binmode: true, &block)
end

def dumpmem(from, to)
  mem = nil
  tmpfile{|fp|
    begin
      gdb_execute("dump memory %s 0x%x 0x%x" % [fp.path, from, to])
    rescue
      return nil
    end
    fp.flush
    mem = fp.read
  }
  mem
end

def readmem(addr, length)
  dumpmem(addr, addr + length)
end

def read_int(addr)
  readmem(addr, 4).unpack("L")[0]
rescue
  nil
end

def read_long(addr)
  readmem(addr, 8).unpack("Q")[0]
rescue
  nil
end

def read_string(addr)
  buf = ""
  while true
    c = readmem(addr + buf.length, 1)
    break if c.nil? || c.length == 0 || c.ord == 0
    buf << c
  end
  buf
end

def writemem(addr, data)
  return if data.length == 0

  tmpfile{|fp|
    fp.write(data.b)
    fp.flush
    begin
      gdb_execute("restore %s binary 0x%x" % [fp.path, addr])
    rescue
    end
  }
end

def write_int(addr, value)
  writemem(addr, [value].pack("L"))
end

def write_long(addr, value)
  writemem(addr, [value].pack("Q"))
end
