#ifndef GDB_CPP
#include <setjmp.h>
#endif

enum command_class {
    no_class = -1
};

enum return_reason {
    RETURN_QUIT = -2,
    RETURN_ERROR
};

#define RETURN_MASK(reason) (1 << (int)(-reason))

typedef enum {
    RETURN_MASK_QUIT = RETURN_MASK(RETURN_QUIT),
    RETURN_MASK_ERROR = RETURN_MASK(RETURN_ERROR),
    RETURN_MASK_ALL = (RETURN_MASK_QUIT | RETURN_MASK_ERROR)
} return_mask;

struct gdb_exception{
    enum return_reason reason;
    int error;
    const char *message;
};

#ifdef GDB_CPP
extern void *exception_try_scope_entry(void);
extern void exception_try_scope_exit(void *saved_state);
extern void exception_rethrow(void);

struct exception_try_scope{
    exception_try_scope(){
        saved_state = exception_try_scope_entry();
    }
    ~exception_try_scope(){
        exception_try_scope_exit(saved_state);
    }

    void *saved_state;
};

#define TRY \
    { \
        try{ \
            exception_try_scope exception_try_scope_instance;   \
            do{

#define CATCH(EXCEPTION, MASK)  \
            }while(0);  \
        }catch(struct gdb_exception ## _ ## MASK &EXCEPTION)

#define END_CATCH   \
        catch(...){ \
            exception_rethrow();    \
        }   \
    }

struct gdb_exception_RETURN_MASK_ALL : public gdb_exception{
};

#else

extern "C" {
    extern jmp_buf *exceptions_state_mc_init(void);
    extern int exceptions_state_mc_action_iter(void);
    extern int exceptions_state_mc_action_iter_1(void);
    extern int exceptions_state_mc_catch(struct gdb_exception *, int);
}

#define TRY \
    {   \
        jmp_buf *buf = exceptions_state_mc_init();   \
        setjmp(*buf);   \
    }   \
    while(exceptions_state_mc_action_iter())    \
        while(exceptions_state_mc_action_iter_1())

#define CATCH(EXCEPTION, MASK)  \
    {   \
        struct gdb_exception EXCEPTION; \
        if(exceptions_state_mc_catch(&(EXCEPTION), MASK))

#define END_CATCH   \
    }

#endif
