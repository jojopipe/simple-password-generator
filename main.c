#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>
#include <assert.h>

#define input_buf_length 64

char random_char(void) {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    uint64_t curr_time = tp.tv_nsec;
    srand(curr_time);
    int random_int = rand();
    char c = (char) (random_int % (126 - 33) + 33);
    return c;
}

char *random_pw(uint32_t len) {
    char *password = (char*) calloc(len, sizeof(char));
    for (uint32_t i = 0; i < len; ++i) {
        password[i] = random_char();
        usleep(1);
    }
    return password;
}

void copy_to_clipboard(char *pw) {
    const size_t len = strlen(pw) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), pw, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    free(pw);
}

void wait_for_user(void) {
    printf("press Enter to exit...");
    fgetc(stdin);
}

struct config {
    int show_pw;
    int clipboard;
};

void load_config(struct config *cfg) {
    cfg->show_pw = 0;
    cfg->clipboard = 1;
    FILE *f = fopen("config","r");
    if (!f) {
        return;
    }
    char buf[1024];
    size_t lines_read = 0;
    while(fgets(buf, 1024, f)) {
        switch(++lines_read) {
            case 3:
                cfg->show_pw = (int) *buf - 48;
                break;
            case 4:
                cfg->clipboard = (int) *buf - 48;
                break;
            default:
                break;
        }
    }
}

int main(void) {
    struct config cfg;
    load_config(&cfg);
    printf("simple password generator (Jorik Pfeiffer, 2025)\n");
    printf("password length: ");
    char input_buf[input_buf_length];
    uint32_t pw_length = 32;
    fgets(input_buf, input_buf_length, stdin);
    if (strlen(input_buf) > 1) {
        pw_length = (uint32_t) strtol(input_buf, NULL, 10);
    }
    char *pw = random_pw(pw_length);
    char *pw_print;
    if (cfg.show_pw) {
        pw_print = pw;
    }
    else {
        pw_print = (char*) calloc(pw_length, sizeof(char));
        memset(pw_print, '*', pw_length);
    }
    printf("password: %s\n", pw_print);
    if (cfg.clipboard) {
        copy_to_clipboard(pw);
        printf("copied to clipboard!\n");
    }
    wait_for_user();
    return 0;
}
