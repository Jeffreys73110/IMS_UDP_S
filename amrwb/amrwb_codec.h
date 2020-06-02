#ifndef	__AMRWB_CODER_H__
#define	__AMRWB_CODER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "basic_op.h"
#include "acelp.h"
#include "cnst.h"
#include "main.h"
#include "bits.h"
#include "count.h"
#include "cod_main.h"
#include "mime_io.tab"


int Init_Amrwb_Coder(void **st, TX_State **tx_state);
int Init_Amrwb_Decoder(void **st, RX_State **rx_state);
int Encode_Amrwb(void *st, Word16 allow_dtx, Word16 coding_mode, Word16 *signal, Word16 *prms, long &frame);
int decode_amrwb(void *st, Word16 frame_type, Word16 mode, Word16 *prms, Word16 *synth, long &frame);
int Close_Amrwb_Coder(void *st, TX_State *tx_state);
int Close_Amrwb_Decoder(void *st, RX_State *rx_state);
int encode_audio_file(const char *input_file_name, const char *output_file_name);
int decode_audio_file(const char *input_file_name, const char *output_file_name);

#endif	// __AMRWB_CODER_H__