ENTRY(Reset_Handler)

MEMORY
{
	FLASH(rx): ORIGIN =0x08000000, LENGTH =32K
	SRAM(rwx): ORIGIN =0x20000000, LENGTH =8K
}


/*__max_heap_size = 0x400*/				/* symbol declaration. Not a variable */
/*__max_stack_size = 0x200*/			/* symbol declaration. Not a variable */

SECTIONS
{
	  .isr_vector :
	  {
	    . = ALIGN(4);
	    KEEP(*(.isr_vector)) /* Startup code */
	    . = ALIGN(4);
	  } >FLASH
  
	.text :
	{
		. = ALIGN(4);
		*(.text)				/* .text of main.o led.o startup.o */
		*(.text*)          /* .text* sections (code) */
		*(.glue_7)         /* glue arm to thumb code */
	    *(.glue_7t)        /* glue thumb to arm code */
	    *(.eh_frame)
	
	    KEEP (*(.init))
	    KEEP (*(.fini))
    
		*(.rodata)
		. = ALIGN(4);
		_etext = .;		/* Store the updated location counter value into a symbol '_etext' */
	} > FLASH AT> FLASH			/*or just "> FLASH"    // vma AT> lma */
 
	_la_data = LOADADDR(.data);
	.data :						/*initialized data*/
	{
		. = ALIGN(4);
		_sdata = .;			/* Assign a value to a symbol '_sdata' ; start of VMA(!) of this section */
		*(.data)
		*(.data*)          /* .data* sections */
    	*(.RamFunc)        /* .RamFunc sections */
    	*(.RamFunc*)       /* .RamFunc* sections */
    
		. = ALIGN(4);
		_edata = .;			/* Location counter always tracks VMA of the section in which it is being used */
	} > SRAM AT> FLASH

	. = ALIGN(4);
	.bss :						/*uninitialized data*/
	{
		_sbss = .;
		*(.bss)
		*(.bss*)
    	*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} > SRAM
	 
}
 