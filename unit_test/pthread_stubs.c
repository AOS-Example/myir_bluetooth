#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define EOK 0

bool pthread_create_fail = false;
bool run_start_routine = false;
int pthread_create_mock( pthread_t *__thr, const pthread_attr_t *__attr, void *( *__start_routine )( void * ), void *__arg ) {
    if ( pthread_create_fail ) {
        return -1;
    }

    if ( __thr != NULL ) {
        // QNX selft-thread id is 1. We need to be different here
        *__thr = 2;
    }

    if ( run_start_routine && __start_routine != NULL ) {
        __start_routine( __arg );
    }

    return EOK;
}
int pthread_cancel_mock( pthread_t __thr ) {
    return EOK;
}
int pthread_setname_np_mock( pthread_t __thr, const char *__name ) {
    return EOK;
}
int pthread_join_mock( pthread_t __thr, void **__value_ptr )  {
    return EOK;
}
int pthread_attr_init_mock( pthread_attr_t *__attr ) {
    return EOK;
}
int pthread_attr_setinheritsched_mock( pthread_attr_t *__attr, int __inheritsched ) {
    return EOK;
}
int pthread_attr_setdetachstate_mock( pthread_attr_t *__attr, int __detachstate ) {
    return EOK;
}
int pthread_attr_setschedpolicy_mock( pthread_attr_t *__attr, int __policy ) {
    return EOK;
}
int pthread_getschedparam_mock( const pthread_t __thr, int *__policy, struct sched_param *__param ) {
    return EOK;
}
int pthread_attr_setschedparam_mock( pthread_attr_t *__attr, const struct sched_param *__param ) {
    return EOK;
}
int pthread_cond_timedwait_mock( pthread_cond_t *__cond, pthread_mutex_t *__mutex, const struct timespec *__abstime ) {
    return EOK;
}
int pthread_cond_signal_mock( pthread_cond_t *__cond ) {
    return EOK;
}
int pthread_cond_wait_mock( pthread_cond_t *__cond, pthread_mutex_t *__mutex ) {
    return EOK;
};
int pthread_mutex_lock_mock( pthread_mutex_t *__mutex ) {
    return EOK;
}
int pthread_mutex_unlock_mock( pthread_mutex_t *__mutex ) {
    return EOK;
}