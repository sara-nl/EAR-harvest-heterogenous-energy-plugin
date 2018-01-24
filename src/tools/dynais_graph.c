#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string_enhanced.h>
#include <ear_dynais/ear_dynais.h>
#include <config.h>
#include <cgraph.h>
#include <gvc.h>

// gcc -D ANALYSIS -I ../../src/library/ -c ../../src/library/ear_dynais/ear_dynais.c -msse4.2
// gcc -I ../../src/common/ -c ../../src/common/string_enhanced.c
// gcc -D ANALYSIS -I /usr/include/graphviz -I ../../src/library -I ../../src/common/ -o dynais_graph dynais_graph.c ear_dynais.o string_enhanced.o -lgvc -lcgraph -lcdt
// ./dynais_input app.dynais.trace.bin 500 5
// scp xjaneas@172.30.7.32:app.dynais.trace.bin .

typedef struct chunk {
    char label[4096];
    uint crc;
} chunk_t;

typedef struct loop {
    int level;
    ulong input;
    uint length;
    uint weight;
    uint times;
    uint crc;
} loop_t;

typedef struct event {
    char label[128];
    uint iterations;
    int chunk_index;
    int loop_index;
    Agnode_t *node;
    ulong duration;
    ulong inputs;
} event_t;

loop_t loops[2048];
event_t events[2048];
chunk_t chunks[2048];
Agraph_t *T;
GVC_t* gvc;

ulong last_time = 0;
int last_status = 0;
int event_head = -1;
int event_tail = -1;
int loops_index = -1;
int chunks_index = -1;
unsigned int *iterations;
unsigned long *samples;
unsigned int *sizes;
unsigned int *crcs;
unsigned int panic;

// Color list depending on the level. To switch
// between colores, edit the names of the following list:
// · https://www.graphviz.org/doc/info/colors.html
static char *_auxviz_get_color(uint index)
{
    switch (index) {
        case 0: return "gray93";
        case 1: return "antiquewhite";
        case 2: return "red";
        case 3: return "green";
        default: return "gray93";
    }
}

// List of shapes depending on the level. To switch
// between shapes, edit the names of the following list:
// · https://www.graphviz.org/doc/info/shapes.html#polygon
static char *_auxviz_get_shape(uint index)
{
    switch (index) {
        case 0: return "circle";
        case 1: return "diamond";
        case 2: return "square";
        case 3: return "hexagon";
        default: return "circle";
    }
}

// Gets the node and its font size.
static void _auxviz_get_size(ulong sample, float *node_size, float *font_size)
{
    *node_size = 1.0;
    *font_size = 1.0;

    if (sample < 1000)
    {
        *node_size = 1.0;
    }
    else if (sample >= 1000 && sample < 10000)
    {
        *node_size = (((double) sample) / 10000) * (2.0 - 1.0) + 1.0;
    }
    else if (sample >= 10000 && sample < 100000)
    {
        *node_size = (((double) sample) / 100000) * (4.0 - 2.0) + 2.0;
    }
    else if (sample >= 100000 && sample < 1000000)
    {
        *node_size = (((double) sample) / 1000000) * (8.0 - 4.0) + 4.0;
    }
    else if (sample >= 1000000 && sample < 10000000)
    {
        *node_size = (((double) sample) / 10000000) * (16.0 - 8.0) + 8.0;
    }
    else if (sample >= 10000000 && sample < 100000000)
    {
        *node_size = (((double) sample) / 100000000) * (32.0 - 16.0) + 16.0;
    }
    else if (sample >= 100000000 && sample < 1000000000)
    {
        *node_size = (((double) sample) / 1000000000) * (64.0 - 32.0) + 32.0;
    }
    else if (sample >= 1000000000)
    {
        *node_size = 72.0;
    }
    *font_size = 24.0 + *node_size;
}

// Searches in the array of loops the one with the provided
// CRC checksum and level. Returns the index and a bit
// indicating if was not found and therefore added in the tail.
static int _graphviz_get_loop_index(uint crc, uint level, int *is_new)
{
    int i;

    for (i = 0; i < 2048; ++i)
    {
        if (i > loops_index)
        {
            loops_index += 1;
            *is_new = 1;
            return i;
        }
        else if (loops[i].crc == crc && loops[i].level == level)
        {
            *is_new = 0;
            return i;
        }
    }

    printf("ERROR: No space");
    exit(0);
}

// Searches in the array of chunks (level 0 loops) the one
// with the provided CRC checksum. Returns the index and a bit
// indicating if was not found and therefore added in the tail.
static int _graphviz_get_chunk_index(uint crc, int *is_new)
{
    int i;

    for (i = 0; i < 2048; ++i)
    {
        if (i > chunks_index)
        {
            chunks_index += 1;
            *is_new = 1;
            return i;
        }
        else if (chunks[i].crc == crc)
        {
            *is_new = 0;
            return i;
        }
    }

    printf("ERROR: No space");
    exit(0);
}

// Appends a record node.
static void _auxviz_append_record(char *string, ulong sample, uint times, char last_char)
{
    ulong number = sample;
    int digits = 20;

    sprintf(string, "%s{", string);
    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        sprintf(string, "%s_", string);
        digits--;
    }
    sprintf(string, "%s%lu|%u}%c", string, sample, times, last_char);
}

// Adds and initializes a chunk (level 0 loop) to the chunk
// array. It returns an index of the position of the chunk in
// the array. If already exists, it is not appended.
static int _graphviz_add_chunk(uint crc, uint level, uint elements)
{
    int index, times, accum;
    int i, n, is_new;
    chunk_t *chunk;
    ulong last;

    // Not 0, then there is not interest, no chunk
    if (level != 0) {
        return -1;
    }

    // Setting our space
    index = _graphviz_get_chunk_index(crc, &is_new);
    chunk = &chunks[index];

    if (is_new)
    {
        chunk->label[0] = '\0';
        chunk->crc = crc;
    } else {
        return index;
    }

    // Content
    i = elements - 1;
    times = accum = 0;
    last = samples[i];

    // Merges the same level 0 inputs and counts the times
    // appeared. This reduces the string length of the label.
    for (; i >= 0 && accum < 39; --i)
    {
        if (samples[i] != last)
        {
            _auxviz_append_record(chunk->label, last, times, '|');
            last = samples[i];
            times = 1;
            accum++;
        } else {
            times++;
        }
    }
    // Adds the last one to the label.
    if (accum < 39) {
        _auxviz_append_record(chunk->label, last, times, '\0');
    } else {
        sprintf(chunk->label, "%s{ + %d }", chunk->label, elements - 39);
    }

    return index;
}

// Adds and initializes a loop to the loop array. It returns
// an index of the position of the loop in the array. If
// already exists, it is not appended.
static int _graphviz_add_loop(ulong input, uint crc, uint level, uint elements)
{
    ulong previous_sample;
    uint previous_size, times;
    int index, is_new;
    int i, n;

    // Setting our space
    index = _graphviz_get_loop_index(crc, level, &is_new);

    // If its not new, the loop counter is increased.
    if (!is_new)
    {
        loops[index].times += 1;
        return index;
    }

    // If it is not new, the loop is initialized.
    loops[index].input = input;
    loops[index].length = elements;
    loops[index].weight = 0;
    loops[index].level = level;
    loops[index].times = 1;
    loops[index].crc = crc;

    // It accumulates the same inputs counting the times appeared.
    i = times = 0;
    previous_sample = samples[i];
    previous_size = sizes[i];

    for (; i < elements; ++i)
    {
        if (samples[i] != previous_sample)
        {
            loops[index].weight += previous_size * times;
            //printf("(%lu,%u,%d)", previous_sample, previous_size, times);
            previous_sample = samples[i];
            previous_size = sizes[i];
            times = 1;
        } else {
            times++;
        }
    }

    loops[index].weight += previous_size * times;
    //printf("(%lu,%u,%u)", previous_sample, previous_size, times);
    //printf("] ::: (loop id: %d, loop weight: %u) ", index, loops[index].weight);

    return index;
}

// Adds and initializes an event to the event array. It returns
// an index of the position of the event in the array. All events
// are different.
static int _graphviz_add_event(int loop_index, int chunk_index)
{
    static uint creation = 0;
    event_t *event;

    event = &events[creation];
    event->loop_index = loop_index;
    event->chunk_index = chunk_index;

    creation = creation + 1;
    return creation - 1;
}

// Creates a new edge between tail and head nodes in the provided graph.
static void _graphviz_create_edge(Agnode_t *node_tail, Agnode_t *node_head, Agraph_t *graph)
{
    static uint creation = 0;
    Agedge_t *edge;
    char name[32];

    // Name
    sprintf(name, "%d", creation);

    // Creation
    edge = agedge(graph, node_tail, node_head, name, TRUE);

    // Style
    agsafeset(edge, "style", "dotted", "");
    creation = creation + 1;
}

// Creates a chunk node, which is in the form of a record
// and just appended in the level 0 nodes.
static Agnode_t *_graphviz_create_chunk_node(int index, Agraph_t *subgraph)
{
    static unsigned int creation = 0;
    char name[32];
    Agnode_t *node;
    chunk_t *chunk;

    // Name
    name[0] = '\0';
    sprintf(name, "c%u", creation);

    // Creation
    chunk = &chunks[index];
    node = agnode(subgraph, name, TRUE);

    // Style
    agset(node, "style", "filled");
    agset(node, "shape", "record");
    agset(node, "fillcolor", "white");
    agset(node, "label", chunk->label);

    creation = creation + 1;
    return node;
}

static Agnode_t *_graphviz_create_event_node(int index, Agraph_t *subgraph)
{
    char buffer[32];
    float node_size, font_size;
    char *sh, *co, *la;
    event_t *event;
    loop_t *loop;

    // Name
    buffer[0] = '\0';
    sprintf(buffer, "e%d", index);

    // Create
    event = &events[index];
    loop = &loops[event->loop_index];
    event->node = agnode(subgraph, buffer, TRUE);

    // Setting the shape and color
    sh = _auxviz_get_shape(loop->level);
    co = _auxviz_get_color(loop->level);
    _auxviz_get_size(event->duration, &node_size, &font_size);

    agset(event->node, "style", "filled");
    agset(event->node, "fillcolor", co);
    agset(event->node, "shape", sh);

    // Creating the label
    la = event->label;
    sprintf(la, "%u\n", event->loop_index);
    sprintf(la, "%slength: %u\n", la, loop->length);
    sprintf(la, "%sweight: %u\n", la, loop->weight);
    sprintf(la, "%siterations: %u\n", la, event->iterations);
    sprintf(la, "%sduration: %lu us\n", la, event->duration);
    sprintf(la, "%sinputs during loop: %lu\n", la, event->inputs);
    sprintf(la, "%snode id: %u", la, index);
    agset(event->node, "label", event->label);

    // Setting the node width and height
    sprintf(buffer, "%0.2f", node_size);
    agset(event->node, "width", buffer);
    agset(event->node, "height", buffer);

    // Setting the font size
    sprintf(buffer, "%0.2f", font_size);
    agset(event->node, "fontsize", buffer);

    return event->node;
}

static Agraph_t *_graphviz_create_subgraph()
{
    char buffer[32];
    static uint creation = 0;

    // Name
    buffer[0] = '\0';
    sprintf(buffer, "s%u", creation);

    // Creating graph
    Agraph_t *subgraph = agsubg(T, buffer, TRUE);

    // Attributes
    agattr(subgraph, AGRAPH, "rank", "same");
    agattr(subgraph, AGRAPH, "rankdir", "TB");

    creation = creation + 1;
    return subgraph;
}

static void _auxviz_print_header()
{
    // Print content
    set_spacing_digits(6);
    print_spacing_string("NID");

    set_spacing_digits(5);
    print_spacing_string("LVL");

    set_spacing_digits(24);
    print_spacing_string("INPUT");

    set_spacing_digits(14);
    print_spacing_string("CRC");

    set_spacing_digits(8);
    print_spacing_string("LID");

    set_spacing_digits(8);
    print_spacing_string("LENGTH");

    set_spacing_digits(8);
    print_spacing_string("WEIGHT");

    set_spacing_digits(6);
    print_spacing_string_align_left("ITERS", 4);

    set_spacing_digits(11);
    print_spacing_string_align_left("TIME (us)", 4);

    set_spacing_digits(10);
    print_spacing_string_align_left("INPUTS", 4);

    printf("\n");
}

static void _auxviz_print_event(int index)
{
    event_t *event;
    loop_t *loop;

    // Getting event and loop
    event = &events[index];
    loop = &loops[event->loop_index];

    // Print content
    set_spacing_digits(6);
    print_spacing_int(index);

    set_spacing_digits(5);
    print_spacing_int(loop->level);

    set_spacing_digits(24);
    print_spacing_ulong(loop->input);

    set_spacing_digits(14);
    print_spacing_uint(loop->crc);

    set_spacing_digits(8);
    print_spacing_uint(event->loop_index);

    set_spacing_digits(8);
    print_spacing_uint(loop->length);

    set_spacing_digits(8);
    if (loop->level > 0) {
        print_spacing_uint(loop->weight);
    } else {
        print_spacing_string("-");
    }

    set_spacing_digits(6);
    print_spacing_string_align_left(add_point_uint(event->iterations), 4);

    set_spacing_digits(11);
    print_spacing_string_align_left(add_point_ulong(event->duration), 4);

    set_spacing_digits(10);
    print_spacing_string_align_left(add_point_ulong(event->inputs), 4);

    printf("\n");
}

// The second step of the timeline. The resting data is
// set and the nodes are created.
static void _graphviz_time_step_two(int index, ulong duration, ulong inputs, uint iterations)
{
    Agnode_t *event_node, *chunk_node;
    Agraph_t *subgraph;
    event_t *event;
    loop_t *loop;

    // Getting event information
    event = &events[index];
    event->iterations = iterations;
    event->duration = duration;
    event->inputs = inputs;

    // Getting loop information
    loop = &loops[event->loop_index];

    //
    subgraph = _graphviz_create_subgraph();
    event_node = _graphviz_create_event_node(index, subgraph);

    // If the new loop level is not 0, no chunk is needed.
    if (loop->level == 0)
    {
        chunk_node = _graphviz_create_chunk_node(event->chunk_index, subgraph);
        _graphviz_create_edge(chunk_node, event_node, subgraph);
    }

    _auxviz_print_event(index);
}

// The first step. It initializes the loop data.
static int _graphviz_time_step_one(ulong input, uint length, uint level)
{
    int loop_index, chunk_index, event_index;
    uint crc;
    int n;

    // samples = array of loops data
    // sizes = array of loops sizes
    // crcs = array of loops crc codes
    n = dynais_loop_data(samples, sizes, crcs, level, length);
    crc = crcs[n - 1];

    // The first data is initialized.
    loop_index = _graphviz_add_loop(input, crc, level, n);
    chunk_index = _graphviz_add_chunk(crc, level, n);
    event_index = _graphviz_add_event(loop_index, chunk_index);

    return event_index;
}

void graphviz_add_timeline(ulong value, ulong time, uint size, uint level, int status)
{
    static ulong inputs, inputs_counter;
    static ulong iterations;
    static ulong duration;

    // Input counting
    inputs_counter += 1;

    // New iteration, then 1 is added to the counter.
    if (status == NEW_ITERATION) {
        iterations += 1;
    }

    // If is the end of a loop, the node is created an its
    // annexed content node in case the level of the node
    // is 0. Finally an edge is created between this newly
    // created node and the previous one.
    if (status == END_LOOP || status == END_NEW_LOOP)
    {
        inputs = inputs_counter - inputs;
        duration = time - duration;

        // The second step, the creation of the node.
        _graphviz_time_step_two(event_head, duration, inputs, iterations);

        if (event_tail != -1) {
            _graphviz_create_edge(events[event_tail].node, events[event_head].node, T);
        }

        event_tail = event_head;
    }

    // If is the beggining of a new loop, it's memory space
    // and indexes are initialized. This is the step one.
    if (status >= NEW_LOOP)
    {
        event_head = _graphviz_time_step_one(value, size, level);

        // Data initialization
        inputs = inputs_counter;
        duration = time;
        iterations = 3;
    }

    // This variables are taken out, because if the input
    // ends in a loop state, it has to be closed forcefully
    // to create the node, because it is created at the end
    // of a loop.
    last_status = status;
    last_time = time;
}

void graphviz_close_timeline()
{
    // Closes a loop if is not closed yet.
    if (last_status >= IN_LOOP) {
        graphviz_add_timeline(0, last_time, 0, 0, END_LOOP);
    }
}

void graphviz_init(unsigned int windows_size, unsigned int num_levels)
{
    // Memory allocation
    iterations = malloc(num_levels * sizeof(unsigned int));
    samples = malloc(windows_size * sizeof(unsigned long));
    sizes = malloc(windows_size * sizeof(unsigned int));
    crcs = malloc(windows_size * sizeof(unsigned int));

    // Graph
    gvc = gvContext();
    T = agopen("root", Agdirected, NULL);

    // Attributes
    agattr(T, AGRAPH, "ordering", "out");
    agattr(T, AGRAPH, "rankdir", "LR");
    agattr(T, AGNODE, "style", "filled");
    agattr(T, AGNODE, "shape", "circle");
    agattr(T, AGNODE, "fillcolor", "black");
    agattr(T, AGNODE, "height", "0.0");
    agattr(T, AGNODE, "width", "0.0");
    agattr(T, AGNODE, "fontsize", "14.0");

    // Printing the header
    _auxviz_print_header();
}

void graphviz_dispose(char *output_file)
{
    FILE *output;

    //
    graphviz_close_timeline();
    output = fopen(output_file, "w");
    agwrite(T, (void *) output);
    fclose(output);

    //
    agclose (T);
    gvFreeContext(gvc);
}

void usage()
{
    printf("Usage: dynais_input_file windows_length n_levels output_file\n");
    printf("- input_file: path to the input file (binary)\n");
    printf("- windows_length: size of each level window\n");
    printf("- n_levels: number of levels\n");
    printf("- output_file: path to the output file (graphviz)\n");
    exit(1);
}

int main(int argc, char **argv)
{
    FILE *input;
    unsigned int level, size;
    unsigned long value[5];
    int status;

    // Application
    if (argc != 5) {
        usage();
    }

    if ((input = fopen(argv[1], "r")) < 0) {
        perror("File read failed.");
        exit(1);
    }

    // Algorithms
    dynais_init(atoi(argv[2]), atoi(argv[3]));
    graphviz_init(atoi(argv[2]), atoi(argv[3]));

    // Reading
    while(!panic && fread(&value, 5, sizeof(unsigned long), input) > 0)
    {
        status = dynais(value[3], &size, &level);
        graphviz_add_timeline(value[3], value[4], size, level, status);
    }

    //
    fclose(input);

    // Algorithms
    graphviz_dispose(argv[4]);
    dynais_dispose();

    // Memory
    free(samples);
    free(sizes);
    free(crcs);

    return 0;
}