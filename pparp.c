#include "m_pd.h"  
 
static t_class *pparp_class;  
 
typedef struct _pparp {  
    t_object  x_obj; 
    t_int i_note_out;
    t_int i_note_num;
    t_int i_note_vel;
    t_int i_voices[4];
    t_int i_oct_arp_shift;
    t_int i_oct_arp_index;
} t_pparp;  

static t_int octave_arp(t_pparp *x) {
    t_int i = 0;
    t_int note = 0;

    // play notes in the allocated voices array, so sequence plays notes in the order they were pressed
    i = 0;
    do {
        x->i_oct_arp_index++;
        x->i_oct_arp_index &= 3;
        if (!x->i_oct_arp_index) x->i_oct_arp_shift++;
        i++;
    }
    while (!x->i_voices[x->i_oct_arp_index] && (i < 4));
    if (x->i_voices[x->i_oct_arp_index]) {
        note = x->i_voices[x->i_oct_arp_index]  + (12 * (x->i_oct_arp_shift & 0x1));  // do the octave shift
    }   
    return note;
}

static void allocate_voices(t_pparp *x){
    int j, already_playing = 0;
    
    t_int num = x->i_note_num;
    t_int vel = x->i_note_vel;

    // if the key is up
    if (vel == 0){
        for (j=0; j < 4; j++){
            if (x->i_voices[j] == num) x->i_voices[j] = 0;
        }
    }
    // if its down
    else {
        // see that its not already playing
        already_playing = 0;
        for (j=0; j < 4; j++) {
            if (x->i_voices[j] == num) already_playing = 1;
        }
        // if its not, try to allocate
        if (!already_playing){
            for (j=0; j < 4; j++) {
                if (x->i_voices[j] == 0) {
                    x->i_voices[j] = num;  
                    break;
                }
            }
        }
    }
}

void pparp_bang(t_pparp *x) {
    
    t_int note = octave_arp(x);
    if (note) {
        outlet_float(x->x_obj.ob_outlet, note);
    }
    
/*    t_int a, b, c, d;
    a = x->i_voices[0];
    b = x->i_voices[1];
    c = x->i_voices[2];
    d = x->i_voices[3];
    post("%d, %d, %d, %d", a, b, c, d);*/
}

void pparp_notein(t_pparp *x, t_floatarg num) {  
    x->i_note_num = (t_int)num;
    allocate_voices(x); 
}  

void pparp_velin(t_pparp *x, t_floatarg vel) {  
    x->i_note_vel = (t_int)vel;
}

void *pparp_new(void) {  
    t_pparp *x = (t_pparp *)pd_new(pparp_class);  
    
    outlet_new(&x->x_obj, &s_float);

    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("notein"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("velin"));
    
    return (void *)x;  
}  

void pparp_setup(void) {  
    pparp_class = class_new(gensym("pparp"),  
        (t_newmethod)pparp_new,  
        0, sizeof(t_pparp),  
        CLASS_DEFAULT, 0);  
    class_addbang(pparp_class, pparp_bang);  
    class_addmethod(pparp_class,  
        (t_method)pparp_notein, gensym("notein"),  
        A_DEFFLOAT, 0);  
    class_addmethod(pparp_class,  
        (t_method)pparp_velin, gensym("velin"),  
        A_DEFFLOAT, 0);  
}
