/* drivers/input/mouse/psmouse-base.c: */

/* List of supported PS/2 mouse protocols */
static struct psmouse_protocol psmouse_protocols[] = {
    {
        .type = PSMOUSE_PS2, /* The bare PS/2 handler */
        .name = "PS/2",
        .alias = "bare",
        .maxproto = 1,
        .detect = ps2bare_detect,
    },
    /* ... */
    {
        .type = PSMOUSE_SYNAPTICS,  /* Synaptics TouchPad Protocol */
        .name = "SynPS/2",
        .alias = "synaptics",
        .detect = synaptics_detect, /* Is the protocol detected? */
        .init = synaptics_init,     /* Initialize Protocol Handler */
    },
    /* ... */
}

/* drivers/input/mouse/psmouse.h: */

/* The structure that ties various mouse protocols together */
struct psmouse {
    struct input_dev *dev; /* The input device */

    /* ... */

    /* Protocol Methods */
    psmouse_ret_t (*protocol_handler)
        (struct psmouse *psmouse, struct pt_regs *regs);
    void (*set_rate)(struct psmouse *psmouse, unsigned int rate);
    void (*set_resolution)
        (struct psmouse *psmouse, unsigned int resolution);
    int (*reconnect)(struct psmouse *psmouse);
    void (*disconnect)(struct psmouse *psmouse);
    /* ... */
};

/* drivers/input/mouse/synaptics.c:*/
/* init() method of the Synaptics protocol */
int synaptics_init(struct psmouse *psmouse)
{
    struct synaptics_data *priv;
    psmouse->private = priv = kmalloc(sizeof(struct synaptics_data),
                                      GFP_KERNEL);
    /* ... */
    /* This is called in interrupt context when mouse
       movement is sensed */
    psmouse->protocol_handler = synaptics_process_byte;
    /* More protocol methods */
    psmouse->set_rate = synaptics_set_rate;
    psmouse->disconnect = synaptics_disconnect;
    psmouse->reconnect = synaptics_reconnect;
    /* ... */
}

/* drivers/input/mouse/synaptics.c: */
/* If you unfold synaptics_process_byte() and look at
   synaptics_process_packet(), you can see the input
   events being reported to user applications via mousedev */
static void synaptics_process_packet(struct psmouse *psmouse)
{
    /* ... */
    if (hw.z > 0) {
        /* Absolute X Coordinate */
        input_report_abs(dev, ABS_X, hw.x);
        /* Absolute Y Coordinate */
        input_report_abs(dev, ABS_Y,
                         YMAX_NOMINAL + YMIN_NOMINAL - hw.y);
    }

    /* Absolute Z Coordinate */
    input_report_abs(dev, ABS_PRESSURE, hw.z);
    /* ... */

    /* Left TouchPad button */
    input_report_key(dev, BTN_LEFT, hw.left);

    /* Right TouchPad button */
    input_report_key(dev, BTN_RIGHT, hw.right);

    /* ... */
} 
