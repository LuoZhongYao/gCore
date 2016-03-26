#include <utils/file.h>
#include <utils/utils.h>

void create_file(FILE *stx,void *buffer,u16 n,void (*fn)(const void *data,u16 len))
{
    stx->buffer = buffer;
    stx->read_pos = 0;
    stx->write_pos = n;
    stx->length = n;
    stx->fn = fn;
}

void clear_file(FILE *stx)
{
    stx->read_pos = 0;
    stx->write_pos = 0;
}

int  putc(int c,FILE *stx)
{
    if(stx) {
        if(stx->fn) {
            stx->fn(&c,1);
            return c;
        } else if(stx->buffer && stx->write_pos < stx->length) {
            return stx->buffer[stx->write_pos++] = c;
        }
    }
    return EOF;
}

int getc(FILE *stx)
{
    if(stx && stx->read_pos < stx->write_pos)
        return stx->buffer[stx->read_pos++];
    return EOF;
}

int ungetc(int c,FILE *stx)
{
    if(stx && stx->read_pos)
        stx->read_pos--;
    return c;
}

