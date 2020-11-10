/* testandset.S - test_and_set (for x86) */

        .text
	    .globl test_and_set

/*------------------------------------------------------------------------
 * testandset -  X86 test_and_set atomic instruction call is test_and_set(&ptr,new_value);
 *------------------------------------------------------------------------
uint32 test_and_set(uint32 *ptr, uint32 new_value) {
    uint32 old_value = *ptr;
    *ptr = new_value;
    return old_value;
}
*/

test_and_set:
        pushl	%ebp		     /* Push ebp onto stack		    */
		movl	%esp,%ebp	     /* Record current SP in ebp	*/
        pushl   %edx             /* save value of reg           */

        movl    8(%ebp),%edx     /* record &ptr in edx */
        movl    12(%ebp),%eax    /* record new_value in eax */
        xchg    (%edx),%eax      /* atomic swap of *ptr and new_value */
     
        popl    %edx              /* restore value of reg */
        popl    %ebp             /* Restore the caller base pointer */
        ret                      /* return to caller */
        




