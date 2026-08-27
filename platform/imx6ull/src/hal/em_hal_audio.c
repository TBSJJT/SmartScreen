#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <alsa/asoundlib.h>

#define MAX_AUDIO_CMD_LEN 200
#define PLAYER_MAX_URL_LENGTH 200

volatile pid_t aplay_pid = -1;
/*
int em_get_audio_vol() {
    char command[MAX_AUDIO_CMD_LEN];
    char output[MAX_AUDIO_CMD_LEN];
    int volume = 0;
    sprintf(command, "amixer -D hw:audiocodec cget name='DAC volume'");

    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        return 0;
    }

    while(fgets(output, MAX_AUDIO_CMD_LEN, fp) != NULL){
        char* volume_str = strstr(output, " values=");
        if (volume_str != NULL) {
            char* volstr = strtok(volume_str, ",");
            while (volstr != NULL) {
                volstr = strtok(NULL, ",");
                if (volstr != NULL) {
                    volume = atoi(volstr);
                }
            }
            printf("get_audio_vol %d\n", volume);
        }
    }
    pclose(fp);
    //0-200的值映射到0-100
    return (volume * 100) / 200;;
}

int em_set_audio_vol(int vol){
    int ret = 0;
    char cmd[MAX_AUDIO_CMD_LEN];
    memset(cmd, 0, sizeof(cmd));
    //0-100的值映射到0-200
	int volume = 200 * vol / 100;
    sprintf(cmd, "amixer -D hw:audiocodec cset name='DAC volume' 0,%d", volume);       
    ret = system(cmd);  
    printf("set_audio_vol %s,ret %d\n", cmd, ret);
    return 0;
}

*/

// 获取音量
int em_get_audio_vol(void) {
    long pmin, pmax, volume = 0;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem = NULL;
    
    if (snd_mixer_open(&handle, 0) < 0) return 0;
    if (snd_mixer_attach(handle, "hw:audiocodec") < 0) {
        if (snd_mixer_attach(handle, "default") < 0) {
            if (snd_mixer_attach(handle, "hw:0") < 0) {
                snd_mixer_close(handle);
                return 0;
            }
        }
    }
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);
    //指定 "Playback"
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Playback");
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        return 0;
    }
    snd_mixer_selem_get_playback_volume_range(elem, &pmin, &pmax);
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);
    snd_mixer_close(handle);
    if (pmax == pmin) return 0;
    // 硬件的 70% 作为实际发声的起点门槛
    long audible_min = pmin + (pmax - pmin) * 70 / 100;
    if (volume <= pmin) return 0;       // 绝对静音
    if (volume <= audible_min) return 1; // 废区兜底显示 1%
    // 反向映射给 UI 进度条
    return (int)((volume - audible_min) * 100 / (pmax - audible_min));
}
// 设置音量
int em_set_audio_vol(int vol) {
    long pmin, pmax, volume;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem = NULL;
    if (snd_mixer_open(&handle, 0) < 0) return -1;
    if (snd_mixer_attach(handle, "hw:audiocodec") < 0) {
        if (snd_mixer_attach(handle, "default") < 0) {
            if (snd_mixer_attach(handle, "hw:0") < 0) {
                snd_mixer_close(handle);
                return -1;
            }
        }
    }
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);
    //指定 "Playback"
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Playback");
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        printf("ALSA API: 找不到 Playback 控制项！\n");
        snd_mixer_close(handle);
        return -1;
    }
    // 限幅保护
    if(vol < 0) vol = 0;
    if(vol > 100) vol = 100;
    snd_mixer_selem_get_playback_volume_range(elem, &pmin, &pmax);
    // 70% 以下硬件没声音，截断
    long audible_min = pmin + (pmax - pmin) * 70 / 100; 
    if (vol == 0) {
        volume = pmin; // 滑块为0时，底层绝对静音
    } else {
        // 滑块 1~100 映射到底层有效发声区间
        volume = audible_min + (pmax - audible_min) * vol / 100;
    }
    
    snd_mixer_selem_set_playback_volume_all(elem, volume);
    snd_mixer_close(handle);
    printf("ALSA API: UI音量 %d%% -> 映射底层寄存器值: %ld\n", vol, volume);
    return 0;
}

void em_stop_play_audio(){
    int ret = 0;
    char cmd[200];
    memset(cmd, 0, 200);
    sprintf(cmd, "killall -9 aplay");      
    ret = system(cmd);
    printf("stop_play_local_audio %s,ret %d\n", cmd, ret);
}

int em_play_audio(const char *url)
{
    int ret = 0;
    char cmd[PLAYER_MAX_URL_LENGTH+10];
    if(url == NULL){
        return 0;
    }
    memset(cmd, 0, PLAYER_MAX_URL_LENGTH);
    sprintf(cmd, "aplay %s", url);  
    // 如果已经在播放，先停止
    if (aplay_pid > 0) {
        printf("kill %d\n",aplay_pid);
        em_stop_play_audio();
        waitpid(aplay_pid, NULL, 0);
        aplay_pid = -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        ret = system("echo 0 > /sys/class/gpio/gpio34/value");
        ret = system(cmd);  
        printf("play_local_audio %s,ret %d\n", cmd, ret);
        ret = system("echo 1 > /sys/class/gpio/gpio34/value");
        exit(1);
    } else if (pid > 0) {
        aplay_pid = pid;
    } else {
        perror("fork failed");
    }
    return 0;
}