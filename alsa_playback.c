#include <alsa/asoundlib.h>
#include <stdio.h>
#include <signal.h>
#define PCM_DEVICE "default"

void sigint(int sig)
{
	printf("Found the ctrl+c signal\n");
	sleep(1);
}

int main(int argc, char **argv)
{
	unsigned int pcm, tmp, dir;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	char *buff;
	int  loops;
	unsigned int period_time=4000;//microseconds(4ms)
	int get_rate,sampling_rate=48000;
	int channels = 2;
	int get_channels;
	int err,seconds;
	int periods = 2;
	int buff_size;
	snd_pcm_uframes_t frames = 192;
	snd_pcm_uframes_t period_size=((period_time*sampling_rate)/(1000000));
	unsigned int rate;


	/*if (argc < 4)
        {
		printf("Usage: %s <sample_rate> <channels> <seconds>\n",
								argv[0]);
		return -1;
	}*/

	//rate 	 = atoi(argv[1]);
	//channels = atoi(argv[2]);
	//seconds  = atoi(argv[3]);

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,SND_PCM_STREAM_PLAYBACK, 0) < 0) 
	{
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",PCM_DEVICE, snd_strerror(pcm));
	}

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);
	

	if ((err = snd_pcm_hw_params_any (pcm_handle, params)) < 0) 
		{
			fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",snd_strerror (err));
			exit (1);
		}
	

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
	{
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));
	}
	
	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0)
	{ 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));
	}
		
	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,SND_PCM_FORMAT_S16_LE) < 0) 
	{
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));
	}

      
       if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sampling_rate, 0) < 0) 
	{
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));
	}

	
       snd_pcm_hw_params_set_period_size(pcm_handle,params,period_size,dir);

       snd_pcm_hw_params_set_period_time(pcm_handle,params,period_time,dir);

	//snd_pcm_sw_params_set_start_threshold(pcm_handle,sparams,(frames/2));
	
	//snd_pcm_sw_params_set_stop_threshold(pcm_handle,sparams,(frames));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
	{
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));
	}

	
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
		printf("(mono)\n");
	else if (tmp == 2)
		printf("(stereo)\n");


	snd_pcm_hw_params_get_rate(params, &sampling_rate, 0);
	printf("rate: %d bps\n", sampling_rate);

		

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &period_size, 0);
	printf("period size : %ld frames\n" , period_size);

	buff_size = (periods * period_size*2);
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &period_time, 0);
	printf("period_time :%d microseconds\n" , period_time);
	
	if (pcm = snd_pcm_start(pcm_handle) < 0)
		{
			printf("ERROR: Can't start pcm %s\n", snd_strerror(pcm));
		}

	
        while(1)
	
	{

               (pcm = read(0, buff, buff_size)) ;
               signal(SIGINT ,sigint);
       
			
		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) 
		{
			
			printf("Buffer is having Zero data.\n");
			snd_pcm_prepare(pcm_handle);
		} 
		
		else if (pcm < 0)
	    {
			printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
	    }

	}

	snd_pcm_drain(pcm_handle);
	if (err < 0)
	{
        	printf("snd_pcm_drain failed: %s\n", snd_strerror(err));
    }
	
	snd_pcm_close(pcm_handle);
	free(buff);

	return 0;
}
