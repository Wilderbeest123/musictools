#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "shapes.h"
#include "ui.h"
#include "jtime.h"

typedef struct
{
    v2 pos;
    v2 size;
    unsigned int tid;           /* ID of the texture to render */
    bool sel;
    ui_node_t n;

    int border;                 /* textbox border size */

    int idx;                    /* Index for the text buffer */
    char text[64];              /* The character buffer */
    gl_charset_t cset;

    jtime_t timer;
    bool l_show;

} textbox_t;

ui_node_t* textbox_init(v2 pos, v2 size, gl_charset_t cset, char *str);

void textbox_add(v2 pos, v2 size, char *str);


#endif  /* TEXTBOX_H_ */
