#include <stdio.h>
#include <ruby.h>

enum command_class {
    no_class = -1
};

extern void *add_com(const char *name, enum command_class theclass, void *func, const char *doc);
extern char *execute_command_to_string(const char *p, int from_tty);
extern void xfree(void *p);

static VALUE gdbruby_binding;

// just call $gdbruby_binding.eval(command)
static VALUE eval_ruby_internal(VALUE command){
    RUBY_INIT_STACK;

    return rb_funcall(gdbruby_binding, rb_intern("eval"), 1, command);
}

static void eval_ruby(const char *script){
    int state;

    VALUE result;
    VALUE command;
    RUBY_INIT_STACK;

    if(script == NULL){
        puts("no script given");
        return;
    }

    command = rb_str_new2(script);
    result = rb_protect(eval_ruby_internal, command, &state);;

    if(state){
        puts("-> error");
    }
}

// just call $gdbruby_binding.pry
static VALUE launch_pry_internal(){
    RUBY_INIT_STACK;

    return rb_funcall(gdbruby_binding, rb_intern("pry"), 0);
}

static void launch_pry(){
    int state;
    VALUE result;
    RUBY_INIT_STACK;

    result = rb_protect(launch_pry_internal, Qnil, &state);

    if(state){
        puts("-> error");
    }
}

static VALUE gdb_execute(VALUE self, VALUE arg1){
    char *code;
    char *result;
    VALUE rb_result;
    RUBY_INIT_STACK;

    SafeStringValue(arg1);

    code = calloc(1, RSTRING_LEN(arg1) + 1);
    if(code == NULL){
        puts("-> error");
        return Qnil;
    }
    memcpy(code, RSTRING_PTR(arg1), RSTRING_LEN(arg1));

    result = execute_command_to_string(code, 0);
    free(code);

    if(result == NULL){
        return Qnil;
    }

    rb_result = rb_str_new2(result);
    xfree(result);

    return rb_result;
}

static void __attribute__((constructor)) onload(){
    int a = 3;
    char *args[] = {"ruby", "-e", "", NULL};
    char **arg = args;
    int state;

    /* Initialize Ruby VM */
    ruby_sysinit(&a, &arg);
    {
        RUBY_INIT_STACK;

        ruby_init();
        ruby_options(a, arg);  // a hacky way to get ruby initialized :P

        rb_define_global_function("gdb_execute", gdb_execute, 1);
        rb_eval_string_protect("require 'pry'", &state);  // why doesn't rb_require("pry") work? :(
        if(state){
            puts("failed to load pry");
        }

        gdbruby_binding = rb_funcall(Qnil, rb_intern("binding"), 0);
        rb_define_readonly_variable("gdbruby_binding", &gdbruby_binding);
    }

    /* Add ruby and pry command to gdb */
    add_com("ruby", no_class, eval_ruby, "eval ruby\n");
    add_com("pry", no_class, launch_pry, "launch pry\n");
}
