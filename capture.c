/* This code focuses on capturing/reading the buffer of the adalm2k
 * Author: SgoSkz
 * Email: <will put something later>
 */

#include <iio/iio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    struct iio_scan *scan = iio_scan(NULL, "usb");
    int scan_cnt = iio_scan_get_results_count(scan);
    printf("Found %d usb devices\n", scan_cnt);

    if (scan_cnt == 0) {
        return 1;
    }

    char *uri;
    for (int i = 0; i < scan_cnt; i++) {
        uri = (char *)iio_scan_get_uri(scan, i);
        printf("%s\n", uri);
    }

    struct iio_context *ctx = iio_create_context(NULL, uri);
    struct iio_device *dev;
    int dev_cnt = iio_context_get_devices_count(ctx);
    char *name;
    int id;
    for (int i = 0; i < dev_cnt; i++) {
        dev = iio_context_get_device(ctx, i);
        name = (char *)iio_device_get_name(dev);
        printf("Dev name: %s\n\
\tTrigger: %d\n\
\tHWmon: %d\n\
\tAttrs: %d\n\
\tChannels: %d\n",
               name, iio_device_is_trigger(dev), iio_device_is_hwmon(dev),
               iio_device_get_attrs_count(dev),
               iio_device_get_channels_count(dev));
        if(strcmp(name, "m2k-logic-analyzer-tx") == 0) {
            free(name);
            break;
        }
        free(name);
    }

    int ch_cnt = iio_device_get_channels_count(dev);
    struct iio_channel *ch;

    struct iio_channels_mask *mask;
    mask = iio_create_channels_mask(1);

    for (int i = 0; i < ch_cnt; i++) {
        ch = iio_device_get_channel(dev, i);
        name = (char *)iio_channel_get_id(ch);
        printf("Ch name: %s\n\tScan: %d\n",
               name,
               iio_channel_is_scan_element(ch));
        iio_channel_enable(ch, mask);
        free(name);
    }

    struct iio_buffer *buf;
    buf = iio_device_create_buffer(dev, 0, mask);

    struct iio_block *blk;
    blk = iio_buffer_create_block(buf, 4096);

    /* for(void *ptr = iio_block_first(blk, 0); */
    /*     ptr < iio_block_end(blk); */
    /*     ptr += iio_block_step(blk)) { */
    /* } */

    return 0;
}
