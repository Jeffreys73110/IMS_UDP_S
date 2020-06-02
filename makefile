include ./amrwb/amrwb_setting.mk


DIR_SEC_AUT = ./sec
DIR_BUILD = ./build


AMRWB_LIBOBJ2 := $(foreach n, $(AMRWB_LIBOBJ), $(UP_AMRWB_BUILD)/$(n))
TEST_OBJ := 
# TEST_OBJ += ${DIR_BUILD}/Authentication.o 
TEST_OBJ += ${DIR_BUILD}/Sip_Response.o 
TEST_OBJ += ${DIR_BUILD}/rtp.o 
TEST_OBJ += ${DIR_BUILD}/UDP.o 
TEST_OBJ += ${DIR_BUILD}/sctp_client.o 
TEST_OBJ += ${AMRWB_LIBOBJ2}

# test: clean dir amrwb ${TEST_OBJ} 
test: clean dir ${TEST_OBJ} 
	g++ ${TEST_OBJ} -o ./ims.out -lpthread
dir:
	mkdir -p build
amrwb: clean_Amrwb_obj
	cd amrwb; make -f amrwb.mk amrwb_lib; cd ..





$(DIR_BUILD)/UDP.o: UDP.cpp
	g++ -c $< -o $@

$(DIR_BUILD)/sctp_client.o: sctp_client.cpp
	g++ -c $< -o $@

$(DIR_BUILD)/Authentication.o: AKAv1-MD5/Authentication.cpp AKAv1-MD5/Authentication.h 
	g++ -c $< -o $@

$(DIR_BUILD)/Sip_Response.o: Sip_Response/Sip_Response.cpp  Sip_Response/Sip_Response.h 
	g++ -c $< -o $@

$(DIR_BUILD)/rtp.o: rtp.cpp  rtp.h
	g++ -c $< -o $@



.phony: clean clean_Amrwb_obj
clean:
	-rm ${DIR_BUILD}/*.o ./ims.out
clean_Amrwb_obj:
	-rm $(UP_AMRWB_BUILD)/*.o
