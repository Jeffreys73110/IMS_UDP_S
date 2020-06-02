#
#	makefile for AMR WB Speech Codec
#

# For GCC compiler
CC = g++
LFLAG = 
CFLAGS = -Wall

path := ./amrwb
build := ./build

# Objects
LIBOBJ :=  
LIBOBJ += $(build)/amrwb_codec.o 
# LIBOBJ += $(build)/agc2.o 
# LIBOBJ += $(build)/autocorr.o 
# LIBOBJ += $(build)/az_isp.o 
# LIBOBJ += $(build)/bits.o 
# LIBOBJ += $(build)/c2t64fx.o 
# LIBOBJ += $(build)/c4t64fx.o 
# LIBOBJ += $(build)/cod_main.o
# LIBOBJ += $(build)/convolve.o 
# LIBOBJ += $(build)/cor_h_x.o 
# LIBOBJ += $(build)/d2t64fx.o 
# LIBOBJ += $(build)/d4t64fx.o 
# LIBOBJ += $(build)/decim54.o 
# LIBOBJ += $(build)/dec_main.o 
# LIBOBJ += $(build)/deemph.o
# LIBOBJ += $(build)/dtx.o 
# LIBOBJ += $(build)/d_gain2.o 
# LIBOBJ += $(build)/gpclip.o 
# LIBOBJ += $(build)/g_pitch.o 
# LIBOBJ += $(build)/hp6k.o 
# LIBOBJ += $(build)/hp7k.o 
# LIBOBJ += $(build)/hp50.o 
# LIBOBJ += $(build)/hp400.o 
# LIBOBJ += $(build)/hp_wsp.o
# ]LIBOBJ += $(build)/int_lpc.o 
# LIBOBJ += $(build)/isfextrp.o 
# LIBOBJ += $(build)/isp_az.o 
# LIBOBJ += $(build)/isp_isf.o 
# LIBOBJ += $(build)/lagconc.o 
# LIBOBJ += $(build)/lag_wind.o 
# LIBOBJ += $(build)/levinson.o
# LIBOBJ += $(build)/lp_dec2.o 
# LIBOBJ += $(build)/math_op.o 
# LIBOBJ += $(build)/ph_disp.o 
# LIBOBJ += $(build)/pitch_f4.o 
# LIBOBJ += $(build)/pit_shrp.o 
# LIBOBJ += $(build)/pred_lt4.o 
# LIBOBJ += $(build)/preemph.o
# LIBOBJ += $(build)/p_med_ol.o 
# LIBOBJ += $(build)/qisf_ns.o 
# LIBOBJ += $(build)/qpisf_2s.o 
# LIBOBJ += $(build)/q_gain2.o 
# LIBOBJ += $(build)/q_pulse.o 
# LIBOBJ += $(build)/random.o 
# LIBOBJ += $(build)/residu.o
# LIBOBJ += $(build)/scale.o 
# LIBOBJ += $(build)/syn_filt.o 
# LIBOBJ += $(build)/updt_tar.o 
# LIBOBJ += $(build)/util.o 
# LIBOBJ += $(build)/voicefac.o 
# LIBOBJ += $(build)/wb_vad.o 
# LIBOBJ += $(build)/weight_a.o
# LIBOBJ += $(build)/basicop2.o 
# LIBOBJ += $(build)/count.o 
# LIBOBJ += $(build)/log2.o 
# LIBOBJ += $(build)/oper_32b.o 
# LIBOBJ += $(build)/homing.o

# Implicit Rules
.c.o:
	$(CC) $(CFLAGS)  -c  $<

# all:    coder decoder


# Explicit Rules
amrwb_lib: 	clean $(LIBOBJ)
	$(CC) $(LFLAG) -o test $(CFLAGS) $(LIBOBJ)
coder:    $(OBJ)
	$(CC) $(LFLAG) -o coder $(CFLAGS) $(OBJ)
decoder:  $(DOBJ)
	$(CC) $(LFLAG) -o decoder $(CFLAGS) $(DOBJ)


# Individual File Dependencies

$(BUILD)/basicop2.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/count.o:	$(PATH)/typedef.h $(PATH)/count.h
$(BUILD)/log2.o:		$(PATH)/log2.h $(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/log2_tab.h
$(BUILD)/oper_32b.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/count.h
$(BUILD)/autocorr.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/az_isp.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/count.h
$(BUILD)/bits.o:		$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/cnst.h $(PATH)/bits.h $(PATH)/acelp.h $(PATH)/count.h $(PATH)/dtx.h
$(BUILD)/c2t64fx.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/acelp.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/c4t64fx.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/acelp.h $(PATH)/count.h $(PATH)/cnst.h $(PATH)/q_pulse.h
$(BUILD)/cod_main.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/math_op.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/cod_main.h $(PATH)/bits.h $(PATH)/count.h
$(BUILD)/convolve.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/cor_h_x.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/count.h
$(BUILD)/d2t64fx.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/d4t64fx.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/cnst.h $(PATH)/q_pulse.h
$(BUILD)/decim54.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/acelp.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/dec_main.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/dec_main.h $(PATH)/bits.h $(PATH)/count.h $(PATH)/math_op.h
$(BUILD)/deemph.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/count.h
$(BUILD)/dtx.o:		$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/math_op.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/bits.h $(PATH)/dtx.h $(PATH)/count.h $(PATH)/log2.h
$(BUILD)/d_gain2.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/math_op.h $(PATH)/log2.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/gpclip.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/g_pitch.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/count.h
$(BUILD)/homing.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/cnst.h $(PATH)/bits.h 
$(BUILD)/hp400.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/hp50.o:		$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/hp6k.o:		$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/acelp.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/hp_wsp.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/int_lpc.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/isfextrp.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/isp_az.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/isp_isf.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/lagconc.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/cnst.h $(PATH)/acelp.h
$(BUILD)/lag_wind.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h
$(BUILD)/levinson.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/lp_dec2.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/math_op.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/count.h
$(BUILD)/ph_disp.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/pitch_f4.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/acelp.h $(PATH)/cnst.h $(PATH)/count.h
$(BUILD)/pit_shrp.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/pred_lt4.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/preemph.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/p_med_ol.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/acelp.h $(PATH)/oper_32b.h $(PATH)/count.h $(PATH)/math_op.h
$(BUILD)/qisf_ns.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/qpisf_2s.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/q_gain2.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/oper_32b.h $(PATH)/math_op.h $(PATH)/count.h $(PATH)/log2.h $(PATH)/acelp.h
$(BUILD)/q_pulse.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/q_pulse.h
$(BUILD)/random.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/residu.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/scale.o: 	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/syn_filt.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/count.h $(PATH)/cnst.h
$(BUILD)/updt_tar.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/util.o:		$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/voicefac.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/math_op.h $(PATH)/count.h
$(BUILD)/wb_vad.o:	$(PATH)/cnst.h $(PATH)/wb_vad.h $(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/math_op.h $(PATH)/wb_vad_c.h
$(BUILD)/weight_a.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h
$(BUILD)/agc2.o:		$(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/count.h $(PATH)/math_op.h
$(BUILD)/hp7k.o:		$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/cnst.h $(PATH)/acelp.h $(PATH)/count.h
$(BUILD)/decoder.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/acelp.h $(PATH)/cnst.h $(PATH)/main.h $(PATH)/bits.h $(PATH)/dtx.h $(PATH)/count.h
$(BUILD)/coder.o:	$(PATH)/typedef.h $(PATH)/basic_op.h $(PATH)/acelp.h $(PATH)/cnst.h $(PATH)/main.h $(PATH)/bits.h $(PATH)/count.h $(PATH)/cod_main.h
$(BUILD)/amrwb_codec.o:	$(PATH)/amrwb_codec.c
	$(CC) $(CFLAGS) -c $< -o $@;
$(BUILD)/test.o: $(PATH)/test.c
	$(CC) $(CFLAGS) -c $< -o $@;

.phony: $(path)/clean
clean:
	-rm build/*.o

