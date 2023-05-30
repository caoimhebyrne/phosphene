// Make sure the linker puts this at the start of the kernel image
.section ".text.boot"

// Execution starts here
.global _start

_start:
    // Store the processor ID in `x0`.
    mrs     x0, mpidr_el1
    and     x0, x0, #3

    // If the processor id is 0 (cbz), go to `entry`.
    cbz     x0, entry

    // Otherwise, halt the processor indefinately.
halt:  
    wfe
    b       halt

entry:  
    // We are on the main processor.

    // Set stack to start below our code.
    ldr     x0, =_start
    mov     sp, x0

    // Clean the BSS section.
    // This is where our uninitialized variables are stored.
clear_bss:
    // Store the start of the BSS in `x0`.
    ldr     x0, =__bss_start

    // Store the end/size of the BSS in `w2`.
    ldr     w2, =__bss_size

1:
    // If the end/size is zero, quit the loop.
    cbz     w2, run_init
    str     xzr, [x0], #8
    sub     w2, w2, #1

    // Continue the loop if `w2` is non-zero.
    cbnz    w2, 1b

run_init:
    // Jump to our init() function
    bl      init

    // If it does return, halt the master core too
    b       halt
