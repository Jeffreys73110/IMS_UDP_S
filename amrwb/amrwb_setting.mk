#
#	makefile for AMR WB Speech Codec
#

# For GCC compiler
CC = g++
LFLAG = 
CFLAGS = -Wall -O4 -DWMOPS=0 -D__MSDOS__
AMRWB_BUILD = ../build/amrwb
UP_AMRWB_BUILD = ./build/amrwb


AMRWB_LIBOBJ :=  
AMRWB_LIBOBJ += amrwb_codec.o 
AMRWB_LIBOBJ += agc2.o 
AMRWB_LIBOBJ += autocorr.o 
AMRWB_LIBOBJ += az_isp.o 
AMRWB_LIBOBJ += bits.o 
AMRWB_LIBOBJ += c2t64fx.o 
AMRWB_LIBOBJ += c4t64fx.o 
AMRWB_LIBOBJ += cod_main.o
AMRWB_LIBOBJ += convolve.o 
AMRWB_LIBOBJ += cor_h_x.o 
AMRWB_LIBOBJ += d2t64fx.o 
AMRWB_LIBOBJ += d4t64fx.o 
AMRWB_LIBOBJ += decim54.o 
AMRWB_LIBOBJ += dec_main.o 
AMRWB_LIBOBJ += deemph.o
AMRWB_LIBOBJ += dtx.o 
AMRWB_LIBOBJ += d_gain2.o 
AMRWB_LIBOBJ += gpclip.o 
AMRWB_LIBOBJ += g_pitch.o 
AMRWB_LIBOBJ += hp6k.o 
AMRWB_LIBOBJ += hp7k.o 
AMRWB_LIBOBJ += hp50.o 
AMRWB_LIBOBJ += hp400.o 
AMRWB_LIBOBJ += hp_wsp.o
AMRWB_LIBOBJ += int_lpc.o 
AMRWB_LIBOBJ += isfextrp.o 
AMRWB_LIBOBJ += isp_az.o 
AMRWB_LIBOBJ += isp_isf.o 
AMRWB_LIBOBJ += lagconc.o 
AMRWB_LIBOBJ += lag_wind.o 
AMRWB_LIBOBJ += levinson.o
AMRWB_LIBOBJ += lp_dec2.o 
AMRWB_LIBOBJ += math_op.o 
AMRWB_LIBOBJ += ph_disp.o 
AMRWB_LIBOBJ += pitch_f4.o 
AMRWB_LIBOBJ += pit_shrp.o 
AMRWB_LIBOBJ += pred_lt4.o 
AMRWB_LIBOBJ += preemph.o
AMRWB_LIBOBJ += p_med_ol.o 
AMRWB_LIBOBJ += qisf_ns.o 
AMRWB_LIBOBJ += qpisf_2s.o 
AMRWB_LIBOBJ += q_gain2.o 
AMRWB_LIBOBJ += q_pulse.o 
AMRWB_LIBOBJ += random.o 
AMRWB_LIBOBJ += residu.o
AMRWB_LIBOBJ += scale.o 
AMRWB_LIBOBJ += syn_filt.o 
AMRWB_LIBOBJ += updt_tar.o 
AMRWB_LIBOBJ += util.o 
AMRWB_LIBOBJ += voicefac.o 
AMRWB_LIBOBJ += wb_vad.o 
AMRWB_LIBOBJ += weight_a.o
AMRWB_LIBOBJ += basicop2.o 
AMRWB_LIBOBJ += count.o 
AMRWB_LIBOBJ += log2.o 
AMRWB_LIBOBJ += oper_32b.o 
AMRWB_LIBOBJ += homing.o

TOBJ = test.o 
