#coding:utf-8
require "mkmf"

gdb_version = `gdb --version`.lines[0].strip.split[-1].split(".").map(&:to_i)
cpp = gdb_version[0] >= 8

have_library("stdc++")
$CXXFLAGS << " -std=c++11"
if cpp
  $CFLAGS << " -DGDB_CPP"
  $CXXFLAGS << " -DGDB_CPP"
end

create_makefile("libgdbruby")
