/*
 * start.S
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

.macro save_regs
	sub	sp, sp, #68
	stmia sp, {r0-r12}
	ldr	r0, [sp, #72]
	str	r0, [sp, #60]
	ldr	r0, [sp, #76]
	mov	r3, r0
	orr	r0, r0, #0xc0
	msr	cpsr_c, r0
	mov	r1, sp
	mov	r2, lr
	cps #0x13
	str	r1, [sp, #52]
	str	r2, [sp, #56]
	str	r3, [sp, #64]
	mov r0, sp
.endm

.macro restore_regs
	ldr	r1, [sp, #52]
	ldr	r2, [sp, #56]
	ldr	r0, [sp, #64]
	orr	r0, r0, #0xc0
	msr	cpsr_c, r0
	mov	sp, r1
	mov	lr, r2
	cps #0x13
	ldr	r0, [sp, #60]
	str	r0, [sp, #72]
	ldmia sp, {r0-r12}
	add	sp, #68
.endm

/*
 * Exception vector table
 */
.text
	.arm

	.global	_start
_start:
	/* Boot head information for BROM */
	.long 0xea000016
	.byte 'e', 'G', 'O', 'N', '.', 'B', 'T', '0'
	.long 0, 0x8000
	.byte 'S', 'P', 'L', 2
	.long 0, 0
	.long 0, 0, 0, 0, 0, 0, 0, 0
	.long 0, 0, 0, 0, 0, 0, 0, 0

_vector:
	b reset
	ldr pc, _undefined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq

/*
 * The actual reset code
 */
reset:
	/* Save boot params to 0x00000040 */
	ldr	r0, =0x00000040
	str	sp, [r0, #0]
	str	lr, [r0, #4]
	mrs	lr, cpsr
	str	lr, [r0, #8]
	mrc	p15, 0, lr, c1, c0, 0
	str	lr, [r0, #12]
	mrc	p15, 0, lr, c12, c0, 0
	str	lr, [r0, #16]
	mrc	p15, 0, lr, c1, c0, 0
	str	lr, [r0, #20]

	/* Enter svc mode cleanly and mask interrupts */
	mrs r0, cpsr
	eor r0, r0, #0x1a
	tst r0, #0x1f
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	bne 1f
	orr r0, r0, #0x100
	adr lr, 2f
	msr spsr_cxsf, r0
	.word 0xe12ef30e /* msr elr_hyp, lr */
	.word 0xe160006e /* eret */
1:	msr cpsr_c, r0
2:	nop

	/* Set vector base address register */
	ldr r0, =_vector
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1<<13)
	mcr p15, 0, r0, c1, c0, 0

	/* Enable neon/vfp unit */
	mrc p15, 0, r0, c1, c0, 2
	orr r0, r0, #(0xf << 20)
	mcr p15, 0, r0, c1, c0, 2
	isb
	mov r0, #0x40000000
	vmsr fpexc, r0

	/* Enable non-secure access to all peripherals */
	ldr r0, =0x01c23408
	ldr r1, =0xbe
	str r1, [r0]
	ldr r0, =0x01c23414
	ldr r1, =0xff
	str r1, [r0]
	ldr r0, =0x01c23420
	ldr r1, =0x7f
	str r1, [r0]

	/* Copyself to link address */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq 1f
	bl brom_copyself
1:	nop

	/* Initialize stacks */
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x1b
	msr cpsr_cxsf, r1
	ldr sp, _stack_und_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x17
	msr cpsr_cxsf, r1
	ldr sp, _stack_abt_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x12
	msr cpsr_cxsf, r1
	ldr sp, _stack_irq_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x11
	msr cpsr_cxsf, r1
	ldr sp, _stack_fiq_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x13
	msr cpsr_cxsf, r1
	ldr sp, _stack_srv_end

	/* Copy data section */
	ldr r0, _data_start
	ldr r3, _image_start
	ldr r1, _data_shadow_start
	sub r1, r1, r3
	adr r3, _start
	add r1, r1 ,r3
	ldr r2, _data_shadow_start
	ldr r3, _data_shadow_end
	sub r2, r3, r2
	bl memcpy

	/* Clear bss section */
	ldr r0, _bss_start
	ldr r2, _bss_end
	sub r2, r2, r0
	mov r1, #0
	bl memset

	/* Call _main */
	ldr r1, =_main
	mov pc, r1
_main:
	mov r0, #1;
	mov r1, #0;
	bl xboot_main
	b _main

	.global return_to_fel
return_to_fel:
	ldr	r0, =0x00000040
	ldr	sp, [r0, #0]
	ldr	lr, [r0, #4]
	ldr	r1, [r0, #20]
	mcr	p15, 0, r1, c1, c0, 0
	ldr	r1, [r0, #16]
	mcr	p15, 0, r1, c12, c0, 0
	ldr	r1, [r0, #12]
	mcr	p15, 0, r1, c1, c0, 0
	ldr	r1, [r0, #8]
	msr	cpsr, r1
	bx	lr

/*
 * Exception handlers
 */
	.align 5
undefined_instruction:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_undefined_instruction
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
software_interrupt:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_software_interrupt
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
prefetch_abort:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_prefetch_abort
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
data_abort:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_data_abort
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
not_used:
	b .

	.align 5
irq:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_irq
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
fiq:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_fiq
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

/*
 * The location of section
 */
 	.align 4
_image_start:
	.long __image_start
_image_end:
	.long __image_end
_data_shadow_start:
	.long __data_shadow_start
_data_shadow_end:
	.long __data_shadow_end
_data_start:
	.long __data_start
_data_end:
	.long __data_end
_bss_start:
	.long __bss_start
_bss_end:
	.long __bss_end
_stack_und_end:
	.long __stack_und_end
_stack_abt_end:
	.long __stack_abt_end
_stack_irq_end:
	.long __stack_irq_end
_stack_fiq_end:
	.long __stack_fiq_end
_stack_srv_end:
	.long __stack_srv_end
