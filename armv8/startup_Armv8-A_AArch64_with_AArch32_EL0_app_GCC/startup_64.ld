/* Linker script to place sections and symbol values.
 * It references following symbols, which must be defined in code:
 *   start64 : Entry point
 * 
 * It defines following symbols, which code can use without definition:
 *   __cs3_peripherals
 *   __code_start
 *   __el3_stack
 *   __ttb0_l1
 *   __ttb0_l2_ram
 *   __ttb0_l2_private
 *   __ttb0_l2_periph
 *   __top_of_ram
 */

ENTRY(start64)

SECTIONS
{
    /*
     * CS3 Peripherals is a 64MB region from 0x1c000000
     * that includes the following:
     * System Registers             at 0x1C010000
     * UART0 (PL011)                at 0x1C090000
     * Color LCD Controller (PL111) at 0x1C1F0000
     * plus a number of others.
     * CS3_PERIPHERALS is used by the startup code for page-table generation
     * This region is not truly empty, but we have no
     * predefined objects that live within it
     */
    __cs3_peripherals  = 0x1c000000;

    /*
     * GICv3 distributor
     */
    .gicd 0x2f000000 (NOLOAD):
    {
        *(.gicd)
    }

    /*
     * GICv3 redistributors
     * 128KB for each redistributor in the system
     */
    .gicr 0x2f100000 (NOLOAD):
    {
        *(.gicr)
    }

    .vectors 0x80000000:
    {
        __code_start = .;
        KEEP(*(StartUp))
        KEEP(*(EL1VECTORS EL2VECTORS EL3VECTORS))
    }

    .text :
    {
        *(.text*)
    }

    .el3_stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + 8 * 0x1000;
        __el3_stack = .;
    }

    .el1_stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + 8 * 0x1000;
        __el1_stack = .;
    }

    .ttb0_l1 (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l1 = .;
        . = . + 0x1000;
    }

    .ttb0_l2_ram (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l2_ram = .;
        . = . + 0x1000;
    }

    .ttb0_l2_private (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l2_private = .;
        . = . + 0x1000;
    }

    .ttb0_l2_periph (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l2_periph = .;
        . = . + 0x1000;
    }

    /*
     * Allow extra space for (large) EL1 apps.  Adjust the size to suit your app.
     * If size of EL3+EL1 apps exceeds 2MB, this ensures an MMU table entry will be set up for it.
     */
    .extra_space_for_el1_app (NOLOAD):
    {
        . = ALIGN(4096);
        __extra_space_for_el1_app = .;
        . = . + 0x500000;
    }

    /*
     * The startup code uses the end of this region to calculate
     * the top of memory - don't place any RAM regions after it
     */
    __top_of_ram = .;
}
