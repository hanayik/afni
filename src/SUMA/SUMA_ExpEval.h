#ifndef SUMA_EXPEVAL_INCLUDED
#define SUMA_EXPEVAL_INCLUDED

char *SUMA_bool_eval( char *expr, byte *res );
void SUMA_bool_eval_test( char *expr, byte exprval );
SUMA_Boolean SUMA_bool_mask_eval(int N_vals, int N_vars, byte **mask, char *expr,
                                 byte *res);

#endif
