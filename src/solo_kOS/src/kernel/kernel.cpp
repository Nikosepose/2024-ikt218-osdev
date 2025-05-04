
extern "C"{
    #include "kernel/interrupts.h"
    #include "libc/stdio.h"
    #include "libc/system.h"
    #include "common/io.h"
    #include "common/input.h"
    #include "kernel/interrupts.h"
    #include "kernel/interrupt_functions.h"

}



extern "C" int kernel_main(void);
int kernel_main(){

    printf("Hello from kernel_main!\n");
    init_interrupt_functions(); // Initialize interrupt functions


    return 0;
}