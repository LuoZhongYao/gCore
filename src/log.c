#include <utils/log.h>

#ifdef  LOG_ENABLED
void  logd_bytes(const void *data,const int length)
{
    int i = 0;
    const uint8 *_data = data;
    uprintf("[ ");
    for(;i < length;i++)
        uprintf("%02x ",_data[i]);
    uprintf("]\n");
}
#endif
