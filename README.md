# libgdbruby
Note: this is an experimental project.

## Features
* Add `ruby`, `pry` command to gdb
* Add a Ruby function `gdb_execute(command)` which executes a command on gdb and captures its output as String

## Installation
```
$ git clone https://github.com/Charo-IT/libgdbruby.git
$ cd libgdbruby/libgdbruby
$ ruby extconf.rb
$ make
$ echo "source /path/to/gdbruby.py" >> ~/.gdbinit
```

## Sample
```
$ gdb /bin/sh
Reading symbols from /bin/sh...(no debugging symbols found)...done.
gdb-peda$ ruby puts "Hello world from gdb!"
Hello world from gdb!
gdb-peda$ pry
[1] pry(main)> puts gdb_execute("i file")
Symbols from "/bin/dash".
Local exec file:
        `/bin/dash', file type elf64-x86-64.
        Entry point: 0x18b30
        0x0000000000000238 - 0x0000000000000254 is .interp
        0x0000000000000254 - 0x0000000000000274 is .note.ABI-tag
        0x0000000000000274 - 0x0000000000000298 is .note.gnu.build-id
        0x0000000000000298 - 0x0000000000000634 is .gnu.hash
        0x0000000000000638 - 0x0000000000001838 is .dynsym
        0x0000000000001838 - 0x0000000000001f2a is .dynstr
        0x0000000000001f2a - 0x00000000000020aa is .gnu.version
        0x00000000000020b0 - 0x0000000000002120 is .gnu.version_r
        0x0000000000002120 - 0x0000000000004430 is .rela.dyn
        0x0000000000004430 - 0x000000000000444a is .init
        0x0000000000004450 - 0x0000000000004460 is .plt
        0x0000000000004460 - 0x0000000000004728 is .plt.got
        0x0000000000004730 - 0x000000000001c732 is .text
        0x000000000001c734 - 0x000000000001c73d is .fini
        0x000000000001c740 - 0x000000000001e3e2 is .rodata
        0x000000000001e3e4 - 0x000000000001eff8 is .eh_frame_hdr
        0x000000000001eff8 - 0x000000000002321c is .eh_frame
        0x0000000000223f68 - 0x0000000000223f70 is .init_array
        0x0000000000223f70 - 0x0000000000223f78 is .fini_array
        0x0000000000223f78 - 0x0000000000223f80 is .jcr
        0x0000000000223f80 - 0x0000000000224b48 is .data.rel.ro
        0x0000000000224b48 - 0x0000000000224d08 is .dynamic
        0x0000000000224d08 - 0x0000000000225000 is .got
        0x0000000000225000 - 0x0000000000225220 is .data
        0x0000000000225220 - 0x0000000000227e50 is .bss
=> nil
[2] pry(main)> puts gdb_execute("i file").length
1439
=> nil
[3] pry(main)> quit
gdb-peda$ q
```
