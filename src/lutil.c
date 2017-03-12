#include "lutil.h"

void lutil_eltset(t_atom * src, t_atom * dest)
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



void lutil_everyn(int argc, t_atom *argv,  t_atom * ret, int _n, int _offset)
{
    int n = _n < 1 ? 1 : (_n > argc ? argc : _n);
    int offset = _offset % n;
    int sz = (int)(argc/n);
    int i, idx;
    for(i=0;i<sz; i++)
    {   idx = (i * n) + offset;
        idx = idx > argc ? argc : idx;
        lutil_eltset(&argv[idx], &ret[i]);
    };


}

double lutil_sum(int argc, t_atom * argv)
{
    int i;
    double sum = 0;
    for(i=0;i<argc; i++)
    {
        if(argv[i].a_type == A_FLOAT)
        {
            sum += argv[i].a_w.w_float;
        };
    };
    return sum;
}

double lutil_prod(int argc, t_atom * argv)
{
    int i;
    double prod = 1;
    for(i=0;i<argc; i++)
    {
        if(argv[i].a_type == A_FLOAT)
        {
            prod *= argv[i].a_w.w_float;
        };
    };
    return prod;
}

int lutil_numdigits(int num)
{
    int count = 0;
    while(num)
    {
        num /= 10;
        ++count;
    };
    return count;
}

int lutil_untilf(int argc, t_atom * argv, t_float f)
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

int lutil_untilstr(int argc, t_atom * argv, char * cmp)
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

void lutil_eltswap(t_atom * src, t_atom * dest)
{
    t_atom temp;
    lutil_eltset(dest, &temp);
    lutil_eltset(src, dest);
    lutil_eltset(&temp, src);
}

void lutil_listcopy(t_atom * src, int srcsz, t_atom * dest, int destsz)
{
    int loopsz = srcsz < destsz ? srcsz : destsz;
    int i=0;
    for(i=0;i<loopsz; i++)
    {
        lutil_eltset(&src[i],&dest[i]);
    };

}

