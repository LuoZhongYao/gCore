#include <utils/scanf.h>
int
__scanf(FILE *stream, const char *format, ...)
{
    va_list ap;
    int retval;

    va_start(ap, format);

    retval = _doscan(stream, format, ap);

    va_end(ap);

    return retval;
}

