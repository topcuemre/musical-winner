#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include <string.h>  

#define NUM_SECONDS   (25)
#define SAMPLE_RATE   (48000.000)
#define FRAMES_PER_BUFFER  (64)
#define NUM_CHANNELS_INPUT    (1)
#define NUM_CHANNELS_OUTPUT    (2)
#define DEVICE (6)

typedef struct
 {
     int          frameIndex;  /* Index into sample array. */
     int          maxFrameIndex;
     float      *recordedSamples;
 }
 paTestData;

static int recordCallback( const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData )
{
    paTestData *data = (paTestData*)userData;
    const float *rptr = (const float*)inputBuffer;
    float *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS_INPUT];
    long framesToCalc, i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    if( framesLeft < framesPerBuffer )
     {
         framesToCalc = framesLeft;
         finished = paComplete;
     }
     else
     {
         framesToCalc = framesPerBuffer;
         finished = paContinue;
     }
 
     if( inputBuffer == NULL )
     {
         for( i=0; i<framesToCalc; i++ )
         {
             *wptr++ = 1.0f;  /* left */
             if( NUM_CHANNELS_INPUT == 2 ) *wptr++ = 1.0f;  /* right */
         }
     }
     else
     {
         for( i=0; i<framesToCalc; i++ )
         {
             *wptr++ = *rptr++;  /* left */
             if( NUM_CHANNELS_INPUT == 2 ) *wptr++ = *rptr++;  /* right */
         }
     }
     data->frameIndex += framesToCalc;
     return finished;
 }

static int playCallback( const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData )
{
    paTestData *data = (paTestData*)userData;
    float *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS_OUTPUT];
    float *wptr = (float*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )
    {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            *wptr++ = *rptr++;  /* right */
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}

int main(void){
    PaError err;
    int i, totalFrames, numSamples;
    const   PaDeviceInfo *deviceInfo;
    const char * name;
    PaStreamParameters inputParameters, outputParameters;
    PaStream* stream;
    paTestData data;

     //   MEMORY ALLOCATION FOR RECORD ARRAY

    data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CHANNELS_OUTPUT;
    data.recordedSamples = (float *) malloc( numSamples * sizeof(float));

    if( data.recordedSamples == NULL ){
        printf("Could not allocate memory for array.\n");
        goto error;
    }

    for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

     // INPUT DEVICE CONNECTION CHECKING

    err = Pa_Initialize();
    if( err != paNoError ){
        printf( "\n\n\nERROR: Pa_Initialize returned 0x%x\n", err );
        goto error;
    }

    deviceInfo = Pa_GetDeviceInfo(DEVICE);
    name = deviceInfo->name;
    inputParameters.device = DEVICE;
    inputParameters.channelCount = NUM_CHANNELS_INPUT;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    printf("%s  is plugged in!\n",name);

    if( strcmp(name, "Rocksmith USB Guitar Adapter: Audio (hw:1,0)")){
        printf("\n\n\nERROR: Rocksmith Cable is not plugged in returned 0x\n");
        goto error;
    }

     //  RECORDING

    err = Pa_OpenStream(
               &stream,
               &inputParameters,
               NULL,                  /* &outputParameters, */
               SAMPLE_RATE,
               FRAMES_PER_BUFFER,
               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
               recordCallback,
               &data );
    if( err != paNoError ){
        printf( "\n\n\nERROR: Pa_OpenStream returned 0x%x\n", err );
        goto error;
    }
    err = Pa_StartStream( stream );
    if( err != paNoError ){
        printf( "\n\n\nERROR: Pa_StartStream returned 0x%x\n", err );
        goto error;
    }
    for( i = 1; i<4; i++)
    {
        Pa_Sleep(1000);
        printf("Record starting in  %i...\n",i );
    }
    printf("\nRecord has started,  start playing!!!\n");
 
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
        printf("----------------------\n");
    }

    if( err < 0 ){
        printf( "\n\n\nERROR: Pa_IsStreamActive returned 0x%x\n", err );
        goto error;
    }
 
    err = Pa_CloseStream( stream );
    if( err != paNoError ){
        printf( "\n\n\nERROR: Pa_CloseStream returned 0x%x\n", err );
        goto error;
    }

     // PLAYBACK THE RECORD ARRAY

    data.frameIndex = 0;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = NUM_CHANNELS_OUTPUT;                     /* stereo output */
    outputParameters.sampleFormat =  paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    printf("\nPlaying the record back\n"); fflush(stdout);
    err = Pa_OpenStream(
            &stream,
            NULL, /* no input */
            &outputParameters,
            SAMPLE_RATE/2,
            FRAMES_PER_BUFFER,
            paClipOff,      /* we won't output out of range samples so don't bother clipping them */
            playCallback,
            &data );
    if( err != paNoError ) goto error;
     
    if( stream )
    {
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto error;
        
        printf("Waiting for playback to finish.\n"); fflush(stdout);

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
        if( err < 0 ) goto error;
        
        err = Pa_CloseStream( stream );
        if( err != paNoError ) goto error;
        
        printf("Finished.\n"); fflush(stdout);
    }

    Pa_Terminate();
    printf("Record and playback succesffully\n");
    return err;
error:
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
            free( data.recordedSamples );
            printf("Memory deallocated\n");
    if( err != paNoError )
        {
            fprintf( stderr, "An error occured while using the portaudio stream\n" );
            fprintf( stderr, "Error number: %d\n", err );
            fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
            err = 1;          /* Always return 0 or 1, but no other return codes. */
        }
    return err;
}
