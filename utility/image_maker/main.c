/**
 *  file    ImageMaker.c
 *  date    2008/12/16
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   부트 로더와 커널 이미지를 연결하고, 섹터 단위로 정렬해 주는 ImageMaker의
 *          소스 파일
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BYTES_OF_SECTOR  512

static int adjust_in_sector_size(int fd, int source_size);
static void update_bootloader_sector_count(int fd, int total_sector_count, int kernel32_sector_count);
static int copy_file(int source_fd, int target_fd);

int main(int argc, char *argv[]) {
    int source_fd;
    int target_fd;
    int bootloader_size;
    int kernel32_sector_count;
    int kernel64_sector_count;
    int source_size;
    int c;
    char *output_file = NULL;

    if (argc < 4) {
        fprintf(stderr, "[usage] mkimage -o MintOS64Disk.img BootLoader.bin kernel32.bin kernel64.bin\n");
        exit(-1);
    }

    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
            case 'o':
                output_file = optarg;
                break;
            default:
                fprintf(stderr, "[ERROR] unknown option\n");
                exit(1);
        }
    }
    if (output_file == NULL || strlen(output_file) == 0) {
        fprintf(stderr, "[ERROR] output file must be specified.\n");
        exit(1);
    }

    if ((target_fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE)) == -1) {
        fprintf(stderr, "[ERROR] %s open fail.\n", output_file);
        exit(1);
    }

    // 부트 로더 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
    printf("[INFO] Copy boot loader to %s\n", output_file);
    if ((source_fd = open(argv[optind], O_RDONLY)) == -1) {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[optind]);
        exit(1);
    }

    source_size = copy_file(source_fd, target_fd);
    close(source_fd);

    // 파일 크기를 섹터 크기인 512바이트로 맞추기 위해 나머지 부분을 0x00 으로 채움
    bootloader_size = adjust_in_sector_size(target_fd, source_size);
    printf("[INFO] %s size = [%d] and sector count = [%d]\n",
           argv[optind], source_size, bootloader_size);
    optind++;

    // 32비트 커널 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
    printf("[INFO] Copy protected mode kernel to %s\n", output_file);
    if ((source_fd = open(argv[optind], O_RDONLY)) == -1) {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[optind]);
        exit(1);
    }

    source_size = copy_file(source_fd, target_fd);
    close(source_fd);

    // 파일 크기를 섹터 크기인 512바이트로 맞추기 위해 나머지 부분을 0x00 으로 채움
    kernel32_sector_count = adjust_in_sector_size(target_fd, source_size);
    printf("[INFO] %s size = [%d] and sector count = [%d]\n",
           argv[optind], source_size, kernel32_sector_count);
    optind++;

    // 64비트 커널 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
    printf("[INFO] Copy IA-32e mode kernel to %s\n", output_file);
    if ((source_fd = open(argv[optind], O_RDONLY)) == -1) {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[optind]);
        exit(1);
    }

    source_size = copy_file(source_fd, target_fd);
    close(source_fd);

    // 파일 크기를 섹터 크기인 512바이트로 맞추기 위해 나머지 부분을 0x00 으로 채움
    kernel64_sector_count = adjust_in_sector_size(target_fd, source_size);
    printf("[INFO] %s size = [%d] and sector count = [%d]\n",
           argv[optind], source_size, kernel64_sector_count);

    // 디스크 이미지에 커널 정보를 갱신
    printf("[INFO] Starting to update the count of kernel image sectors\n");
    // 부트섹터의 5번째 바이트부터 커널에 대한 정보를 넣음
    update_bootloader_sector_count(target_fd, kernel32_sector_count + kernel64_sector_count, kernel32_sector_count);
    printf("[INFO] Disk image %s creation completed\n", output_file);

    close(target_fd);
    return 0;
}

/**
 * 현재 위치부터 512바이트 배수 위치까지 맞추어 0x00으로 채움
 */
int adjust_in_sector_size(int fd, int source_size) {
    int i;
    int need_to_adjust_size;
    char ch;

    need_to_adjust_size = source_size % BYTES_OF_SECTOR;
    ch = 0x00;

    if (need_to_adjust_size != 0) {
        need_to_adjust_size = 512 - need_to_adjust_size;
        printf("[INFO] File size [%d] and need to filled with [%u] bytes\n", source_size,
               need_to_adjust_size);
        for (i = 0; i < need_to_adjust_size; i++) {
            write(fd, &ch, 1);
        }
    } else {
        printf("[INFO] File size is already aligned to 512 byte\n");
    }

    return (source_size + need_to_adjust_size) / BYTES_OF_SECTOR;
}

/**
 * 부트 로더에 커널에 대한 정보를 삽입
 */
void update_bootloader_sector_count(int fd, int total_sector_count, int kernel32_sector_count) {
    unsigned short data;
    long pos;

    // 파일의 시작에서 5바이트 떨어진 위치가 커널의 총 섹터 수 정보를 나타냄
    pos = lseek(fd, (off_t) 5, SEEK_SET);
    if (pos == -1) {
        fprintf(stderr, "lseek fail. Return value = %ld, errno = %d, %d\n", pos, errno, SEEK_SET);
        exit(1);
    }

    // 부트 로더를 제외한 총 섹터 수 및 보호 모드 커널의 섹터 수 저장
    data = (unsigned short) total_sector_count;
    write(fd, &data, 2);
    data = (unsigned short) kernel32_sector_count;
    write(fd, &data, 2);

    printf("[INFO] The sector count of kernel image [%d]\n", total_sector_count);
    printf("[INFO] The sector count of protected mode kernel image [%d]\n", kernel32_sector_count);
}

/**
 * 소스 파일(Source FD)의 내용을 목표 파일(Target FD)에 복사하고 그 크기를 되돌려줌
 */
int copy_file(int source_fd, int target_fd) {
    ssize_t src_file_size;
    ssize_t rlen;
    ssize_t wlen;
    char buf[BYTES_OF_SECTOR];

    src_file_size = 0;
    while (1) {
        rlen = read(source_fd, buf, sizeof(buf));
        wlen = write(target_fd, buf, rlen);

        if (rlen != wlen) {
            fprintf(stderr, "[ERROR] rlen != wlen.. \n");
            exit(-1);
        }
        src_file_size += rlen;

        if (rlen != sizeof(buf)) {
            break;
        }
    }
    return src_file_size;
}