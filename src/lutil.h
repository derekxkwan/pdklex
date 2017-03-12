
#include "m_pd.h"
#include <string.h>

void lutil_everyn(int argc, t_atom *argv, t_atom * ret, int _n, int _offset);
double lutil_sum(int argc, t_atom * argv);
double lutil_prod(int argc, t_atom * argv);
int lutil_numdigits(int num);
int lutil_untilf(int argc, t_atom * argv, t_float f);
int lutil_untilstr(int argc, t_atom * argv, char * cmp);
void lutil_eltswap(t_atom * src, t_atom * dest);
void lutil_eltset(t_atom * src, t_atom * dest);
void lutil_listcopy(t_atom * src, int srcsz, t_atom * dest, int destsz);
