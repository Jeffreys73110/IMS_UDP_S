/*--------------------------------------------------------------------------*
 *                         BITS.H                                           *
 *--------------------------------------------------------------------------*
 *       Number of bits for different modes			                        *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include "typedef.h"
#include "cnst.h"

#define NBBITS_7k     132                  /* 6.60k  */
#define NBBITS_9k     177                  /* 8.85k  */
#define NBBITS_12k    253                  /* 12.65k */
#define NBBITS_14k    285                  /* 14.25k */
#define NBBITS_16k    317                  /* 15.85k */
#define NBBITS_18k    365                  /* 18.25k */
#define NBBITS_20k    397                  /* 19.85k */
#define NBBITS_23k    461                  /* 23.05k */
#define NBBITS_24k    477                  /* 23.85k */

#define NBBITS_SID    35
#define NB_BITS_MAX   NBBITS_24k

#define BIT_0     (Word16)-127
#define BIT_1     (Word16)127
#define BIT_0_ITU (Word16)0x007F
#define BIT_1_ITU (Word16)0x0081

#define SIZE_MAX  (3+NB_BITS_MAX)          /* serial size max */
#define TX_FRAME_TYPE (Word16)0x6b21
#define RX_FRAME_TYPE (Word16)0x6b20

static const Word16 nb_of_bits[NUM_OF_MODES] = {
    NBBITS_7k,
    NBBITS_9k,
    NBBITS_12k,
    NBBITS_14k,
    NBBITS_16k,
    NBBITS_18k,
    NBBITS_20k,
    NBBITS_23k,
    NBBITS_24k,
NBBITS_SID};

typedef struct
{
    Word16 sid_update_counter;
    Word16 sid_handover_debt;
    Word16 prev_ft;
} TX_State;


typedef struct
{
    Word16 prev_ft;
    Word16 prev_mode;
} RX_State;


Word16 Init_write_serial(TX_State ** st);
Word16 Close_write_serial(TX_State *st);
void Reset_write_serial(TX_State * st);
Word16 Init_read_serial(RX_State ** st);
Word16 Close_read_serial(RX_State *st);
void Reset_read_serial(RX_State * st);
void Write_serial(FILE * fp, Word16 prms[], Word16 coding_mode, Word16 mode, TX_State *st, Word16 bitstreamformat);
Word16 Read_serial(FILE * fp, Word16 prms[], Word16 * frame_type, Word16 * mode, RX_State *st, Word16 bitstreamformat);
Word16 Extract_serial(FILE * fp, UWord8 packet[], Word16 * frame_type, Word16 * mode, RX_State *st, Word16 bitstreamformat);

void Parm_serial(
     Word16 value,                         /* input : parameter value */
     Word16 no_of_bits,                    /* input : number of bits  */
     Word16 ** prms
);
Word16 Serial_parm(                        /* Return the parameter    */
     Word16 no_of_bits,                    /* input : number of bits  */
     Word16 ** prms
);
