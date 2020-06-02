#
#	makefile for AMR WB Speech Codec
#

include ./amrwb_setting.mk


# Implicit Rules
.c.o:
	$(CC) $(CFLAGS)  -c  $<

amrwb_lib: clean $(AMRWB_LIBOBJ)
	mkdir -p $(AMRWB_BUILD)
	mv *.o $(AMRWB_BUILD)

# Explicit Rules
coder:    $(OBJ)
	$(CC) $(LFLAG) -o coder $(CFLAGS) $(OBJ)
decoder:  $(DOBJ)
	$(CC) $(LFLAG) -o decoder $(CFLAGS) $(DOBJ)
test: clean	$(TOBJ) $(AMRWB_LIBOBJ)
	$(CC) $(LFLAG) -o test $(CFLAGS) $(TOBJ) $(AMRWB_LIBOBJ)



# Individual File Dependencies

basicop2.o:	typedef.h basic_op.h count.h
count.o:	typedef.h count.h
log2.o:		log2.h typedef.h basic_op.h count.h log2_tab.h
oper_32b.o:	typedef.h basic_op.h oper_32b.h count.h
autocorr.o:	typedef.h basic_op.h oper_32b.h acelp.h count.h
az_isp.o:	typedef.h basic_op.h oper_32b.h count.h
bits.o:		typedef.h basic_op.h cnst.h bits.h acelp.h count.h dtx.h
c2t64fx.o:	typedef.h basic_op.h math_op.h acelp.h count.h cnst.h
c4t64fx.o:	typedef.h basic_op.h math_op.h acelp.h count.h cnst.h q_pulse.h
cod_main.o:	typedef.h basic_op.h oper_32b.h math_op.h cnst.h acelp.h \
	cod_main.h bits.h count.h
convolve.o:	typedef.h basic_op.h count.h
cor_h_x.o:	typedef.h basic_op.h math_op.h count.h
d2t64fx.o:	typedef.h basic_op.h count.h cnst.h
d4t64fx.o:	typedef.h basic_op.h count.h cnst.h q_pulse.h
decim54.o:	typedef.h basic_op.h acelp.h count.h cnst.h
dec_main.o:	typedef.h basic_op.h oper_32b.h cnst.h acelp.h dec_main.h  bits.h  count.h  math_op.h 
deemph.o:	typedef.h basic_op.h math_op.h count.h
dtx.o:		typedef.h basic_op.h oper_32b.h math_op.h cnst.h acelp.h bits.h dtx.h count.h log2.h
d_gain2.o:	typedef.h basic_op.h oper_32b.h math_op.h log2.h cnst.h acelp.h count.h 
gpclip.o:	typedef.h basic_op.h count.h 
g_pitch.o:	typedef.h basic_op.h math_op.h count.h 
homing.o:	typedef.h basic_op.h cnst.h bits.h
hp400.o:	typedef.h basic_op.h oper_32b.h acelp.h count.h 
hp50.o:		typedef.h basic_op.h oper_32b.h cnst.h acelp.h count.h 
hp6k.o:		typedef.h basic_op.h acelp.h count.h cnst.h 
hp_wsp.o:	typedef.h basic_op.h oper_32b.h acelp.h count.h 
int_lpc.o:	typedef.h basic_op.h cnst.h acelp.h count.h 
isfextrp.o:	typedef.h basic_op.h oper_32b.h cnst.h acelp.h count.h 
isp_az.o:	typedef.h basic_op.h oper_32b.h count.h cnst.h 
isp_isf.o:	typedef.h basic_op.h count.h 
lagconc.o:	typedef.h basic_op.h count.h cnst.h acelp.h 
lag_wind.o:	typedef.h basic_op.h oper_32b.h 
levinson.o:	typedef.h basic_op.h oper_32b.h acelp.h count.h 
lp_dec2.o:	typedef.h basic_op.h count.h cnst.h 
math_op.o:	typedef.h basic_op.h math_op.h count.h 
ph_disp.o:	typedef.h basic_op.h cnst.h acelp.h count.h 
pitch_f4.o:	typedef.h basic_op.h math_op.h acelp.h cnst.h count.h 
pit_shrp.o:	typedef.h basic_op.h count.h 
pred_lt4.o:	typedef.h basic_op.h count.h 
preemph.o:	typedef.h basic_op.h count.h 
p_med_ol.o:	typedef.h basic_op.h acelp.h oper_32b.h count.h math_op.h 
qisf_ns.o:	typedef.h basic_op.h acelp.h count.h 
qpisf_2s.o:	typedef.h basic_op.h cnst.h acelp.h count.h 
q_gain2.o:	typedef.h basic_op.h oper_32b.h math_op.h count.h log2.h acelp.h 
q_pulse.o:	typedef.h basic_op.h count.h q_pulse.h 
random.o:	typedef.h basic_op.h count.h 
residu.o:	typedef.h basic_op.h count.h 
scale.o:	typedef.h basic_op.h count.h 
syn_filt.o:	typedef.h basic_op.h math_op.h count.h cnst.h 
updt_tar.o:	typedef.h basic_op.h count.h 
util.o:		typedef.h basic_op.h count.h 
voicefac.o:	typedef.h basic_op.h math_op.h count.h 
wb_vad.o:	cnst.h wb_vad.h typedef.h basic_op.h count.h math_op.h wb_vad_c.h 
weight_a.o:	typedef.h basic_op.h count.h 
agc2.o:		cnst.h acelp.h typedef.h basic_op.h count.h math_op.h 
hp7k.o:		typedef.h basic_op.h cnst.h acelp.h count.h 
decoder.o:	typedef.h basic_op.h acelp.h cnst.h main.h bits.h dtx.h count.h 
coder.o:	typedef.h basic_op.h acelp.h cnst.h main.h bits.h count.h cod_main.h 
amrwb_codec.o:	amrwb_codec.c
	$(CC) $(CFLAGS) -c $< -o $@
test.o: test.c
	$(CC) $(CFLAGS) -c $< -o $@

.phony: clean
clean:
	-rm *.o

