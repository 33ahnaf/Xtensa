#include "AudioStreamer.h"
#include <Arduino.h>
#include <stdint.h>
#include <SD.h>
#include <driver/i2s.h>
#include <math.h>
// #include "esp_system.h"
// #include "esp_heap_caps.h"


// #define MAX_FILENAME_SIZE 300
#define BUFF_SIZE 1024
#define HIGH_LIMIT 1.0f
#define LOW_LIMIT 0.0f
#define DELTA 0.1f
#define REVERB_DELAY 250 // 250ms
#define JUNO_DELAY 50 // 50ms
#define TRUE 1
#define FALSE 0


typedef struct {
    // char path[MAX_FILENAME_SIZE];   /*  Path to the file. */
    File stream;           /*  Audio file stream.           */
    uint32_t sample_rate;   /*  Sample rate in hertz.        */
    uint32_t file_size;     /*  Total file size in bytes.    */
    uint32_t data_size;     /*  Raw data size in bytes.      */
    uint32_t header_size;   /*  Length of header in bytes.   */
} AudioFile;

typedef struct {
    float low;
    float high;
} EQState;

typedef struct {
    float *buffer;
    int writePos;
    int size;
} ReverbState;

typedef struct {
    float *buffer;
    int writePos;
    int size;
    float phase1;
    float phase2;
} JunoState;


uint8_t inBuf[BUFF_SIZE];
uint8_t outBuf[BUFF_SIZE * 2];
AudioFile audioFile;
EQState left = {0};
EQState right = {0};
ReverbState revL = {0};
ReverbState revR = {0};
JunoState junoL = {0};
JunoState junoR = {0};

float bassGain = 100.0f / 100.0f;
float midGain = 10.0f / 100.0f;
float treGain = 100.0f / 100.0f;
float reverbMix = 0.2f; //wet amount
float reverbFeedback = 0.4f;
float junoRate = 0.15f; // 0.05f
float junoDepth = 700.0f; // 800.0f
float junoMix = 0.5f; // 0.4f

// optimal DO NOT TOUCH
float lowCut = 0.01;
float highCut = 0.125;


void dacLow(void);
void showINFO(void);
static void i2s_attach(uint32_t sample_rate);
static void i2s_detach(void);
static void AudioFile_init(AudioFile *audioFile);
static void applyReverb(float *sample, ReverbState *st);
static void applyJuno(float *sample, JunoState *st);
static void applyEQ(uint8_t *sample, EQState *st, float bassGain, float midGain, float treGain);
static void Reverb_allocate(ReverbState *st, uint32_t sample_rate);
static void Reverb_release(ReverbState *st);
static void Juno_allocate(JunoState *st, uint32_t sample_rate);
static void Juno_release(JunoState *st);
static void Reverb_Juno_allocate_all(uint32_t sample_rate);
static void Reverb_Juno_release_all(void);
static void fastForwardAudio(AudioFile *audioFile);
static void rewindAudio(AudioFile *audioFile);
static void progressBar(AudioFile *audioFile);


void streamAudio(const char *src){
    junoR.phase1 = 3.141592f;
    junoR.phase2 = 0.0f;
    
    audioFile.stream = SD.open(src);

    if(!audioFile.stream){
        Serial.println("Cannot open source file!");
        dacLow();
        return;
    }
    AudioFile_init(&audioFile);
    i2s_attach(audioFile.sample_rate);
    
    // Serial.printf("Free heap: %u bytes\n\n", ESP.getFreeHeap());
    // Serial.printf("Free heap: %u\n\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
    // Serial.printf("Largest block: %u\n\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    Reverb_Juno_allocate_all(audioFile.sample_rate);
    // Serial.printf("Free heap: %u bytes\n\n", ESP.getFreeHeap());
    // Serial.printf("Free heap: %u\n\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
    // Serial.printf("Largest block: %u\n\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    uint8_t pauseAudio = FALSE;
    uint8_t exitFlag = FALSE;
    uint32_t bytesWritten = 0; // unused
    size_t bytesRead; // must NOT be unsigned for it is used by FS::read() which returns -1 on error occurrence
    size_t outIndex = 0;
    size_t currentTime = 0;
    size_t previousTime = 0;
    audioFile.stream.seek(audioFile.header_size);

    while(1){
        if(Serial.available() > 0){
            char c = (char) Serial.read();
            switch(c){
                case 'q': bassGain = bassGain >= HIGH_LIMIT ? HIGH_LIMIT : bassGain + DELTA; showINFO(); break;
                case 'a': bassGain = bassGain <= LOW_LIMIT ? LOW_LIMIT : bassGain - DELTA; showINFO(); break;

                case 'w': midGain = midGain >= HIGH_LIMIT ? HIGH_LIMIT : midGain + DELTA; showINFO(); break;
                case 's': midGain = midGain <= LOW_LIMIT ? LOW_LIMIT : midGain - DELTA; showINFO(); break;

                case 'e': treGain = treGain >= HIGH_LIMIT ? HIGH_LIMIT : treGain + DELTA; showINFO(); break;
                case 'd': treGain = treGain <= LOW_LIMIT ? LOW_LIMIT : treGain - DELTA; showINFO(); break;

                case 'r': junoRate = junoRate >= HIGH_LIMIT ? HIGH_LIMIT : junoRate + 0.05f; showINFO(); break;
                case 'f': junoRate = junoRate <= LOW_LIMIT ? LOW_LIMIT : junoRate - 0.05f; showINFO(); break;

                case 't': junoDepth = junoDepth >= 10000 ? 10000 : junoDepth + 100.0f; showINFO(); break;
                case 'g': junoDepth = junoDepth <= 0 ? 0 : junoDepth - 100.0f; showINFO(); break;

                case 'y': junoMix = junoMix >= HIGH_LIMIT ? HIGH_LIMIT : junoMix + 0.05f; showINFO(); break;
                case 'h': junoMix = junoMix <= LOW_LIMIT ? LOW_LIMIT : junoMix - 0.05f; showINFO(); break;

                case 'u': reverbMix = reverbMix >= HIGH_LIMIT ? HIGH_LIMIT : reverbMix + 0.05f; showINFO(); break;
                case 'j': reverbMix = reverbMix <= LOW_LIMIT ? LOW_LIMIT : reverbMix - 0.05f; showINFO(); break;

                case 'i': reverbFeedback = reverbFeedback >= HIGH_LIMIT ? HIGH_LIMIT : reverbFeedback + 0.05f; showINFO(); break;
                case 'k': reverbFeedback = reverbFeedback <= LOW_LIMIT ? LOW_LIMIT : reverbFeedback - 0.05f; showINFO(); break;

                case ' ': pauseAudio = !pauseAudio; Serial.println(pauseAudio ? "Streaming paused!" : "Resumed!"); break;
                case 'x': exitFlag = TRUE; break;

                case ',': rewindAudio(&audioFile); progressBar(&audioFile); break;
                case '.': fastForwardAudio(&audioFile); progressBar(&audioFile); break;
            }
        }
        if(exitFlag)
            break;
        if(pauseAudio){
            delay(10);
            continue;
        }
        outIndex = 0;
        bytesRead = audioFile.stream.read(inBuf, BUFF_SIZE);
        if(bytesRead == 0)  break;
        for(size_t i = 0; i < bytesRead; i+=2){ // do i+1 instead of only i to make sure it's safe when bytesRead is odd (no need)
            applyEQ(&inBuf[i], &left, bassGain, midGain, treGain);
            applyEQ(&inBuf[i+1], &right, bassGain, midGain, treGain);
            outBuf[outIndex++] = inBuf[i];
            outBuf[outIndex++] = inBuf[i + 1];
            outBuf[outIndex++] = inBuf[i];
            outBuf[outIndex++] = inBuf[i + 1];
        }
        i2s_write(I2S_NUM_0, outBuf, outIndex, &bytesWritten, portMAX_DELAY); // use outIndex exclusively, as the outBuf becomes x2 of inBuf
        currentTime = millis();
        if(currentTime - previousTime >= 1000){
            progressBar(&audioFile);
            previousTime = currentTime;
        }
    }

    audioFile.stream.close();
    i2s_detach();
    Reverb_Juno_release_all();
    dacLow();
}



void dacLow(void){
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    digitalWrite(25, LOW);
    digitalWrite(26, LOW);
}

static void i2s_attach(uint32_t sample_rate){
    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = sample_rate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = BUFF_SIZE,
        .use_apll = true,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

static void i2s_detach(void){
    i2s_driver_uninstall(I2S_NUM_0);
}

void showINFO(void){
    Serial.printf("bass: %1.1f  mid: %1.1f  tre: %1.1f     junoRate: %1.2f  junoDepth: %4.1f  junoMix: %1.2f     reverbMix: %1.2f  reverbFB: %1.2f\n", bassGain, midGain, treGain, junoRate, junoDepth, junoMix, reverbMix, reverbFeedback);
}

static void AudioFile_init(AudioFile *audioFile){
    audioFile->stream.seek(4);
    audioFile->stream.read((uint8_t*)&audioFile->file_size, 4);
    audioFile->file_size+=8;

    audioFile->stream.seek(24);
    audioFile->stream.read((uint8_t*)&audioFile->sample_rate, 4);

    audioFile->stream.seek(40);
    for(int i = 40; i < 100; i++){
        if(audioFile->stream.read() == 'd' && audioFile->stream.read() == 'a' && audioFile->stream.read() == 't' && audioFile->stream.read() == 'a'){
            audioFile->stream.seek(i+4);
            audioFile->stream.read((uint8_t*)&audioFile->data_size, 4);
            audioFile->header_size = audioFile->file_size - audioFile->data_size;
            break;
        }
    }
    audioFile->stream.seek(0);
}

static void applyJuno(float *sample, JunoState *st){
    st->buffer[st->writePos] = *sample;

    float lfo1 = (sin(st->phase1) + 1.0f) * 0.5f;
    float lfo2 = (sin(st->phase2) + 1.0f) * 0.5f;

    float delay1 = 200 + lfo1 * junoDepth;
    float delay2 = 200 + lfo2 * junoDepth;

    int readPos1 = st->writePos - (int)delay1;
    int readPos2 = st->writePos - (int)delay2;

    if(readPos1 < 0) readPos1 += st->size;
    if(readPos2 < 0) readPos2 += st->size;

    float delayed1 = st->buffer[readPos1];
    float delayed2 = st->buffer[readPos2];

    float chor = (delayed1 + delayed2) * 0.5f;

    *sample = *sample * (1.0f - junoMix) + chor * junoMix;
    st->writePos++;
    if(st->writePos >= st->size)
        st->writePos = 0;
    st->phase1 += junoRate * 0.001f;
    st->phase2 += junoRate * 0.001f * 0.7f;
    if(st->phase1 > 6.283185f)   st->phase1 -= 6.283185f;
    if(st->phase2 > 6.283185f)   st->phase2 -= 6.283185f;
}

static void applyReverb(float *sample, ReverbState *st){
    float delayed = st->buffer[st->writePos];
    *sample = *sample + delayed * reverbMix;
    st->buffer[st->writePos] = *sample + delayed * reverbFeedback;
    st->writePos++;
    if(st->writePos >= st->size)
        st->writePos = 0;
}

static void applyEQ(uint8_t *sample, EQState *st, float bassGain, float midGain, float treGain){
    // Normalize
    float s = (float)(*sample - 128);

    // low band
    st->low += lowCut * (s - st->low);

    // high band
    float hp = s - st->low;
    st->high += highCut * (hp - st->high);

    float low = st->low;
    float high = st->high;
    float mid = s - low - high;
    float out = low * bassGain + mid * midGain + high * treGain;

    applyReverb(&out, st == &left ? &revL : &revR);
    applyJuno(&out, st == &left ? &junoL : &junoR);

    *sample = constrain((int)(out + 128), 0, 255);
}

static void Reverb_allocate(ReverbState *st, uint32_t sample_rate){
    st->size = (REVERB_DELAY / 1000.0f) * sample_rate;
    st->buffer = (float*)malloc(st->size * sizeof(float));
    memset(st->buffer, 0, st->size * sizeof(float));
    Serial.printf("Reverb size: %u\n", st->size);
}

static void Reverb_release(ReverbState *st){
    free(st->buffer);
    st->size = 0;
}

static void Juno_allocate(JunoState *st, uint32_t sample_rate){
    st->size = (JUNO_DELAY / 1000.0f) * sample_rate;
    st->buffer = (float*)malloc(st->size * sizeof(float));
    memset(st->buffer, 0, st->size * sizeof(float));
    Serial.printf("Juno size: %u\n", st->size);
}

static void Juno_release(JunoState *st){
    free(st->buffer);
    st->size = 0;
}

static void Reverb_Juno_allocate_all(uint32_t sample_rate){
    Serial.printf("Sample rate: %u\n", sample_rate);
    Reverb_allocate(&revL, sample_rate);
    Reverb_allocate(&revR, sample_rate);
    Juno_allocate(&junoL, sample_rate);
    Juno_allocate(&junoR, sample_rate);
}

static void Reverb_Juno_release_all(void){
    Reverb_release(&revL);
    Reverb_release(&revR);
    Juno_release(&junoL);
    Juno_release(&junoR);
}

static void fastForwardAudio(AudioFile *audioFile){
    if(audioFile->stream.position() + audioFile->sample_rate*2*5 > audioFile->file_size){
        audioFile->stream.seek(audioFile->file_size-1);
        Serial.printf("\nCurrent position: %dth byte\n", audioFile->stream.position());
    }else{
        audioFile->stream.seek(audioFile->stream.position() + audioFile->sample_rate*2*5);
        Serial.printf("\nCurrent position: %dth byte\n", audioFile->stream.position());
    }
}

static void rewindAudio(AudioFile *audioFile){
    if(audioFile->stream.position() - audioFile->sample_rate*2*5 > 0){
        audioFile->stream.seek(audioFile->stream.position() - audioFile->sample_rate*2*5);
        Serial.printf("\nCurrent position: %dth byte\n", audioFile->stream.position());
    }else{
        audioFile->stream.seek(audioFile->header_size);
        Serial.printf("\nCurrent position: %dth byte\n", audioFile->stream.position());
    }
}

static void progressBar(AudioFile *audioFile){
    int progress = (int)((double)audioFile->stream.position() / audioFile->data_size * 100.0f);
    int totalSeconds = audioFile->data_size/(audioFile->sample_rate*2);
    int currentSecond = audioFile->stream.position()/(audioFile->sample_rate*2);
    Serial.printf("\r%3d%%", progress);
    Serial.printf(" [");
    for(int i = 0; i < 50; i++){
        if(i < progress/2)
            Serial.printf("+");
        else
            Serial.printf(" ");
    }
    Serial.printf("]\t%02d:%02d / %02d:%02d", currentSecond/60, currentSecond%60, totalSeconds/60, totalSeconds%60);
    Serial.flush(TRUE);
}


/*
    Simple audio streaming Mono/Stereo without Oversampling:
        uint8_t buffer[BUFFER_SIZE:1024]; 

        uint32_t bytesWritten = 0;
        uint32_t bytesRead = audioFile.read(buffer, BUFFER_SIZE);
        if(bytesRead == 0)  break;
        i2s_write(I2S_NUM_0, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
         * Use >bytesRead< instead of direct >buffer< as the actualy bytesRead
         * becomes less than buffer size at the very end of the file.
        
    Audio streaming Mono with Oversampling:
        uint8_t inBuf[BUFFER_SIZE:1024];
        uint8_t outBuf[BUFFER_SIZE * 2: 1024 * 2: 2048];

        uint32_t outIndex = 0;
        uint32_t bytesRead = audioFile.read(inBuf, BUFFER_SIZE);
        if(bytesRead == 0)  break;
        for(uint32_t i = 0; i < bytesRead; i++){
            outBuf[outIndex++] = inBuf[i]; // original
            outBuf[outIndex++] = inBuf[i]; // duplicate
        }
        i2s_write(I2S_NUM_0, outBuf, outIndex, &bytesWritten, portMAX_DELAY); // use outIndex exclusively, as the outBuf becomes x2 of inBuf
*/
