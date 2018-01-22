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
    uint level_size;
    uint total_size;
    uint times;
    uint crc;
    int level;
} loop_t;

typedef struct event {
    char label[128];
    uint iterations;
    ulong duration;
    int chunk_index;
    int loop_index;
    Agnode_t *node;
} event_t;

loop_t loops[2048];
event_t events[2048];
chunk_t chunks[2048];
Agraph_t *T;
GVC_t* gvc;

int loops_index = -1;
int chunks_index = -1;
unsigned long *samples;
unsigned int *sizes;
unsigned int *crcs;
unsigned int panic;

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
    *font_size = 18.0 + *node_size;
}

static int _graphviz_get_loop_index(uint crc, uint level, int *is_new)
{
    int i;

    for (i = 0; i < 2048; ++i)
    {
        //
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

static void _auxviz_sprintf_chunk(char *string, ulong sample, uint times, char last_char)
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

    // Label
    for (; i >= 0 && accum < 39; --i)
    {
        if (samples[i] != last)
        {
            _auxviz_sprintf_chunk(chunk->label, last, times, '|');
            last = samples[i];
            times = 1;
            accum++;
        } else {
            times++;
        }
    }
    if (accum < 39) {
        _auxviz_sprintf_chunk(chunk->label, last, times, '\0');
    } else {
        sprintf(chunk->label, "%s{ + %d }", chunk->label, elements - 39);
    }

    return index;
}

static int _graphviz_add_loop(uint crc, uint level, uint elements)
{
    ulong last;
    int index, times;
    int i, n, is_new;

    // Setting our space
    index = _graphviz_get_loop_index(crc, level, &is_new);

    if (is_new)
    {
        loops[index].level_size = elements;
        loops[index].total_size = 0;
        loops[index].level = level;
        loops[index].times = 1;
        loops[index].crc = crc;
    } else {
        loops[index].times += 1;
        printf("][loop: %d] ::: (%u, ", index, loops[index].total_size);
        return index;
    }

    // Content
    i = 0;
    times = 0;
    last = samples[i];

    for (; i < elements; ++i)
    {
        if (samples[i] != last)
        {
            loops[index].total_size += sizes[i] * times;
            printf("(%lu,%u,%d)", last, sizes[i], times);
            last = samples[i];
            times = 1;
        } else {
            times++;
        }
    }
    loops[index].total_size += sizes[i-1] * times;
    printf("(%lu,%u,%u)", last, sizes[i-1], times);
    printf("][loop: %d] ::: (%u, ", index, loops[index].total_size);

    return index;
}

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

static void _graphviz_create_edge(Agnode_t *node_tail, Agnode_t *node_head, Agraph_t *graph)
{
    static uint creation = 0;
    Agedge_t *edge;
    char name[32];

    // Name
    sprintf(name, "%d", creation);

    // Create
    edge = agedge(graph, node_tail, node_head, name, TRUE);

    // Style
    agsafeset(edge, "style", "dotted", "");
    creation = creation + 1;
}

static Agnode_t *_graphviz_create_chunk_node(int index, Agraph_t *subgraph)
{
    static unsigned int creation = 0;
    char name[32];
    Agnode_t *node;
    chunk_t *chunk;

    // Name
    name[0] = '\0';
    sprintf(name, "c%u", creation);

    // Create
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
    event_t *event;
    loop_t *loop;
    float node_size, font_size;
    char *sh, *co, *la;

    // Name
    buffer[0] = '\0';
    sprintf(buffer, "e%d", index);

    // Create
    event = &events[index];
    loop = &loops[event->loop_index];
    event->node = agnode(subgraph, buffer, TRUE);

    // Style
    sh = _auxviz_get_shape(loop->level);
    co = _auxviz_get_color(loop->level);
    _auxviz_get_size(event->duration, &node_size, &font_size);

    printf("event: %d level: %d\n", index, loop->level);

    agset(event->node, "style", "filled");
    agset(event->node, "fillcolor", co);
    agset(event->node, "shape", sh);

    la = event->label;
    sprintf(la, "%u\n", index);
    sprintf(la, "%slevel size: %u\n", la, loop->level_size);
    sprintf(la, "%stotal size: %u\n", la, loop->total_size);
    sprintf(la, "%siterations: %u\n", la, event->iterations);
    sprintf(la, "%sduration: %lu us\n", la, event->duration);
    sprintf(la, "%snnum: %u", la, index);
    agset(event->node, "label", event->label);

    sprintf(buffer, "%0.2f", node_size);
    agset(event->node, "width", buffer);
    agset(event->node, "height", buffer);

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

static void _graphviz_time_step_two(int index, ulong duration, uint iterations)
{
    Agnode_t *event_node, *chunk_node;
    Agraph_t *subgraph;
    event_t *event;
    loop_t *loop;

    event = &events[index];
    loop = &loops[event->loop_index];
    event->iterations = iterations;
    event->duration = duration;

    subgraph = _graphviz_create_subgraph();
    event_node = _graphviz_create_event_node(index, subgraph);

    if (loop->level == 0)
    {
        chunk_node = _graphviz_create_chunk_node(event->chunk_index, subgraph);
        _graphviz_create_edge(chunk_node, event_node, subgraph);
    }
}

static int _graphviz_time_step_one(ulong value, uint level_size, uint level)
{
    int loop_index, chunk_index, event_index;
    uint crc;
    int n;

    // Print content
    printf("%u : %lu : %u [", level, value, level_size);

    // samples = array of loops data
    // sizes = array of loops sizes
    // crcs = array of loops crc codes
    n = dynais_loop_data(samples, sizes, crcs, level, level_size);
    crc = crcs[n - 1];

    //
    loop_index = _graphviz_add_loop(crc, level, n);
    chunk_index = _graphviz_add_chunk(crc, level, n);
    event_index = _graphviz_add_event(loop_index, chunk_index);

    // Print content
    printf("%u)", crc);
    printf("\n");

    return event_index;
}

void graphviz_add_timeline(ulong value, ulong time, uint size, uint level, int status)
{
    static int event_head, event_tail = -1;
    static uint iterations;
    static ulong duration;

    //if (status == NEW_LOOP) printf("NEW_LOOP %lu\n ", value);
    //else if (status == END_NEW_LOOP) printf("END_NEW_LOOP %lu\n", value);
    //else if (status == END_LOOP) printf("END_LOOP\n");

    if (status == END_LOOP || status == END_NEW_LOOP)
    {
        duration = time - duration;
        _graphviz_time_step_two(event_head, duration, iterations);

        if (event_tail != -1) {
            _graphviz_create_edge(events[event_tail].node, events[event_head].node, T);
        }

        event_tail = event_head;
    }
    if (status >= NEW_LOOP)
    {
        duration = time;
        event_head = _graphviz_time_step_one(value, size, level);
    }
}

void graphviz_init()
{
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
}

void graphviz_dispose(char *output_file)
{
    FILE *output;

    output = fopen(output_file, "w");
    agwrite(T, (void *) output);
    //agwrite(H, (void *) output);
    fclose(output);

    //agclose (H);
    agclose (T);
    gvFreeContext(gvc);
}

void usage()
{
    printf("Usage: dynais_input_file size_windows n_levels output_file\n");
    printf("- input_file: path to the input file (binary)\n");
    printf("- size_windows: size of each level window\n");
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

    // Memory
    samples = malloc(atoi(argv[2]) * sizeof(unsigned long));
    sizes = malloc(atoi(argv[2]) * sizeof(unsigned int));
    crcs = malloc(atoi(argv[2]) * sizeof(unsigned int));

    // Algorithms
    dynais_init(atoi(argv[2]), atoi(argv[3]));
    graphviz_init();

    // Reading
    /*while(fread(&value, 4, sizeof(unsigned long), input) > 0)
    {
        status = dynais(value[3], &size, &level);
        graphviz_add_timeline(value[3], 0, size, level, status);
    }*/
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