/* Copyright 2017 - Derek Kwan
 *  * Distributed under GPL v3 */

#include "lutil.h"
#include <stdlib.h>
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
    RPT1, //similar to dklmunge rpt but with specifying atoms
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
    else if(strcmp(cs, "rpt1") == 0) x->x_gen = RPT1;
}


static void dklgen_params(t_dklgen *x, t_symbol *s, int argc, t_atom * argv)
{
    
    dkmem_alloc(x->x_dkparam, argc);
    lutil_listcopy(argv, argc, x->x_dkparam->m_data, x->x_dkparam->m_allocsz);
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

static void dklgen_output(t_dklgen * x, t_symbol * s, int argc, t_atom * argv)
{
    if(s) outlet_anything(x->x_outlet, s, argc, argv);
    else outlet_list(x->x_outlet, 0, argc, argv);
}


static void dklgen_randnum(t_dklgen *x, int plen, t_atom * p, int itype, int norpt)
{
    //itype = if 1, typecast to int; norpt = if 1, don't allow repeats
    int i,retsz;
    double rnd, lo, hi,temp, f0, f1, f2, range;

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
    if(itype)
    {
        if(plen < 2)
        {
            lo = 0;
            hi = 2;
            range = 1.5;
        }
        else
        {
            lo = (int) lo;
            hi = (int) hi;
            range = (int)hi-lo;
            range -= 0.5; //make sure we don't hit hi of range ever
        };
        dkrnd_range(x->x_dkrnd, range);
    }
    else
    {
        range = hi-lo;
        dkrnd_range(x->x_dkrnd, range);
    };
    
    for(i=0;i< retsz; i++)
    {
        rnd = dkrnd_next(x->x_dkrnd) + lo;
        if(itype) rnd = (int)rnd;
        SETFLOAT(&ret[i], (t_float)rnd);
    };  
    dklgen_output(x, 0, retsz, ret);
}

static void dklgen_rpt1(t_dklgen *x, int plen, t_atom * p)
{
    int halflist = (int)plen/2;
    int retsz, i, j, rptamt, retidx = 0;
    t_atom rpts[halflist];
    lutil_everyn( plen, p, rpts, 2, 1); //get the repeats only
    retsz = lutil_sum(halflist, rpts); //get the sum of all the repeats
    t_atom ret[retsz];
    //format: atom, rpt, atom, rpt
    for(i=0; i<halflist; i++)
    {
       if(p[(i*2)+1].a_type == A_FLOAT)
       {
        rptamt = (int)p[(i*2)+1].a_w.w_float;
        if(rptamt >= 1)
        {
            for(j=0;j<rptamt; j++)
            {
                lutil_eltset(&p[(i*2)], &ret[retidx]);
                retidx++;
            };
        };
       };

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
        case RANDFU: dklgen_randnum(x, paramlen, params, 0, 0);
                     break;
        case RANDIU: dklgen_randnum(x, paramlen, params, 1, 0);
                     break;
        case RPT1: if(paramlen >= 2) dklgen_rpt1(x, paramlen, params);
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
