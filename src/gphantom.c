/** phantom actuator effect using gaussian approach from Yun, Oh, and Choi (10.1109/WHC.2019.8816104) */

#include "m_pd.h"
#include <math.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

static t_class *gphantom_class;

typedef struct _gphantom {
    t_object x_obj;
    t_int i_n, i_m; // n x m dense grid approach
    t_float f_x, f_y, f_a; // (x, y) and amplitude
} t_gphantom;

void gphantom_set(t_gphantom *x, t_floatarg f1, t_floatarg f2, t_floatarg f3) {
    x->f_x = MIN(x->i_m, MAX(0, f1));
    x->f_y = MIN(x->i_n, MAX(0, f2));
    x->f_a = MAX(0, f3);
}

void gphantom_reset(t_gphantom *x) {
    x->f_x = 0;
    x->f_y = 0;
    x->f_a = 0;
}

void gphantom_bang(t_gphantom *x) {
   // TODO do computation
   // make a list
   // outlet_list(x->, &s_list, int argc, t_atom *argv)
}

void gphantom_list(t_gphantom *x, t_symbol *s, t_int argc, t_atom *argv) {
    switch (argc) {
        case 3:
            gphantom_set(x, atom_getfloat(argv), atom_getfloat(argv + 1), atom_getfloat(argv + 2));
            gphantom_bang(x);
            break;
        default:
            error("[gphantom ]: three arguments are expected (x, y, amp)!");
            break;
    }
}

void *gphantom_new(t_floatarg n, t_floatarg m) {
    t_gphantom *x = (t_gphantom *)pd_new(gphantom_class);

    x->i_n = n;
    x->i_m = m;
    post("Created with %d, %d", n, m);
    gphantom_reset(x);
    outlet_new(&x->x_obj, &s_list);

    return (void *)x;
}

void gphantom_setup(void) {
    gphantom_class = class_new(gensym("gphantom"),
            (t_newmethod)gphantom_new,
            0, sizeof(t_gphantom),
            CLASS_DEFAULT,
            A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addbang(gphantom_class, (t_method)gphantom_bang);
    class_addlist(gphantom_class, (t_method)gphantom_list);
}
