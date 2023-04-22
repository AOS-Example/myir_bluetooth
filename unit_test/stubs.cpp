#include <stdio.h>
#include <errno.h>
#include "btresmgr.h"
#include "priv/bt_priv.h"
#include <btgattsrv.h>
#include "stubs.h"

/* Mock up external api
 */

int btgattsrv_register_table_mock( btres_t *res, int fd, uint8_t *table, size_t len ) {
    return 0;
}

int close_mock ( int __fd ) {
    return 0;
}

ssize_t write_mock( int __fd, const void *__buf, size_t __n ) {
    return __n;
}

ssize_t send_mock( int __fd, const void *__buf, size_t __n, int __flags ) {
    return __n;
}