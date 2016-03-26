#include <stdlib.h>
#include <string.h>
#include <utils/utils.h>

u8 atou(char ch)
{
    if(ch >= '0' && ch <= '9')
        return ch - '0';
    else if(ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if(ch >= 'A' && ch <= 'F')
        return ch -'A' + 10;
    return 0;
}

void *memmem(void *haystack,u16 haystacklen,
             const void *needle,u16 needlelen)
{

    char *cur, *last;
    char *cl = (char *)haystack;
    const char *cs = (const char *)needle;

    /* we need something to compare */
    if (haystacklen == 0 || needlelen == 0)
        return NULL;

    if (haystacklen < needlelen)
        return NULL;

    /* "s" must be smaller or equal to "l" */
    if (haystacklen < needlelen)
        return NULL;

    /* special case where needlelen == 1 */
    if (needlelen == 1)
        return memchr(haystack, (int)*cs, haystacklen);

    /* the last position where its possible to find "s" in "l" */
    last = cl + haystacklen - needlelen;

    for (cur = cl; cur <= last; cur++)
        if (cur[0] == cs[0] && memcmp(cur, cs, needlelen) == 0)
            return cur;

    return NULL;
}

