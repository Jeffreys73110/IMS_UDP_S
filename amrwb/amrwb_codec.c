#include "amrwb_codec.h"

int Init_Amrwb_Coder(void **st, TX_State **tx_state)
{
	Init_coder(st);
    Init_write_serial(tx_state);
    Init_WMOPS_counter();                  /* for complexity calculation */

	return 0;
}

int Init_Amrwb_Decoder(void **st, RX_State **rx_state)
{
	Init_decoder(st);
	Init_read_serial(rx_state);
	Init_WMOPS_counter();                  /* for complexity calculation */

	return 0;
}

int Encode_Amrwb(void *st, Word16 allow_dtx, Word16 coding_mode, Word16 *signal, Word16 *prms, long &frame)
{
	Word16 reset_flag, i;
	Word16 nb_bits = nb_of_bits[coding_mode];

	frame++;
	fprintf(stderr, " Frames processed: %hd\r", frame);

	Reset_WMOPS_counter();
	//--- check for homing frame
	reset_flag = encoder_homing_frame_test(signal);

	for (i = 0; i < L_FRAME16k; i++)   /* Delete the 2 LSBs (14-bit input) */
	{
		signal[i] = (Word16) (signal[i] & 0xfffC);      logic16(); move16();
	}

	coder(&coding_mode, signal, prms, &nb_bits, st, allow_dtx);

	// 

	WMOPS_output((Word16) (coding_mode == MRDTX));

	// perform homing if homing frame was detected at encoder input
	if (reset_flag != 0)
	{
		Reset_encoder(st, 1);
	}

	return 0;
}

int decode_amrwb(void *st, Word16 frame_type, Word16 mode, Word16 *prms, Word16 *synth, long &frame)
{
	Word16 reset_flag = 0;
	Word16 mode_old = 0;
	Word16 reset_flag_old = 1;
	Word16 frame_length, i;

	Reset_WMOPS_counter();

	frame++;

	fprintf(stderr, " Frames processed: %ld\r", frame);

	if ((frame_type == RX_NO_DATA) | (frame_type == RX_SPEECH_LOST))
	{
		mode = mode_old;
		reset_flag = 0;
	}
	else
	{
		mode_old = mode;

		// if homed: check if this frame is another homing frame
		if (reset_flag_old == 1)
		{
			// only check until end of first subframe
			reset_flag = decoder_homing_frame_test_first(prms, mode);
		}
	}
	

	// produce encoder homing frame if homed & input=decoder homing frame
	if ((reset_flag != 0) && (reset_flag_old != 0))
	{
		for (i = 0; i < L_FRAME16k; i++)
		{
			synth[i] = EHF_MASK;
		}
	} else
	{
		decoder(mode, prms, synth, &frame_length, st, frame_type);
	}

	for (i = 0; i < L_FRAME16k; i++)   /* Delete the 2 LSBs (14-bit output) */
	{
		synth[i] = (Word16) (synth[i] & 0xfffC);      logic16(); move16();
	}

	

	WMOPS_output((Word16) (mode == MRDTX));

	/* if not homed: check whether current frame is a homing frame */
	if (reset_flag_old == 0)
	{
		/* check whole frame */
		reset_flag = decoder_homing_frame_test(prms, mode);
	}
	/* reset decoder if current frame is a homing frame */
	if (reset_flag != 0)
	{
		Reset_decoder(st, 1);
	}
	reset_flag_old = reset_flag;

	return 0;
}

int Close_Amrwb_Coder(void *st, TX_State *tx_state)
{
	Close_coder(st);                       /* Initialize the coder */
    Close_write_serial(tx_state);

	return 0;
}

int Close_Amrwb_Decoder(void *st, RX_State *rx_state)
{
	Close_decoder(st);
    Close_read_serial(rx_state);

	return 0;
}

int encode_audio_file(const char *input_file_name, const char *output_file_name)
{
	FILE *f_speech;                        /* File of speech data                   */
    FILE *f_serial;                        /* File of serial bits for transmission  */
    FILE *f_mode = NULL;                   /* File of modes for each frame          */
    Word16 signal[L_FRAME16k];             /* Buffer for speech @ 16kHz             */
	Word16 prms[NB_BITS_MAX];
    
    Word16 allow_dtx, mode_file, mode = 0, i;
    Word16 bitstreamformat=0;;
    long frame;

    void *st;
	TX_State *tx_state;

	//--- initialize coder
	Init_Amrwb_Coder(&st, &tx_state);
	allow_dtx = 1;
	mode = 0;
	frame = 0;
	
	if ((f_speech = fopen(input_file_name, "rb")) == NULL)
    {
        fprintf(stderr, "Error opening input file  %s !!\n", input_file_name);
        exit(0);
    }
    fprintf(stderr, "Input speech file:  %s\n", input_file_name);

    if ((f_serial = fopen(output_file_name, "wb")) == NULL)
    {
        fprintf(stderr, "Error opening output bitstream file %s !!\n", output_file_name);
        exit(0);
    }
    fprintf(stderr, "Output bitstream file:  %s\n", output_file_name);

	//--- encode audio
	while (fread(signal, sizeof(Word16), L_FRAME16k, f_speech) == L_FRAME16k)
    {
		Encode_Amrwb(st, allow_dtx, mode, signal, prms, frame);
		Write_serial(f_serial, prms, mode, mode, tx_state, bitstreamformat);
	}

	//--- free allocated memory
    Close_Amrwb_Coder(st, tx_state);
    fclose(f_speech);
    fclose(f_serial);
    if (f_mode != NULL)
    {
       fclose(f_mode);
    }

	return 0;

}

int decode_audio_file(const char *input_file_name, const char *output_file_name)
{
	FILE *f_serial;                        /* File of serial bits for transmission  */
    FILE *f_synth;                         /* File of speech data                   */

    Word16 synth[L_FRAME16k];              /* Buffer for speech @ 16kHz             */
    Word16 prms[NB_BITS_MAX];

    Word16 nb_bits, mode, frame_type;
    long frame;

	Word16 bitstreamformat;
	RX_State *rx_state;

	char magic[10];
    void *st;

      fprintf(stderr, "\n");
      fprintf(stderr, " ==================================================================================================\n");
      fprintf(stderr, " AMR Wideband Codec 3GPP TS26.190 / ITU-T G.722.2, Aug 25, 2003. Version %s.\n", CODEC_VERSION);
      fprintf(stderr, " ==================================================================================================\n");
      fprintf(stderr, "\n");


	bitstreamformat = 0;

    /* Open file for synthesis and packed serial stream */

    if ((f_serial = fopen(input_file_name, "rb")) == NULL)
    {
        fprintf(stderr, "Input file '%s' does not exist !!\n", input_file_name);
        exit(0);
    } else
        fprintf(stderr, "Input bitstream file:   %s\n", input_file_name);

    if ((f_synth = fopen(output_file_name, "wb")) == NULL)
    {
        fprintf(stderr, "Cannot open file '%s' !!\n", output_file_name);
        exit(0);
    } else
        fprintf(stderr, "Synthesis speech file:  %s\n", output_file_name);
    /*-----------------------------------------------------------------*
     *           Initialization of decoder                             *
     *-----------------------------------------------------------------*/

    Init_Amrwb_Decoder(&st, &rx_state);

    /*-----------------------------------------------------------------*
     *            Loop for each "L_FRAME" speech data                  *
     *-----------------------------------------------------------------*/

    fprintf(stderr, "\n --- Running ---\n");

	/* read and verify magic number if MIME/storage format specified */
	if (bitstreamformat == 2)
	{
		fread(magic, sizeof(char), 9, f_serial);

		if (strncmp(magic, "#!AMR-WB\n", 9))
		{
			fprintf(stderr, "%s%s\n", "Invalid magic number: ", magic);
			fclose(f_serial);
			fclose(f_synth);
			exit(0);
		}
	}

    frame = 0;
    while ((nb_bits = Read_serial(f_serial, prms, &frame_type, &mode, rx_state, bitstreamformat)) != 0)
    {
		decode_amrwb(st, frame_type, mode, prms, synth, frame);
        fwrite(synth, sizeof(Word16), L_FRAME16k, f_synth);
    }

    Close_Amrwb_Decoder(st, rx_state);
    fclose(f_serial);
    fclose(f_synth);
    
	return 0;
}
