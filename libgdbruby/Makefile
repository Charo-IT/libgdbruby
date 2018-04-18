# WIP Makefile

arch = x86_64-linux
rubyincdir = $(MY_RUBY_HOME)/include/ruby-2.4.0
rubyarchincdir = $(rubyincdir)/$(arch)
rubylibdir = $(MY_RUBY_HOME)/lib

build: libgdbruby.so

libgdbruby.so: libgdbruby.c
	cc -o libgdbruby.so libgdbruby.c -fPIC -shared -I $(rubyincdir) -I $(rubyarchincdir) -L $(rubylibdir) -lruby -Wl,-rpath $(rubylibdir)

clean:
	rm -f libgdbruby.so
