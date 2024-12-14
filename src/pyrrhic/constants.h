#ifndef PYRRHIC_CONSTANTS_H
#define PYRRHIC_CONSTANTS_H

#define TB_MAX_MOVES                288
#define TB_MAX_CAPTURES             64

#define TB_LOSS                     0       /* LOSS */
#define TB_BLESSED_LOSS             1       /* LOSS but 50-move draw */
#define TB_DRAW                     2       /* DRAW */
#define TB_CURSED_WIN               3       /* WIN but 50-move draw  */
#define TB_WIN                      4       /* WIN  */

#define TB_RESULT_WDL_MASK          0x0000000F
#define TB_RESULT_TO_MASK           0x000003F0
#define TB_RESULT_FROM_MASK         0x0000FC00
#define TB_RESULT_PROMOTES_MASK     0x00070000
#define TB_RESULT_EP_MASK           0x00080000
#define TB_RESULT_DTZ_MASK          0xFFF00000
#define TB_RESULT_WDL_SHIFT         0
#define TB_RESULT_TO_SHIFT          4
#define TB_RESULT_FROM_SHIFT        10
#define TB_RESULT_PROMOTES_SHIFT    16
#define TB_RESULT_EP_SHIFT          19
#define TB_RESULT_DTZ_SHIFT         20

#endif // PYRRHIC_CONSTANTS_H
