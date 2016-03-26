/*
 * doscan.c - scan formatted input
 */
/* $Header: doscan.c,v 1.9 91/03/26 18:41:47 ceriel Exp $ */

#include <ctype.h>
#include <utils/scanf.h>

#define set_pointer(flags)        /* compilation might continue */

/* Collect a number of characters which constitite an ordinal number.
 * When the type is 'i', the base can be 8, 10, or 16, depending on the
 * first 1 or 2 characters. This means that the base must be adjusted
 * according to the format of the number. At the end of the function, base
 * is then set to 0, so strtol() will get the right argument.
 */
#define FL_LJUST    0x0001      /* left-justify field */
#define FL_SIGN     0x0002      /* sign in signed conversions */
#define FL_SPACE    0x0004      /* space in signed conversions */
#define FL_ALT      0x0008      /* alternate form */
#define FL_ZEROFILL 0x0010      /* fill with zero's */
#define FL_SHORT    0x0020      /* optional h */
#define FL_LONG     0x0040      /* optional l */
#define FL_LONGDOUBLE   0x0080      /* optional L */
#define FL_WIDTHSPEC    0x0100      /* field width is specified */
#define FL_PRECSPEC 0x0200      /* precision is specified */
#define FL_SIGNEDCONV   0x0400      /* may contain a sign */
#define FL_NOASSIGN 0x0800      /* do not assign (in scanf) */
#define FL_NOMORE   0x1000      /* all flags collected */
#define NUMLEN      16


static char *
o_collect( s16 c,  FILE *stream, char type,
            u16 width, s16 *basep,char *inp_buf)
{
     char *bufp = inp_buf;
     s16 base;

    switch (type) {
    default :
    case 'i':    /* i means octal, decimal or hexadecimal */
    case 'p':
    case 'x':
    case 'X':    base = 16;    break;
    case 'd':
    case 'u':    base = 10;    break;
    case 'o':    base = 8;    break;
    case 'b':    base = 2;    break;
    }

    if (c == '-' || c == '+') {
        *bufp++ = c;
        if (--width)
            c = getc(stream);
    }

    if (width && c == '0' && base == 16) {
        *bufp++ = c;
        if (--width)
            c = getc(stream);
        if (c != 'x' && c != 'X') {
            if (type == 'i') base = 8;
        }
        else if (width) {
            *bufp++ = c;
            if (--width)
                c = getc(stream);
        }
    }
    else if (type == 'i') base = 10;

    while (width) {
        if (((base == 10) && isdigit(c))
            || ((base == 16) && isxdigit(c))
            || ((base == 8) && isdigit(c) && (c < '8'))
            || ((base == 2) && isdigit(c) && (c < '2'))) {
            *bufp++ = c;
            if (--width)
                c = getc(stream);
        }
        else break;
    }

    if (width && c != EOF) ungetc(c, stream);
    if (type == 'i') base = 0;
    *basep = base;
    *bufp = '\0';
    return bufp - 1;
}

/*
 * the routine that does the scanning 
 */

int
_doscan( FILE *stream, const char *format, va_list ap)
{
    s16  done = 0;     /* number of items done */
    s16  nrchars = 0;  /* number of characters read */
    s16  conv = 0;     /* # of conversions */
    s16  base;         /* conversion base */
    u32  val;          /* an integer value */
    char *str = 0;     /* temporary pointer */
    u16  width = 0;    /* width of field */
    s16  flags;        /* some flags */
    s16  kind;
    s16  ic = EOF;     /* the input character */
    char inp_buf[12];   

    if (!*format) 
        return 0;

    while (1) {
        if (isspace(*format)) {
            while (isspace(*format))
                format++;    /* skip whitespace */
            ic = getc(stream);
            nrchars++;
            while (isspace (ic)) {
                ic = getc(stream);
                nrchars++;
            }
            if (ic != EOF) ungetc(ic,stream);
            nrchars--;
        }
        if (!*format) 
            break;    /* end of format */

        if (*format != '%') {
            ic = getc(stream);
            nrchars++;
            if ((char)ic != *format++) 
                break;    /* error */
            continue;
        }
        format++;
        if (*format == '%') {
            ic = getc(stream);
            nrchars++;
            if (ic == '%') {
                format++;
                continue;
            }
            else break;
        }
        flags = 0;
        if (*format == '*') {
            format++;
            flags |= FL_NOASSIGN;
        }
        if (isdigit (*format)) {
            flags |= FL_WIDTHSPEC;
            for (width = 0; isdigit (*format);)
                width = width * 10 + *format++ - '0';
        }

        switch (*format) {
        case 'h': flags |= FL_SHORT; format++; break;
        case 'l': flags |= FL_LONG; format++; break;
        case 'L': flags |= FL_LONGDOUBLE; format++; break;
        }
        kind = *format;
        if ((kind != 'c') && (kind != '[') && (kind != 'n')) {
            do {
                ic = getc(stream);
                nrchars++;
            } while (isspace(ic));
            if (ic == EOF) break;        /* outer while */
        } else if (kind != 'n') {        /* %c or %[ */
            ic = getc(stream);
            if (ic == EOF) break;        /* outer while */
            nrchars++;
        }
        switch (kind) {
        default:
            /* not recognized, like %q */
            return conv || (ic != EOF) ? done : EOF;
            break;
        case 'n':
            if (!(flags & FL_NOASSIGN)) {    /* silly, though */
                if(flags & FL_LONG)
                    *va_arg(ap, s32 *) = (s32) nrchars;
                else
                    *va_arg(ap, s16 *) = (s16) nrchars;
            }
            break;
        case 'p':        /* pointer */
            set_pointer(flags);
            /* fallthrough */
        case 'b':        /* binary */
        case 'd':        /* decimal */
        case 'i':        /* general integer */
        case 'o':        /* octal */
        case 'u':        /* unsigned */
        case 'x':        /* hexadecimal */
        case 'X':        /* ditto */
            if (!(flags & FL_WIDTHSPEC) || width > NUMLEN)
                width = NUMLEN;
            if (!width) return done;

            str = o_collect(ic, stream, kind, width, &base,inp_buf);
            if (str < inp_buf
                || (str == inp_buf
                    && (*str == '-'
                    || *str == '+'))) return done;

            /*
             * Although the length of the number is str-inp_buf+1
             * we don't add the 1 since we counted it already
             */
            nrchars += str - inp_buf;

            if (!(flags & FL_NOASSIGN)) {
                if (kind == 'd' || kind == 'i')
                    val = strtol(inp_buf, 0, base);
                else
                    val = strtoul(inp_buf,0, base);
                if(flags & FL_LONG)
                    *va_arg(ap, u32 *) = (u32) val;
                else
                    *va_arg(ap, u16 *) = (u16) val;
            }
            break;
        case 'c':
            if (!(flags & FL_WIDTHSPEC))
                width = 1;
            if (!(flags & FL_NOASSIGN))
                str = va_arg(ap, char *);
            if (!width) return done;

            while (width && ic != EOF) {
                if (!(flags & FL_NOASSIGN))
                    *str++ = (char) ic;
                if (--width) {
                    ic = getc(stream);
                    nrchars++;
                }
            }

            if (width) {
                if (ic != EOF) ungetc(ic,stream);
                nrchars--;
            }
            break;
        case 's':
            if (!(flags & FL_WIDTHSPEC))
                width = 0xffff;
            if (!(flags & FL_NOASSIGN))
                str = va_arg(ap, char *);
            if (!width) return done;

            while (width && ic != EOF) {
                if (!(flags & FL_NOASSIGN))
                    *str++ = (char) ic;
                if (--width) {
                    ic = getc(stream);
                    nrchars++;
                }
            }
            /* terminate the string */
            if (!(flags & FL_NOASSIGN))
                *str = '\0';    
            if (width) {
                if (ic != EOF) ungetc(ic,stream);
                nrchars--;
            }
            break;
        }        /* end switch */
        conv++;
        if (!(flags & FL_NOASSIGN) && kind != 'n') done++;
        format++;
    }
    return conv || (ic != EOF) ? done : EOF;
}
