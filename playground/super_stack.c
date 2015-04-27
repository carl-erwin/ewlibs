// $ gcc  -std=c99 -D_DEFAULT_SOURCE=1 super_stack.c -lpthread

#include <sys/time.h>
#include <sys/mman.h>

#include <signal.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


void recurse(size_t depth)
{
    fprintf(stderr, "recurse depth %lu\n", depth);

    if (depth > (3000000*2)) {
        //fprintf(stderr, "recurse paused @ depth %lu\n", depth);
        return;

        while (1)
          pause();
    }

    recurse(depth+1);

    fprintf(stderr, "recurse returned @ depth %lu\n", depth);

}

void * thread_function(void * arg)
{
    int i = 0;

    fprintf(stderr, "Hello from thread, addr i = %p\n", &i);

    recurse(1);

    pause();
    return NULL;
}


int main() {

    // create thread stack

    size_t stack_size = 1024 * 1024 * 1024 * 1; // 1 Gib
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS|MAP_PRIVATE;
    uint8_t * stack_base = (uint8_t *)mmap(NULL, stack_size, prot, flags, -1, 0);

    if (stack_base == MAP_FAILED) {
        fprintf(stderr, "mmap error %m\n");
        return -1;
    }

    fprintf(stderr, "statck_start %p\n", stack_base);
    fprintf(stderr, "statck_end   %p\n", stack_base + stack_size);

    sleep(3);

    pthread_attr_t th_attr;
    pthread_attr_init(&th_attr);
    pthread_attr_setstack(&th_attr, (void *)stack_base, stack_size);
    pthread_t th;

    int ret = pthread_create(&th, &th_attr, thread_function, NULL);
    if (ret != 0) {
        fprintf(stderr, "cannot start thread %m\n");
        return -1;
    }

/*
 *     for (size_t off = 0; off < stack_size; off++)
 *      stack_base[off] = 0xce;
 */

    pause();
    return 0;
}
