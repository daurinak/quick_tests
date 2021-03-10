#include <SDL2/SDL.h>
#include <math.h>

#define seclen (44100)
//#define seclen (48000)
#define duration 3000

// ---- INT definition
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

inline static u64 rotl_u64(u64 input, u32 nbshift){
	return (input<<nbshift)|(input >> (sizeof(input)*8-nbshift));
}
inline static u64 randv(u64 *v){
	u64 a=*v;
	u64 b=rotl_u64(a,1);
	u64 c=rotl_u64(a,2);
	u64 d=rotl_u64(a,3);

	*v= (b&a)^(c^d);

	return *v;
}
inline static void randv_init(u64 *v,u64 seed){
	*v=seed;
	u64 i;
	for(i=0;i<128*6;++i){
		randv(v);
	}
}


//======= math
//


static inline double moine(double inp){
	double mpi=1.*(3.14159265);
	return inp*mpi;
}

static unsigned int comp(u64 tin, double hzin,double intens){
	//double t=(double)(tin%(u64)(seclen+1));
	double t=(double)(tin);
	double hz=hzin*seclen;

	double val=sin(moine(t*hz))*intens;

	int res= (val+1.)*255./2.;
	return res;

}



struct sounddrive{
	u64 t;
	u64 inc_t;

};

static struct sounddrive *new_sounddrive(){
	struct sounddrive *it = (struct sounddrive*)malloc(sizeof(struct sounddrive));

	it->t=0;
	it->inc_t=1;

	return it;
}

//
//=========



// prototype for our audio callback
// see the implementation for more information
void my_audio_callback(void *userdata, Uint8 *stream, int len);


/*
** PLAYING A SOUND IS MUCH MORE COMPLICATED THAN IT SHOULD BE
*/
int main(int argc, char* argv[]){

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
			return 1;

	// local variables
	static Uint8 *wav_buffer; // buffer containing our audio file

	SDL_AudioSpec AudioSettings = {0};
	SDL_AudioSpec ras= {0};

	AudioSettings.freq = seclen;
	//AudioSettings.format = AUDIO_S16LSB;
	AudioSettings.format = AUDIO_U8;
	AudioSettings.channels = 1;
	AudioSettings.samples = 4096;
	AudioSettings.callback = &my_audio_callback;


	// set the callback function
	AudioSettings.callback = my_audio_callback;
	AudioSettings.userdata = (char*)new_sounddrive(); 
	
	/* Open the audio device */
	if ( SDL_OpenAudio(&AudioSettings, &ras) < 0 ){
	  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	  exit(-1);
	}

	
	printf("opened freq=%i \t len=%i \n",ras.freq,ras.samples);

	/* Start playing */
	SDL_PauseAudio(0);
	printf("started\n");

	SDL_Delay(duration); 
	
	// shut everything down
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);

	free(AudioSettings.userdata);
}

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {

	struct sounddrive *it=(struct sounddrive*)userdata;

	int i;
	for(i=0;i<len;++i){
		float t=(double)it->t/(double)seclen;
		int val=0.;
		int count=0;

		//int na=comp(it->t,440,1.);
		int nb=comp(it->t,246.94,1.);
		int nd=comp(it->t,293.66,0.7);

		//val+=na;
		val+=nb;
		val+=nd;

		val+=comp(it->t,440*1,1.);
		val+=comp(it->t,440/2.,0.4);
		val+=comp(it->t,440*2.,0.2);
		val+=comp(it->t,440/16.,0.28);
		val+=comp(it->t,440*24,0.2);
		count++;
		count++;
		count++;
		count++;
		count++;
		count++;
		count++;
		val=val/count;

		if(val>255) val=255;
		if(val<0) val=0;


		if( 1==0 && i!=-1){
			printf("s=%f \t t=%lli \t val=%i\n",t,it->t,val);

		}
		stream[i]=(u8) val;
		it->t+=it->inc_t;
	}
	//printf("filled up to %i\n",i-1);

	return;

}
