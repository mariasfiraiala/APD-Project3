#include <stdio.h>

#include "debug.h"
#include "utils.h"

void print_file(struct file_t *file, FILE *out)
{
    fprintf(out, "%d\n", file->owner);
    fprintf(out, "%s %d\n", file->name, file->nr_segments);
    for (int i = 0; i < file->nr_segments; ++i)
        fprintf(out, "%s\n", file->segments[i]);
}

void print_swarm(struct swarm_t *s, FILE *out)
{
    fprintf(out, "Capacity: %d\nSize: %d\n", s->capacity, s->size);
    for (int i = 0; i < s->size; ++i)
        print_file(&s->all_files[i], out);
}
