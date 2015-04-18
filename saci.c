/* Plugin structure generated by Schiavoni Pure Data external Generator */
#include "m_pd.h"
#include "g_canvas.h"
#include "g_all_guis.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define OBJ_MAX 100

// bng, cnv, message, text, vu, hradio, vradio, vsl, hsl, nbx, tgl, gatom
static t_class *saci_class;

t_glist * saci_find_parent(t_glist * glist);

typedef enum{SACI_UP, SACI_DOWN, SACI_LEFT, SACI_RIGHT, SACI_RANDOM} t_saci_mode;

typedef struct _saci {
   t_object x_obj;
   t_symbol *sp_name;
   t_glist *glist;
   t_canvas *x_canvas;
   t_float x_step;
   t_float y_step;
   t_saci_mode mode;

   t_int obj_count;
   char objs[OBJ_MAX][100]; //maximum object number

} t_saci;

/* ----------------------------------------------------
 set step
 -----------------------------------------*/
void saci_set_step(t_saci *x, t_floatarg f0){
   x->x_step = f0;
   x->y_step = f0;
}

/* ----------------------------------------------------
 set x step
 -----------------------------------------*/
void saci_set_x_step(t_saci *x, t_floatarg f0){
   x->x_step = f0;
}

/* ----------------------------------------------------
 set y step
 -----------------------------------------*/
void saci_set_y_step(t_saci *x, t_floatarg f0){
   x->y_step = f0;
}

/* ----------------------------------------------------
 set mode
 -----------------------------------------*/
void saci_set_mode(t_saci *x, t_symbol * sp_mode){
   if(strcmp(sp_mode->s_name, "left") == 0){
      x->mode = SACI_LEFT;
   }else if(strcmp(sp_mode->s_name, "right") == 0){
      x->mode = SACI_RIGHT;
   }else if(strcmp(sp_mode->s_name, "up") == 0){
      x->mode = SACI_UP;
   }else if(strcmp(sp_mode->s_name, "down") == 0){
      x->mode = SACI_DOWN;
   }else if(strcmp(sp_mode->s_name, "random") == 0){
      x->mode = SACI_RANDOM;
   }
}

/* ----------------------------------------------------
 set objs
 -----------------------------------------*/
void saci_set_objs(t_saci *x, t_symbol *s, int argc, t_atom *argv){
   if(strcmp(s->s_name, "objs") != 0)
      post("This message should not be here!");
   if(argc > OBJ_MAX)
      post("Saci is limited to mess up with only 100 objects");
   x->obj_count = argc;
   char str[80];
   int i = 0;
   for (i = 0; i < argc && i < OBJ_MAX; i++){
      atom_string(argv+i, str, 100);
      strcpy(x->objs[i], str);
    }
}

/* ----------------------------------------------------
 clear objs
 -----------------------------------------*/
void saci_set_all_objs(t_saci *x){
   x->obj_count = 0;
}

/* ----------------------------------------------------
 is object in list?
 -----------------------------------------*/
int saci_is_obj_in_list(t_saci *x, char * obj_name){
   if(x->obj_count == 0) //nothing selected, take everybody
      return 1;

   int i = 0;
   for(i = 0 ; i < x->obj_count; i++){
      if(strcmp(obj_name, x->objs[i]) == 0)
         return 1;
   }
   return 0;
}
/* ----------------------------------------------------
 set target canvas
 -----------------------------------------*/
void saci_set_target(t_saci *x, t_symbol * sp_name){
   x->sp_name = sp_name;
   if(strcmp(sp_name->s_name, "") == 0){
      t_canvas *canvas=(t_canvas*)glist_getcanvas(x->glist);
      x->x_canvas = canvas;
      return;
   }
   //It was empty. Now we need to find the parent
   if(strcmp(x->sp_name->s_name, "") != 0){
      t_glist * glist = saci_find_parent(x->x_canvas);
      t_canvas *canvas=(t_canvas*)glist_getcanvas(glist);
      x->x_canvas = canvas;
   }
}

/* ----------------------------------------------------
 do it!
 -----------------------------------------*/
void saci_do(t_saci *x, t_canvas * c){

   if(!glist_isvisible((t_glist *)c))
      return;
   int screen_w = c->gl_screenx2 - c->gl_screenx1,
       screen_h = c->gl_screeny2 - c->gl_screeny1;

   int x1, y1, x2, y2, obj_w, obj_h;
   int x_des = 0, y_des = 0;
   int shift;
   t_gobj * obj = NULL;

   srandom(time(NULL));

   for(obj = c->gl_list; obj; obj = obj->g_next){
      if(saci_is_obj_in_list(x, class_getname(pd_class(&(obj)->g_pd))) == 0)
         continue;
      gobj_getrect(obj, c, &x1, &y1, &x2, &y2);
      if(strcmp(class_getname(pd_class(&(obj)->g_pd)), "cnv") == 0){
         //screen: Get the bigger size between the visible width or the selectable area
         obj_w = (((t_my_canvas *)obj)->x_vis_w > x2 - x1)? ((t_my_canvas *)obj)->x_vis_w : x2 - x1;
         obj_h = (((t_my_canvas *)obj)->x_vis_h > y2 - y1)? ((t_my_canvas *)obj)->x_vis_h : y2 - y1;
         // Redefine the rectangle
         x2 = x1 + obj_w;
         y2 = y1 + obj_h;
      }else{
      //common objects get object position
         obj_w = x2 - x1;
         obj_h = y2 - y1;
      }
      if(x->mode == SACI_LEFT) x_des = -x->x_step;
      else if(x->mode == SACI_RIGHT) x_des = x->x_step;
      else if(x->mode == SACI_UP) y_des = -x->y_step;
      else if(x->mode == SACI_DOWN) y_des = x->y_step;
      else if(x->mode == SACI_RANDOM){
         shift = (int) (4.0*random() / (RAND_MAX + 1.0));
         switch(shift){
            case 0:{x_des = x->x_step;  y_des = 0;          break;}
            case 1:{x_des = -x->x_step; y_des = 0;          break;}
            case 2:{x_des = 0;          y_des = x->y_step;  break;}
            case 3:{x_des = 0;          y_des = -x->y_step; break;}
         }
      }

/*      post("Shift: %d", shift);*/
/*      post("screen w: %d screen h: %d obj_h: %d obj_w: %d", screen_w,*/
/*               screen_h, obj_h, obj_w);*/
/*      post("x1: %d x2: %d y1: %d y2: %d x_des: %d y_des: %d", x1, x2, y1, y2, x_des, y_des);*/
      if((x2 + x_des) > screen_w) x_des = screen_w - x1 - obj_w -1;
      else if((x1 + x_des) < 0) x_des = -x1;
      else if((y2 + y_des) > screen_h) y_des = screen_h - y1 - obj_h - 3;
      else if((y1 + y_des) < 0) y_des = -y1;
/*      post("x1: %d x2: %d y1: %d y2: %d x_des: %d y_des: %d", x1, x2, y1, y2, x_des, y_des);*/
      gobj_displace(obj, c, x_des, y_des);
   }
}

/* ----------------------------------------------------
 Recursive search
 -----------------------------------------*/
void saci_recursive_search(t_saci *x, t_glist *c){
   t_gobj*obj = NULL;
   for(obj = c->gl_list; obj; obj = obj->g_next) {
      if(pd_class(&obj->g_pd) != canvas_class)
         continue;
      //if subpatchname is empty, don't go recursively
      saci_recursive_search(x, ((t_glist *)obj));
      //look for subpatch name
      t_binbuf *bz = binbuf_new();
      t_symbol *patchsym;
      binbuf_addbinbuf(bz, ((t_canvas *)obj)->gl_obj.ob_binbuf);
      patchsym = atom_getsymbolarg(1, binbuf_getnatom(bz), binbuf_getvec(bz));
      binbuf_free(bz);
      if(strcmp(x->sp_name->s_name, patchsym->s_name) == 0
         || strcmp(x->sp_name->s_name, "") == 0)
         saci_do(x, (t_canvas *)obj);
   }
}

/* ----------------------------------------------------
 bang
 -----------------------------------------*/
void saci_bang(t_saci *x){
  t_glist *c = x->x_canvas;
   if(c == NULL){
      return;
   }
   if(strcmp(x->sp_name->s_name, "") == 0){
      saci_do(x, c);
      return;
   }
   saci_recursive_search(x, c);
}

/* ----------------------------------------------------
   Find parent
 -----------------------------------------*/
t_glist * saci_find_parent(t_glist * glist){
   while(glist->gl_owner){
      glist = glist->gl_owner;
   }
   return glist;
}

/* ----------------------------------------------------
 Constructor of the class
 -----------------------------------------*/
void * saci_new(t_symbol *sp_name_arg, t_floatarg x_step_arg, t_floatarg y_step_arg) {
   t_saci *x = (t_saci *) pd_new(saci_class);
   t_glist *glist=(t_glist *)canvas_getcurrent();
   x->glist = glist;
   x->sp_name = sp_name_arg;
   // if no name, we use the actual canvas
   if(strcmp(x->sp_name->s_name, "") != 0)
      glist = saci_find_parent(glist);
   t_canvas *canvas=(t_canvas*)glist_getcanvas(glist);
   x->x_canvas = canvas;
   x->x_step = (x_step_arg > 0)?x_step_arg:1;
   x->y_step = (y_step_arg > 0)?y_step_arg:1;
   x->mode = SACI_RANDOM;
   x->obj_count = 0;
   return (void *) x;
}

/* ----------------------------------------------------
   Destroy the class
 -----------------------------------------*/
void saci_destroy(t_saci *x) {
   (void) x;
//   post("You say good bye and I say hello");
}

/* ----------------------------------------------------
   Setup
 -----------------------------------------*/
void saci_setup(void) {
   saci_class = class_new(gensym("saci"),
      (t_newmethod) saci_new, // Constructor
      (t_method) saci_destroy, // Destructor
      sizeof (t_saci),
      CLASS_DEFAULT,
      A_DEFSYMBOL,
      A_DEFFLOAT,
      A_DEFFLOAT,
      0);//Must always ends with a zero

   class_addmethod(saci_class, (t_method) saci_bang, gensym("bang"), 0);
   class_addmethod(saci_class, (t_method) saci_set_target, gensym("set"), A_DEFSYMBOL, 0);
   class_addmethod(saci_class, (t_method) saci_set_mode, gensym("mode"), A_DEFSYMBOL, 0);

   class_addmethod(saci_class, (t_method) saci_set_all_objs, gensym("allobjs"), 0);
   class_addmethod(saci_class, (t_method) saci_set_objs, gensym("objs"), A_GIMME, 0);

   class_addmethod(saci_class, (t_method) saci_set_step, gensym("step"), A_DEFFLOAT, 0);
   class_addmethod(saci_class, (t_method) saci_set_x_step, gensym("xstep"), A_DEFFLOAT, 0);
   class_addmethod(saci_class, (t_method) saci_set_y_step, gensym("ystep"), A_DEFFLOAT, 0);
}
