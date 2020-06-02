#include "amrwb_codec.h"


int main(int argc, char *argv[])
{
	printf("hello\n");
	encode_audio_file("sipaudio.wav.pcm", "sipaudio.enc");
	decode_audio_file("sipaudio.enc", "sipaudio.dec.pcm");

	return 0;
}

