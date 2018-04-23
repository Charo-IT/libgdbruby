#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ruby.h>
#ifdef GDB_CPP
#include <string>
#endif
#include "gdb.h"

#ifdef GDB_CPP
extern void *add_com(const char *, command_class, void (*)(char const *, int), const char *);
extern std::string execute_command_to_string(const char *, int);
#else
extern "C" void *add_com(const char *, command_class, void (*)(char const *, int), const char *);
extern "C" char *execute_command_to_string(const char *, int);
#endif

static VALUE gdbruby_binding;
static VALUE rb_eGdbError;

// just call $gdbruby_binding.eval(command)
static VALUE eval_ruby_internal(VALUE command){
    RUBY_INIT_STACK;

    return rb_funcall(gdbruby_binding, rb_intern("eval"), 1, command);
}

static VALUE error_handler(VALUE args, VALUE exception_object){
    unsigned long i, length;
    VALUE message;
    VALUE backtrace;
    VALUE item;
    RUBY_INIT_STACK;

    if(NIL_P(exception_object)){
        puts("error");
        return Qnil;
    }

    message = rb_funcall(exception_object, rb_intern("to_s"), 0);
    backtrace = rb_funcall(exception_object, rb_intern("backtrace"), 0);

    if(NIL_P(message)){
        printf("%1$s: %1$s\n", rb_obj_classname(exception_object));
    }else{
        SafeStringValue(message);
        printf("%s: %s\n", rb_obj_classname(exception_object), RSTRING_PTR(message));
    }

    if(NIL_P(backtrace)){
        return Qnil;
    }

    length = NUM2LONG(rb_funcall(backtrace, rb_intern("length"), 0));
    for(i = 0; length > 1 && i < length - 1; i++){
        item = rb_ary_entry(backtrace, i);
        if(NIL_P(item)){
            continue;
        }
        SafeStringValue(item);
        printf("    from %s\n", RSTRING_PTR(item));
    }

    return Qnil;
}

static void eval_ruby(const char *script){
    VALUE command;
    RUBY_INIT_STACK;

    if(script == NULL){
        puts("no script given");
        return;
    }

    command = rb_str_new2(script);
    rb_rescue((VALUE (*)(ANYARGS))eval_ruby_internal, command, (VALUE (*)(ANYARGS))error_handler, Qnil);
}

// just call $gdbruby_binding.pry
static VALUE launch_pry_internal(){
    RUBY_INIT_STACK;

    return rb_funcall(gdbruby_binding, rb_intern("pry"), 0);
}

static void launch_pry(){
    int state;
    RUBY_INIT_STACK;

    rb_protect((VALUE (*)(VALUE))launch_pry_internal, Qnil, &state);

    if(state){
        puts("-> error");
    }
}

static VALUE gdb_execute(VALUE self, VALUE arg1){
    char *code;
#ifdef GDB_CPP
    std::string result;
    char *result_cstr;
#else
    char *result;
#endif
    VALUE rb_result;
    RUBY_INIT_STACK;

    SafeStringValue(arg1);

    code = (char *)calloc(1, RSTRING_LEN(arg1) + 1);
    if(code == NULL){
        puts("-> error");
        return Qnil;
    }
    memcpy(code, RSTRING_PTR(arg1), RSTRING_LEN(arg1));

    TRY{
        result = execute_command_to_string(code, 0);
    }CATCH(except, RETURN_MASK_ALL){
        free(code);

        if(except.reason == RETURN_QUIT){
            rb_raise(rb_eInterrupt, "Interrupt");
        }else{
            rb_raise(rb_eGdbError, "%s", except.message);
        }
    }END_CATCH

    free(code);

#ifdef GDB_CPP
    result_cstr = strdup(result.c_str());

    rb_result = rb_str_new2(result_cstr);
    free(result_cstr);
#else
    if(result == NULL){
        return Qnil;
    }

    rb_result = rb_str_new2(result);
    free(result);
#endif

    return rb_result;
}

static void __attribute__((constructor)) onload(){
    int a = 3;
    const char *args[] = {"ruby", "-e", "", NULL};
    char **arg = (char **)args;
    int state;

    /* Initialize Ruby VM */
    ruby_sysinit(&a, &arg);
    {
        RUBY_INIT_STACK;

        ruby_init();
        ruby_options(a, arg);  // a hacky way to get ruby initialized :P
        ruby_script("gdbruby");

        rb_define_global_function("gdb_execute", (VALUE (*)(ANYARGS))gdb_execute, 1);
        rb_eval_string_protect("require 'pry'", &state);  // why doesn't rb_require("pry") work? :(
        if(state){
            puts("failed to load pry");
        }

        gdbruby_binding = rb_funcall(Qnil, rb_intern("binding"), 0);
        rb_define_readonly_variable("gdbruby_binding", &gdbruby_binding);

        rb_eGdbError = rb_define_class("GdbError", rb_eStandardError);
    }

    /* Add ruby and pry command to gdb */
    add_com("ruby", no_class, (void (*)(char const *, int))eval_ruby, "Evaluate a Ruby command.");
    add_com("pry", no_class, (void (*)(char const *, int))launch_pry, "Launch pry.");
}
