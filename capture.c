/* This code focuses on capturing/reading the buffer of the adalm2k
 * Author: SgoSkz
 * Email: <will put something later>
 */

#include <iio/iio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

ssize_t sample_cb(const struct iio_channel *ch, void *src, size_t bytes, void *data);

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

    iio_scan_destroy(scan);

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
            /* free(name); */
            break;
        }
        /* free(name); */
    }
    /* free(dev); */


    int ch_cnt = iio_device_get_channels_count(dev);
    struct iio_channel *ch;

    struct iio_channels_mask *mask;
    mask = iio_create_channels_mask(16);

    for (int i = 0; i < ch_cnt; i++) {
        ch = iio_device_get_channel(dev, i);
        name = (char *)iio_channel_get_id(ch);
        printf("Ch name: %s\n\tScan: %d\n",
               name,
               iio_channel_is_scan_element(ch));
        iio_channel_enable(ch, mask);
        /* free(name); */
    }


    struct iio_buffer *buf;
    buf = iio_device_create_buffer(dev, 0, mask);

    struct iio_block *blk;
    int size = iio_device_get_sample_size(dev, mask);
    blk = iio_buffer_create_block(buf, size);

    iio_buffer_enable(buf);

    /* NOTE: 4 blocks, 2 samples */
    struct iio_stream *str = iio_buffer_create_stream(buf, 4, 2);
    blk = (struct iio_block *)iio_stream_get_next_block(str);

    iio_block_foreach_sample(blk, mask, sample_cb, NULL);
    iio_buffer_disable(buf);

    /* iio_stream_destroy(str); */
    iio_block_destroy(blk);
    iio_buffer_destroy(buf);
    iio_channels_mask_destroy(mask);
    iio_context_destroy(ctx);
    /* NOTE: API does not allow for checking if iio_context has been properly
     * shutdown/destroyed */
    /* if(ctx) { */
    /*     printf("\nCTX exists %d\n", iio_context_get_devices_count(ctx)); */
    /*     return 0; */
    /* } */
    return 0;
}

bool has_repeat = false;

ssize_t sample_cb(const struct iio_channel *chn, void *src, size_t bytes, __notused void *d)
{
    /* void *dst; */
    /* iio_channel_convert(chn, dst, src); */
	const struct iio_data_format *fmt = iio_channel_get_data_format(chn);
    /* printf("%s\n", (char *)dst); */
	unsigned int j, repeat = has_repeat ? fmt->repeat : 1;

    /* iio_channel_convert(chn, d, src); */

	printf("%s ", iio_channel_get_id(chn));
	for (j = 0; j < repeat; ++j) {
		if (bytes == sizeof(int16_t))
			printf("%" PRIi16 " \n", ((int16_t *)src)[j]);
		else if (bytes == sizeof(int64_t))
			printf("%" PRIi64 " \n", ((int64_t *)src)[j]);
	}

	return bytes * repeat;
}
