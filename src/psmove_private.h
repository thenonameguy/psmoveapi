/**
 * PS Move API - An interface for the PS Move Motion Controller
 * Copyright (c) 2012 Thomas Perl <m@thp.io>
 * Copyright (c) 2012 Benjamin Venditti <benjamin.venditti@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#ifndef PSMOVE_PRIVATE_H
#define PSMOVE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psmove.h"

#include <stdio.h>
#include <wchar.h>

    /**
     * PRIVATE DEFINITIONS FOR USE IN psmove.c AND psmove_*.c
     *
     * These constants are considered implementation details and should
     * not be used by external code (they are subject to change).
     *
     * All constants that need to be shared between psmove.c and other
     * implementation modules (psmove_*.c) should go here.
     **/

/* Vendor ID and Product ID of PS Move Controller */
#define PSMOVE_VID 0x054c
#define PSMOVE_PID 0x03d5

#define psmove_PRINTF(section, msg, ...) \
        fprintf(stderr, "[" section "] " msg, ## __VA_ARGS__)

/* Macro: Debugging output */
#ifdef PSMOVE_DEBUG
#    define psmove_DEBUG(msg, ...) \
            psmove_PRINTF("PSMOVE DEBUG", msg, ## __VA_ARGS__)
#else
#    define psmove_DEBUG(msg, ...)
#endif

/* Macro: Warning message */
#define psmove_WARNING(msg, ...) \
        psmove_PRINTF("PSMOVE WARNING", msg, ## __VA_ARGS__)

/* Macro: Print a critical message if an assertion fails */
#define psmove_CRITICAL(x) \
        psmove_PRINTF("PSMOVE CRITICAL", \
                "Assertion fail in %s: %s\n", \
                __func__, x)

/* Macro: Deprecated functions */
#define psmove_DEPRECATED(x) \
        psmove_PRINTF("PSMOVE DEPRECATED", \
                "%s is deprecated: %s\n", \
                __func__, x)

/* Macros: Return immediately if an assertion fails + log */
#define psmove_return_if_fail(expr) \
        {if(!(expr)){psmove_CRITICAL(#expr);return;}}
#define psmove_return_val_if_fail(expr, val) \
        {if(!(expr)){psmove_CRITICAL(#expr);return(val);}}

/* Macro: Length of fixed-size array */
#define ARRAY_LENGTH(x) (sizeof(x)/sizeof((x)[0]))

/* RGB value struct used in the tracker */
struct PSMove_RGBValue {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

/* Buffer size for calibration data */
#define PSMOVE_CALIBRATION_SIZE 49

/* Three blocks, minus 2x the header (2 bytes) for the 2nd and 3rd block */
#define PSMOVE_CALIBRATION_BLOB_SIZE (PSMOVE_CALIBRATION_SIZE*3 - 2*2)


/**
 * [PRIVATE API] Write raw data blob to device
 **/
ADDAPI void
ADDCALL _psmove_write_data(PSMove *move, unsigned char *data, int length);


/**
 * [PRIVATE API] Read raw data blob from device
 **/
ADDAPI void
ADDCALL _psmove_read_data(PSMove *move, unsigned char *data, int length);

/**
 * [PRIVATE API] Disable the connection to remote servers
 *
 * This can only be called at the beginning, and will disable connections to
 * any remote "moved" servers.
 **/
ADDAPI void
ADDCALL _psmove_disable_remote();

/**
 * [PRIVATE API] Disable local (hidapi-based) controllers
 *
 * This can only be called at the beginning, and will disable all local
 * controllers that are connected via hidapi.
 **/
ADDAPI void
ADDCALL _psmove_disable_local();

/**
 * [PRIVATE API] Internal device open function (hidraw, Linux / for moved)
 **/
ADDAPI PSMove *
ADDCALL psmove_connect_internal(wchar_t *serial, char *path, int id);

/**
 * [PRIVATE API] Get device path of a controller (hidraw, Linux / for moved)
 **/
ADDAPI const char *
ADDCALL _psmove_get_device_path(PSMove *move);

/**
 * [PRIVATE API] Get the calibration data from a connected USB controller.
 *
 * The pointer *dest will be set to a newly-allocated byte array
 * of a certain size (which will be saved in *size) and the caller
 * has to free this field with free()
 **/
ADDAPI int
ADDCALL _psmove_get_calibration_blob(PSMove *move, char **dest, size_t *size);

/* A Bluetooth address. */
typedef unsigned char PSMove_Data_BTAddr[6];

/**
 * Read a Bluetooth address from string and write its
 * internal representation into a PSMove_Data_BTAddr.
 *
 * If dest is NULL, the data is not written (only verified).
 *
 * Will return nonzero on success, zero on error.
 **/
ADDAPI int
ADDCALL _psmove_btaddr_from_string(const char *string, PSMove_Data_BTAddr *dest);


/**
 * Formats the contents of addr to a newly-allocated string and
 * returns it. The caller has to free() the return value.
 **/
ADDAPI char *
ADDCALL _psmove_btaddr_to_string(const PSMove_Data_BTAddr addr);

/**
 * Normalize a Bluetooth address given a specific format
 *
 * lowercase ... Make all characters lowercase if nonzero
 * separator ... The separator character (usually ':' or '-')
 *
 * The return value must be free()d by the caller.
 **/
ADDAPI char *
ADDCALL _psmove_normalize_btaddr(const char *addr, int lowercase, char separator);


/**
 * Read the current Bluetooth addresses stored in the controller
 *
 * This only works via USB.
 *
 * If host is not NULL, the current host address will be stored there.
 * If controller is not NULL, the controller address will be stored there.
 **/
ADDAPI int
ADDCALL _psmove_read_btaddrs(PSMove *move, PSMove_Data_BTAddr *host, PSMove_Data_BTAddr *controller);




#ifdef __cplusplus
}
#endif

#endif
