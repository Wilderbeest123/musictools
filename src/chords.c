#include "chords.h"
#include "button.h"
#include "keys.h"
#include "shapes.h"
#include "box.h"

static chord_system_t *gchords;

static void chords_button_pressed(void *data, uint8_t event)
{
    chord_system_t *this = (chord_system_t*)data;

    if(this->state == CHORD_STATE_IDLE) {
        printf("%s: IDLE -> PENDING\n", __FUNCTION__);
        this->state = CHORD_STATE_PENDING;
        return;
    }

    if(this->state == CHORD_STATE_PENDING) {
        printf("%s: PENDING -> IDLE\n", __FUNCTION__);
        this->state = CHORD_STATE_IDLE;
    }
}

void chords_init(input_t *in)
{
    chord_system_t *this = (chord_system_t*)malloc(sizeof(chord_system_t));
    button_t *button = (button_t*)malloc(sizeof(button_t));
    ui_node_t *nptr;

    this->state = CHORD_STATE_IDLE;
    this->in = in;
    gchords = this;

    nptr = button_init(V2(0,550),V2(50,50), gl_load_image("res/yoda.png"),
                       chords_button_pressed, this);

    uisys_append(nptr);
}

static void chords_state_pending(chord_system_t *this)
{
    if(this->in->ev & INEVENT_MIDI_DOWN) {
        printf("%s: PENDING -> TIMER\n", __FUNCTION__);
        timer_init(&this->timer, 250);
        this->state = CHORD_STATE_TIMER;
    }
}

static void chords_state_timer(chord_system_t *this)
{
    if(this->in->ev & INEVENT_MIDI_DOWN ||
       this->in->ev & INEVENT_MIDI_UP) {
        timer_reset(&this->timer);
        return;
    }

    if(timer_check(&this->timer)) {
        printf("%s: TIMER -> APPEND\n", __FUNCTION__);
        this->state = CHORD_STATE_APPEND;
    }
}

static ui_node_t* chord_ui_select(ui_node_t *node, v2 mpos);
static void chord_ui_draw(ui_node_t *node, v2 pos);

static ui_ops_t chord_ops = { .select=chord_ui_select,
                              .draw=chord_ui_draw,
                              .free=NULL };

static ui_node_t* chord_ui_select(ui_node_t *node, v2 mpos)
{
    return NULL;
}

static void chord_ui_draw(ui_node_t *n, v2 pos)
{
    chord_ui_t *this = CONTAINER_OF(n, chord_ui_t, n);
    gl_color_t color = COLOR_INIT(255, 255, 255, 255);
    keys_render_notes(pos, this->notes, this->ncount, this->is_flat, NULL);
}

static void chords_ui_init(void)
{
    chord_ui_t *c;
    ui_t *ui;
    ui_node_t *n;
    v2 size;
    int i;

    c = (chord_ui_t*)malloc(sizeof(chord_ui_t));
    c->ncount = keys_get_notes(c->notes, 20);
    c->nmask = keys_get_note_mask(c->notes, c->ncount);
    c->rnote = keys_get_root_by_thirds(c->nmask);
    c->is_flat = keys_sharp_or_flat(c->rnote, c->nmask);
    c->n.ops = &chord_ops;
    c->n.next = NULL;

    printf("ncount: %d nmask: %d rnote: %d notes: ",
           c->ncount, c->nmask, c->rnote);

    for(i=0; i<c->ncount; i++)
    {
        printf("%hhu", c->notes[i]);
    }

    printf("\n");


    size = keys_render_size(c->notes, c->ncount, c->is_flat, NULL);

    ui = box_init(V2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
                  size, COLOR_INIT(255,255,255,255));


    //Insert the chord_ui_t as a child.
    ui_node_insert(&ui->h, &c->n);
    uisys_append(&ui->n);
}

static void chords_state_append(chord_system_t *this)
{
    chords_ui_init();
    printf("%s: APPEND -> IDLE\n", __FUNCTION__);
    this->state = CHORD_STATE_IDLE;
}

void chords_update(void)
{
    switch(gchords->state)
    {
    case CHORD_STATE_IDLE:
        return;

    case CHORD_STATE_PENDING:
        chords_state_pending(gchords);
        break;

    case CHORD_STATE_TIMER:
        chords_state_timer(gchords);
        break;

    case CHORD_STATE_APPEND:
        chords_state_append(gchords);
        break;

    default:
        printf("ERR: Should not reach here: %s", __FUNCTION__);
        break;
    }
}
