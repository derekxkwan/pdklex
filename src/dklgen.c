/* Copyright 2017 - Derek Kwan
 *  * Distributed under GPL v3 */

#include "m_pd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "dkmem.h"
#include "dkrnd.h"

static t_class *dklgen_class;

typedef enum{
    NONE,
    RANDFU, //random float, uniform
    RANDIU, //random int, uniform
    XRANDFU, //random float, non-repeating uniform
    XRANDIU, //random int, non-repeating uniform
    COUNT,
    FIB,
    LUCAS,
    EUCLID


} munger;


typedef struct _dklgen {
	t_object x_obj;
        munger x_gen;
        t_dkrnd * x_dkrnd;
        t_dkmem * x_dkparam;
        int       x_paramlen;
        t_inlet * x_genlet;
        t_outlet * x_outlet; 
} t_dklgen;

static void dklgen_setgen(t_dklgen *x, t_symbol * s)
{
    char * cs = s->s_name;
    if(strcmp(cs, "randfu") == 0) x->x_gen = RANDFU;
    else if(strcmp(cs, "randiu") == 0) x->x_gen = RANDIU;
    else if(strcmp(cs, "count") == 0) x->x_gen = COUNT;
    else if(strcmp(cs, "fib") == 0) x->x_gen = FIB;
    else if(strcmp(cs, "lucas") == 0) x->x_gen = LUCAS;
    else if(strcmp(cs, "euclid") == 0) x->x_gen = EUCLID;
    else if(strcmp(cs, "xrandfu") == 0) x->x_gen = XRANDFU;
    else if(strcmp(cs, "xrandiu") == 0) x->x_gen = XRANDIU;
}

static void dklgen_eltset(t_atom * src, t_atom * dest)
{
    if(src->a_type == A_FLOAT)
    {
        SETFLOAT(dest, src->a_w.w_float);
    }
    else if(src->a_type == A_SYMBOL)
    {
        SETSYMBOL(dest, src->a_w.w_symbol);
    };
}

static void dklgen_eltswap(t_atom * src, t_atom * dest)
{
    t_atom temp;
    dklgen_eltset(dest, &temp);
    dklgen_eltset(src, dest);
    dklgen_eltset(&temp, src);
}

static void dklgen_listcopy(t_atom * src, int srcsz, t_atom * dest, int destsz)
{
    int loopsz = srcsz < destsz ? srcsz : destsz;
    int i=0;
    for(i=0;i<loopsz; i++)
    {
        dklgen_eltset(&src[i],&dest[i]);
    };

}


static void dklgen_params(t_dklgen *x, t_symbol *s, int argc, t_atom * argv)
{
    
    dkmem_alloc(x->x_dkparam, argc);
    dklgen_listcopy(argv, argc, x->x_dkparam->m_data, x->x_dkparam->m_allocsz);
    x->x_paramlen = argc;
}


static void dklgen_gen(t_dklgen *x, t_symbol *s, int argc, t_atom * argv)
{
    if(argc)
    {
        if(argv->a_type == A_SYMBOL)
        {
            dklgen_setgen(x, argv->a_w.w_symbol);
            argc--;
            argv++;
            if(argc) dklgen_params(x, &s_, argc, argv);
            else x->x_paramlen = 0;
        };
    };
}

static int dklgen_untilf(int argc, t_atom * argv, t_float f)
{
    //returns idx of first instance of f or returns -1 if not found
    int i, idx = -1;
    for(i=0;i<argc; i++)
    {
        if(argv[i].a_type == A_FLOAT)
        {
            if(f == argv[i].a_w.w_float)
            {
                idx = i;
                break;
            };
        };
    };
    return idx;
}

static int dklgen_untilstr(int argc, t_atom * argv, char * cmp)
{
    int i, idx = -1;
    for(i=0; i<argc; i++)
    {
        if(argv[i].a_type == A_SYMBOL)
        {
            if(strcmp(cmp, argv[i].a_w.w_symbol->s_name) == 0)
            {
                idx = i;
                break;
            };
        };
    };
    return idx;
}

static void dklgen_output(t_dklgen * x, t_symbol * s, int argc, t_atom * argv)
{
    if(s) outlet_anything(x->x_outlet, s, argc, argv);
    else outlet_list(x->x_outlet, 0, argc, argv);
}

static int dklgen_numdigits(int num)
{
    int count = 0;
    while(num)
    {
        num /= 10;
        ++count;
    };
    return count;
}

static void dklgen_randfu(t_dklgen *x, int plen, t_atom * p)
{
    int i,retsz;
    double rnd, lo, hi,temp, f0, f1, f2;

    //check if args are floats and get them if so
    switch(plen)
    {
        case 0:
            break;
        case 1:
            if(p[0].a_type == A_FLOAT) f0 = (double) p[0].a_w.w_float;
            else plen = 0;
            break;
        case 2:
            if(p[0].a_type == A_FLOAT)
            {
                f0 = (double) p[0].a_w.w_float;
                if(p[1].a_type == A_FLOAT) f1 = (double) p[1].a_w.w_float;
                else plen = 1;
            }
            else plen = 0;
            break;
        default:
        case 3:
             if(p[0].a_type == A_FLOAT)
            {
                f0 = (double) p[0].a_w.w_float;
                if(p[1].a_type == A_FLOAT)
                {
                    f1 = (double) p[1].a_w.w_float;
                    if(p[2].a_type == A_FLOAT)
                    {
                        f2 = (double)p[2].a_w.w_float;
                        plen = 3; //max
                    }
                    else plen = 2;
                }
                else plen = 1;
            }
            else plen = 0;
            break;
 

    };

    //now for setting args
    switch(plen)
    {
        case 0:
            retsz = 1;
            lo = 0;
            hi = 1;
            break;
        case 1:
            retsz = f0 < 1 ? 1 : (int) f0;
            lo = 0;
            hi = 1;
            break;
        case 2:
            retsz = f1 < 1 ? 1 : (int) f1;
            lo = 0;
            hi = f0;
            if(lo > hi)
            {
               temp = lo;
               lo = hi;
               hi = temp;
            };
            break;
        default:
        case 3:
            retsz = f2 < 1 ? 1 : (int) f2;
            lo = f0;
            hi = f1;
            if( lo > hi)
            {
                temp = lo;
                lo = hi;
                hi = temp;
            };
            break;

    };

    t_atom ret[retsz];
    dkrnd_range(x->x_dkrnd, hi-lo);
    for(i=0;i< retsz; i++)
    {
        rnd = dkrnd_next(x->x_dkrnd) + lo;
        SETFLOAT(&ret[i], rnd);
    };  
    dklgen_output(x, 0, retsz, ret);
}

static void dklgen_router(t_dklgen * x)
{
    t_atom * params = x->x_dkparam->m_data;
    int paramlen = x->x_paramlen;
    switch(x->x_gen)
    {
        case NONE: break;
        case RANDFU: dklgen_randfu(x, paramlen, params);
                     break;
                               
        default: break;

    };
}


static void *dklgen_new(t_symbol *s, int argc, t_atom * argv){
	t_dklgen *x = (t_dklgen *)pd_new(dklgen_class);
	x->x_dkparam = dkmem_new();
        x->x_gen = NONE;
        x->x_dkrnd = dkrnd_new(0);
        x->x_paramlen = 0;
	x->x_genlet = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_list, gensym("gen"));
	x->x_outlet = outlet_new(&x->x_obj, &s_anything);
	return (x);
}

static void *dklgen_free(t_dklgen *x){
        dkmem_free(x->x_dkparam);
        dkrnd_free(x->x_dkrnd);
        inlet_free(x->x_genlet);
	outlet_free(x->x_outlet);
	
	return (void *)x;
}


void dklgen_setup(void){
	dklgen_class = class_new(gensym("dklgen"), (t_newmethod)dklgen_new, (t_method)dklgen_free,
			sizeof(t_dklgen), 0, A_GIMME, 0);
	class_addbang(dklgen_class, dklgen_router);
    class_addmethod(dklgen_class, (t_method)dklgen_gen, gensym("gen"), A_GIMME, 0);
}
