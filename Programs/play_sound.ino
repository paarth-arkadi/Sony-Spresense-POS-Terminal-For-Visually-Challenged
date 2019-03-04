#include <SDHCI.h>
#include <Audio.h>

SDClass theSD;
AudioClass *theAudio;

File myFile;

bool ErrEnd = false;
/*Delcaration of audio_attention_cb funtion */
static void audio_attention_cb(const ErrorAttentionParam *atprm){
    puts("Attention!");
    if (atprm->error_code >= AS_ATTENTION_CODE_WARNING){
        ErrEnd = true;
    }
}
void setup(){

}
void loop()
{
    play_audio("Sound.mp3",40000);
}

void play_audio(String filename,int delay_time) // Generally delay time is 40000
{  
    /* Setup the audio player */
    theAudio = AudioClass::getInstance();
    theAudio->begin(audio_attention_cb);
    puts("initialization Audio Library");
    theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
    theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);
    err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/spif/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);
    if (err != AUDIOLIB_ECODE_OK){
        printf("Player0 initialize error\n");
        exit(1);
    }
    myFile = theSD.open(filename); // Takes the file name as the function parameter
    if (!myFile){
        printf("File open error\n");
        exit(1);
    }
    printf("Open! %d\n",myFile);
    err = theAudio->writeFrames(AudioClass::Player0, myFile);
    if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)){
        printf("File Read Error! =%d\n",err);
        myFile.close();
        exit(1);
    }
    puts("Play!");
    theAudio->setVolume(-160);
    theAudio->startPlayer(AudioClass::Player0);
    while(1){
        puts("loop!!");
        int err = theAudio->writeFrames(AudioClass::Player0, myFile);
        if (err == AUDIOLIB_ECODE_FILEEND){
            printf("Main player File End!\n");
            break;
        }
        if (err){
            printf("Main player error code: %d\n", err);
               sleep(1);
            theAudio->stopPlayer(AudioClass::Player0);
            myFile.close();
            break; // Break the while loop
            exit(1);  
        }
        if (ErrEnd){
            printf("Error End\n");
               sleep(1);
            theAudio->stopPlayer(AudioClass::Player0);
            myFile.close();
            break; // Break the while loop
            exit(1);  
        }
        usleep(40000); 
    }
}
