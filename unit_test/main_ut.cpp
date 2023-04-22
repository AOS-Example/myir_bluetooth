#include <gtest/gtest.h>
#include "TestSuite.h"
#include "btlog.h"

int main( int argc, char **argv ) {

    // Per https://www.eesewiki.ford.com/display/fnv/Gathering+and+Reporting+Code+Coverage
    fprintf( stderr, "NOTE:  If you see errors trying to create .gcda files\n" );
    fprintf( stderr, "\trun these commands before running this executable:\n" );
    fprintf( stderr, "export GCOV_PREFIX=./coverage\n" );
    fprintf( stderr, "export GCOV_PREFIX_STRIP=11\n" );
    fprintf( stderr, "\n" );

    int opt;

    /* Second interation to setup remaining flags */
    while ( ( opt = getopt( argc, argv, "d" ) ) != -1 ) {
        switch ( opt ) {
            case 'd':
                btlog_init( BTLOG_REALTIME_TARGET, BTLOG_LEVEL_VERBOSE, NULL, 10000, 1 );
                break;

            default:
                break;
        }
    }

    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}