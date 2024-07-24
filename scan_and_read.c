#include <iio/iio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    struct iio_scan *scan = iio_scan(NULL, "usb=0456:b672");
    if (!scan) {
        printf("not found\n");
        return 1;
    }

    const char *uri = iio_scan_get_uri(scan, 0);
    if (!uri) {
        return 1;
    }

    struct iio_context *ctx = iio_create_context(NULL, uri);
    if (!ctx) {
        return 1;
    }

    const char *name = iio_context_get_name(ctx);
    if (!name) {
        return 1;
    }

    printf("%s\n\n", name);

    /* char *xml = iio_context_get_xml(ctx); */
    /* printf("%s\n", xml); */
    /* free(xml); */

    const struct iio_context_params *params;
    params = iio_context_get_params(ctx);

    int t = iio_context_get_devices_count(ctx);

    for (int i = 0; i < t; i++) {
        struct iio_device *dev = iio_context_get_device(ctx, i);
        if (!dev) {
            return 1;
        }
        char *name = iio_device_get_name(dev);
        char *lbl = iio_device_get_label(dev);
        int channels = iio_device_get_channels_count(dev);
        if (strcmp(name, "m2k-logic-analyzer") != 0) {
            continue;
        }

        int dev_attrs = iio_device_get_attrs_count(dev);
        printf("Dev name: %s\n", name);
        printf("\tAttrs: %d\n", dev_attrs);
        for (int j = 0; j < dev_attrs; j++) {
            struct iio_attr *dev_attr = iio_device_get_attr(dev, j);
            printf("\t\tAttr name: %s\n", //\t\tAttr val: %d",
                   iio_attr_get_name(dev_attr));
            if(strcmp(iio_attr_get_name(dev_attr), "sampling_frequency") != 0) {
                continue;
            }
            iio_attr_write_string(dev_attr, "100.0");
            /* double *attr_db; */
            printf("\t\tValue: %s\n", iio_attr_get_static_value(dev_attr));
            /* iio_attr_read_double(dev_attr, attr_db); */
            /* printf("\t\tValue: %f\n", *attr_db); */
        }

        printf("name: %s\nlabel: %s\nchannels: %d\n", name, lbl, channels);
        for (int j = 0; j < channels; j++) {
            struct iio_channel *ch = iio_device_get_channel(dev, j);
            int ch_attrs = iio_channel_get_attrs_count(ch);
            printf("\tAttrs: %d\n", ch_attrs);
            for (int k = 0; k < ch_attrs; k++) {
                struct iio_attr *ch_attr = iio_channel_get_attr(ch, k);
                printf("\t\tAttr name: %s\n", //\t\tAttr val: %d",
                       iio_attr_get_name(ch_attr));
            }
        }
    }

    return 0;
}
