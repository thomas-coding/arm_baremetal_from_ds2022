#define ESR_EL1_EC_SHIFT	(26)
#define ESR_EL1_EC_SVC32	(0x11)
#define ESR_EL1_EC_SVC64	(0x15)
#define ESR_EL1_EC_SVC_NUM_MASK (0xffff)
	
	.global syncFirstLevelHandler
	.global irqFirstLevelHandler
	.global irqHandler
	.global svcHandler


   .section InterruptHandlers, "ax"
    .balign 4

    .type irqFirstLevelHandler, "function"
irqFirstLevelHandler:
  STP      x29, x30, [sp, #-16]!
  STP      x18, x19, [sp, #-16]!
  STP      x16, x17, [sp, #-16]!
  STP      x14, x15, [sp, #-16]!
  STP      x12, x13, [sp, #-16]!
  STP      x10, x11, [sp, #-16]!
  STP      x8, x9, [sp, #-16]!
  STP      x6, x7, [sp, #-16]!
  STP      x4, x5, [sp, #-16]!
  STP      x2, x3, [sp, #-16]!
  STP      x0, x1, [sp, #-16]!

  BL       irqHandler

  LDP      x0, x1, [sp], #16
  LDP      x2, x3, [sp], #16
  LDP      x4, x5, [sp], #16
  LDP      x6, x7, [sp], #16
  LDP      x8, x9, [sp], #16
  LDP      x10, x11, [sp], #16
  LDP      x12, x13, [sp], #16
  LDP      x14, x15, [sp], #16
  LDP      x16, x17, [sp], #16
  LDP      x18, x19, [sp], #16
  LDP      x29, x30, [sp], #16
  ERET

//this example only supports SVC exception handling from AArch32 EL0  

  .section syncHandlers, "ax"
    .balign 4

    .type syncFirstLevelHandler, "function"
syncFirstLevelHandler:
  STP      x29, x30, [sp, #-16]!
  STP      x18, x19, [sp, #-16]!
  STP      x16, x17, [sp, #-16]!
  STP      x14, x15, [sp, #-16]!
  STP      x12, x13, [sp, #-16]!
  STP      x10, x11, [sp, #-16]!
  STP      x8, x9, [sp, #-16]!
  STP      x6, x7, [sp, #-16]!
  STP      x4, x5, [sp, #-16]!
  STP      x2, x3, [sp, #-16]!
  STP      x0, x1, [sp, #-16]!

  MRS      x5, ESR_EL1
  LSR      x6, x5, #ESR_EL1_EC_SHIFT
  CMP      x6, #ESR_EL1_EC_SVC32
sync_not_supported:  
  B.NE sync_not_supported
  
  //AArch32 SVC handling
  MOV    x1, x0
  AND    x0, x5, #ESR_EL1_EC_SVC_NUM_MASK
  BL     svcHandler
  
 
  LDP      x0, x1, [sp], #16
  LDP      x2, x3, [sp], #16
  LDP      x4, x5, [sp], #16
  LDP      x6, x7, [sp], #16
  LDP      x8, x9, [sp], #16
  LDP      x10, x11, [sp], #16
  LDP      x12, x13, [sp], #16
  LDP      x14, x15, [sp], #16
  LDP      x16, x17, [sp], #16
  LDP      x18, x19, [sp], #16
  LDP      x29, x30, [sp], #16
  ERET
