/** phantom actuator effect using gaussian approach from Yun, Oh, and Choi
 * (10.1109/WHC.2019.8816104) */

#include "m_pd.h"
#include <math.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

const t_float EPSILON = 0.5;
static t_class *gphantom_class;

typedef struct _gphantom {
  t_object x_obj;
  t_int i_n, i_m;        // n x m dense grid approach
  t_float f_x, f_y, f_a; // (x, y) and amplitude
  t_float f_epsilon;     // shape parameter for Gaussian
} t_gphantom;

void gphantom_set(t_gphantom *x, t_floatarg f1, t_floatarg f2, t_floatarg f3) {
  x->f_x = MIN(x->i_m - 1, MAX(0, f1));
  x->f_y = MIN(x->i_n - 1, MAX(0, f2));
  x->f_a = MAX(0, f3);
}

void gphantom_reset(t_gphantom *x) {
  x->f_x = 0;
  x->f_y = 0;
  x->f_a = 0;
}

t_float gaussian(t_float epsilon, t_float r) {
  return (t_float)exp(-pow(epsilon * r, 2));
}

void gphantom_bang(t_gphantom *x) {
  int argc = x->i_n * x->i_m;
  t_atom *argv = (t_atom *)getbytes(sizeof(t_atom) * argc);

  for (int i = 0; i < x->i_m; i++) {   // x coord
    for (int j = 0; j < x->i_n; j++) { // y coord
      t_float distance =
          (float)sqrt(pow((float)i - x->f_x, 2) + pow((float)j - x->f_y, 2));
      t_float amp = x->f_a * gaussian(x->f_epsilon, distance);
      size_t index = (i * x->i_n) + j;
      SETFLOAT(argv + index, amp);
    }
  }

  outlet_list(x->x_obj.ob_outlet, &s_list, argc, argv);
}

void gphantom_list(t_gphantom *x, t_symbol *s, t_int argc, t_atom *argv) {
  switch (argc) {
  case 3:
    gphantom_set(x, atom_getfloat(argv), atom_getfloat(argv + 1),
                 atom_getfloat(argv + 2));
    gphantom_bang(x);
    break;
  default:
    error(
        "[gphantom ]: three arguments are expected (x, y, amp); received %ld!",
        argc);
    break;
  }
}

void gphantom_shape(t_gphantom *x, t_float f) {
  x->f_epsilon = f;
  post("[gphantom ]: shape parameter updated to %f", x->f_epsilon);
}

void *gphantom_new(t_floatarg n, t_floatarg m) {
  t_gphantom *x = (t_gphantom *)pd_new(gphantom_class);

  x->i_n = n;
  x->i_m = m;
  x->f_epsilon = EPSILON;
  gphantom_reset(x);
  outlet_new(&x->x_obj, &s_list);

  post("[gphantom ]: created instance with %ld by %ld grid", x->i_n, x->i_m);

  return (void *)x;
}

void gphantom_setup(void) {
  gphantom_class =
      class_new(gensym("gphantom"), (t_newmethod)gphantom_new, 0,
                sizeof(t_gphantom), CLASS_DEFAULT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addbang(gphantom_class, (t_method)gphantom_bang);
  class_addlist(gphantom_class, (t_method)gphantom_list);
  class_addmethod(gphantom_class, (t_method)gphantom_shape, gensym("shape"), A_DEFFLOAT, 0);

  class_sethelpsymbol(gphantom_class, gensym("docs/gphantom"));
}
