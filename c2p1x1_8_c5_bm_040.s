	.section .text
	.text

| out-of-line, rarely-needed clamping code
	.balign 4

C2P1X1_8_C5_BM_040_DELTA1:	.space	4
C2P1X1_8_C5_BM_040_DELTA2:	.space	4
C2P1X1_8_C5_BM_040_DELTA3:	.space	4
C2P1X1_8_C5_BM_040_DELTA4:	.space	4
C2P1X1_8_C5_BM_040_DELTA5:	.space	4
C2P1X1_8_C5_BM_040_DELTA6:  .space	4
C2P1X1_8_C5_BM_040_DELTA7:	.space  4
C2P1X1_8_C5_BM_040_DELTA8:	.space	4
C2P1X1_8_C5_BM_040_CHUNKYX: .space	2
C2P1X1_8_C5_BM_040_CHUNKYY: .space	2
C2P1X1_8_C5_BM_040_ROWMOD:  .space	4
C2P1X1_8_C5_BM_040_SIZEOF:	

BitMap:
bm_BytesPerRow:    	.space 2        
bm_Rows:    		.space 2        
bm_Flags:    		.space 1        
bm_Depth:    		.space 1       
bm_Pad:    			.space 2       
bm_Planes:    		.space 32      
bm_SIZEOF:


	.globl	_c2p1x1_8_c5_bm_040
	.globl	c2p1x1_8_c5_bm_040
_c2p1x1_8_c5_bm_040:
c2p1x1_8_c5_bm_040:
	movem.l	d2-d7/a2-a6,-(sp)
	sub.l	#C2P1X1_8_C5_BM_040_SIZEOF,sp

	cmpi.b	#8,bm_Depth(a1)		
	blo	.exit
	move.w	d0,d4
	move.w	d2,d5
	andi.w	#0x1f,d4			
	bne	.exit
	andi.w	#0x7,d5	
	bne	.exit
	moveq	#0,d4
	move.w	bm_BytesPerRow(a1),d4

	move.w	d0,C2P1X1_8_C5_BM_040_CHUNKYX(sp) 
	beq	.exit
	move.w	d1,C2P1X1_8_C5_BM_040_CHUNKYY(sp)
	beq	.exit

	and.l	#0xffff,d0
	and.l	#0xffff,d2

	mulu.w	d4,d3		
	lsr.l	#3,d2
	add.l	d2,d3

	lsl.l	#3,d4			
	sub.l	d0,d4
	bmi	.exit

	lsr.l	#3,d4
	move.l	d4,C2P1X1_8_C5_BM_040_ROWMOD(sp) 

	move.l	a0,a2			
	add.w	C2P1X1_8_C5_BM_040_CHUNKYX(sp),a2

	move.l	bm_Planes+1*4(a1),a3
	sub.l	bm_Planes+5*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA1(sp)
	move.l	bm_Planes+4*4(a1),a3
	sub.l	bm_Planes+1*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA2(sp)
	move.l	bm_Planes+0*4(a1),a3
	sub.l	bm_Planes+4*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA3(sp)
	move.l	bm_Planes+7*4(a1),a3
	sub.l	bm_Planes+0*4(a1),a3
	addq.l	#4,a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA4(sp)
	move.l	bm_Planes+3*4(a1),a3
	sub.l	bm_Planes+7*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA5(sp)
	move.l	bm_Planes+6*4(a1),a3
	sub.l	bm_Planes+3*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA6(sp)
	move.l	bm_Planes+2*4(a1),a3
	sub.l	bm_Planes+6*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA7(sp)
	move.l	bm_Planes+5*4(a1),a3
	sub.l	bm_Planes+2*4(a1),a3
	move.l	a3,C2P1X1_8_C5_BM_040_DELTA8(sp)

	move.l	bm_Planes+7*4(a1),a1
	add.l	d3,a1


	tst.b	32(a0)
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	move.l	(a0)+,d2
	move.l	(a0)+,d3
	tst.b	32(a0)
	move.l	(a0)+,d4
	move.l	(a0)+,d5
	move.l	(a0)+,a5
	move.l	(a0)+,a6

	swap	d4		
	swap	d5
	eor.w	d0,d4
	eor.w	d1,d5
	eor.w	d4,d0
	eor.w	d5,d1
	eor.w	d0,d4
	eor.w	d1,d5
	swap	d4
	swap	d5

	move.l	d4,d6
	move.l	d5,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	#0x33333333,d6
	and.l	#0x33333333,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d4
	eor.l	d7,d5

	exg	d4,a5
	exg	d5,a6

	swap	d4			
	swap	d5
	eor.w	d2,d4
	eor.w	d3,d5
	eor.w	d4,d2
	eor.w	d5,d3
	eor.w	d2,d4
	eor.w	d3,d5
	swap	d4
	swap	d5

	move.l	d4,d6			
	move.l	d5,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d2,d6
	eor.l	d3,d7
	and.l	#0x33333333,d6
	and.l	#0x33333333,d7
	eor.l	d6,d2
	eor.l	d7,d3
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d4
	eor.l	d7,d5

	move.l	d1,d6		
	move.l	d3,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	eor.l	d0,d6
	eor.l	d2,d7
	and.l	#0x0f0f0f0f,d6
	and.l	#0x0f0f0f0f,d7
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#4,d6
	lsl.l	#4,d7
	eor.l	d6,d1
	eor.l	d7,d3

	move.l	d2,d6			
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	#0x00ff00ff,d6
	and.l	#0x00ff00ff,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#8,d6
	lsl.l	#8,d7
	eor.l	d6,d2
	eor.l	d7,d3

	bra	.xstart

.y:
.x:
	tst.b	32(a0)
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	move.l	(a0)+,d2
	move.l	(a0)+,d3
	tst.b	32(a0)
	move.l	(a0)+,d4
	move.l	(a0)+,d5
	move.l	(a0)+,a5
	move.l	(a0)+,a6

	move.l	d6,(a1)

	swap	d4			
	swap	d5
	eor.w	d0,d4
	eor.w	d1,d5
	eor.w	d4,d0
	eor.w	d5,d1
	eor.w	d0,d4
	add.l	C2P1X1_8_C5_BM_040_DELTA1(sp),a1
	eor.w	d1,d5
	swap	d4
	swap	d5

	move.l	d4,d6		
	move.l	d7,(a1)
	move.l	d5,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	#0x33333333,d6
	and.l	#0x33333333,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d4
	eor.l	d7,d5

	exg	d4,a5
	add.l	C2P1X1_8_C5_BM_040_DELTA2(sp),a1
	exg	d5,a6

	swap	d4			
	swap	d5
	eor.w	d2,d4
	eor.w	d3,d5
	eor.w	d4,d2
	eor.w	d5,d3
	eor.w	d2,d4
	eor.w	d3,d5
	swap	d4
	swap	d5

	move.l	a3,(a1)
	move.l	d4,d6			
	move.l	d5,d7
	lsr.l	#2,d6
	lsr.l	#2,d7
	eor.l	d2,d6
	eor.l	d3,d7
	and.l	#0x33333333,d6
	and.l	#0x033333333,d7
	eor.l	d6,d2
	eor.l	d7,d3
	lsl.l	#2,d6
	lsl.l	#2,d7
	eor.l	d6,d4
	eor.l	d7,d5

	move.l	d1,d6		
	move.l	d3,d7
	lsr.l	#4,d6
	lsr.l	#4,d7
	eor.l	d0,d6
	eor.l	d2,d7
	and.l	#0x0f0f0f0f,d6
	and.l	#0x0f0f0f0f,d7
	add.l	C2P1X1_8_C5_BM_040_DELTA3(sp),a1
	eor.l	d6,d0
	eor.l	d7,d2
	lsl.l	#4,d6
	lsl.l	#4,d7
	eor.l	d6,d1
	move.l	a4,(a1)
	eor.l	d7,d3

	move.l	d2,d6			
	move.l	d3,d7
	lsr.l	#8,d6
	lsr.l	#8,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	#0x00ff00ff,d6
	and.l	#0x00ff00ff,d7
	eor.l	d6,d0
	eor.l	d7,d1
	lsl.l	#8,d6
	lsl.l	#8,d7
	add.l	C2P1X1_8_C5_BM_040_DELTA4(sp),a1
	eor.l	d6,d2
	eor.l	d7,d3


	cmp.l	a0,a2
	bhs.s	.xstart

	add.w	C2P1X1_8_C5_BM_040_CHUNKYX(sp),a2
						 
	add.l	C2P1X1_8_C5_BM_040_ROWMOD(sp),a1  
						  

.xstart:

	move.l	d2,d6		
	move.l	d3,d7
	lsr.l	#1,d6
	lsr.l	#1,d7
	eor.l	d0,d6
	eor.l	d1,d7
	and.l	#0x55555555,d6
	and.l	#0x55555555,d7
	eor.l	d6,d0
	eor.l	d7,d1
	move.l	d0,(a1)
	add.l	d6,d6
	add.l	d7,d7
	eor.l	d6,d2
	eor.l	d7,d3

	move.l	a5,d6
	move.l	a6,d7
	move.l	d2,a3
	move.l	d3,a4

	move.l	d5,d2			
	move.l	d7,d3
	lsr.l	#4,d2
	lsr.l	#4,d3
	add.l	C2P1X1_8_C5_BM_040_DELTA5(sp),a1
	eor.l	d4,d2
	eor.l	d6,d3
	and.l	#0x0f0f0f0f,d2
	and.l	#0x0f0f0f0f,d3
	eor.l	d2,d4
	move.l	d1,(a1)
	eor.l	d3,d6

	lsl.l	#4,d2
	lsl.l	#4,d3
	eor.l	d2,d5
	eor.l	d3,d7

	move.l	d4,d2			
	move.l	d5,d3
	lsr.l	#8,d2
	lsr.l	#8,d3
	add.l	C2P1X1_8_C5_BM_040_DELTA6(sp),a1
	eor.l	d6,d2
	eor.l	d7,d3
	and.l	#0x00ff00ff,d2
	and.l	#0x00ff00ff,d3
	eor.l	d2,d6
	move.l	a3,(a1)
	eor.l	d3,d7

	lsl.l	#8,d2
	lsl.l	#8,d3
	eor.l	d2,d4
	eor.l	d3,d5

	move.l	d4,d2			
	move.l	d5,d3
	add.l	C2P1X1_8_C5_BM_040_DELTA7(sp),a1
	lsr.l	#1,d2
	lsr.l	#1,d3
	eor.l	d6,d2
	eor.l	d7,d3
	and.l	#0x55555555,d2
	move.l	a4,(a1)
	and.l	#0x55555555,d3

	eor.l	d2,d6
	eor.l	d3,d7
	add.l	d2,d2
	add.l	d3,d3
	add.l	C2P1X1_8_C5_BM_040_DELTA8(sp),a1
	eor.l	d2,d4
	eor.l	d3,d5

	move.l	d4,a3
	move.l	d5,a4

	cmp.l	a0,a2
	bne	.x

	subq.w	#1,C2P1X1_8_C5_BM_040_CHUNKYY(sp)
	bne	.y

	move.l	d6,(a1)
	add.l	C2P1X1_8_C5_BM_040_DELTA1(sp),a1
	move.l	d7,(a1)
	add.l	C2P1X1_8_C5_BM_040_DELTA2(sp),a1
	move.l	a3,(a1)
	add.l	C2P1X1_8_C5_BM_040_DELTA3(sp),a1
	move.l	a4,(a1)

.exit:
	add.l	#C2P1X1_8_C5_BM_040_SIZEOF,sp
	movem.l	(sp)+,d2-d7/a2-a6
.earlyexit:
	rts

