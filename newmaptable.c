#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_SIZE 100
#define MAX_FILES 10

// Cấu trúc lưu thông tin ánh xạ file
struct file_mapping_entry {
    char file_name[50];
    void *mapped_address;
    int fd;
};

struct file_mapping_entry file_mapping_table[MAX_FILES];
int table_index = 0;

// Hàm thêm một bản ghi ánh xạ file vào bảng
void add_file_mapping_entry(const char *file_name, void *mapped_address, int fd) {
    strcpy(file_mapping_table[table_index].file_name, file_name);
    file_mapping_table[table_index].mapped_address = mapped_address;
    file_mapping_table[table_index].fd = fd;
    table_index++;
}

int main() {
    const char *file_names[] = {
        "file1.txt",
        "file2.txt",
        "file3.txt"
    };
    int num_files = sizeof(file_names) / sizeof(file_names[0]);

    for (int i = 0; i < num_files; i++) {
        int fd = open(file_names[i], O_RDWR | O_CREAT, 0666);
        if (fd == -1) {
            perror("open");
            continue;
        }

        if (ftruncate(fd, FILE_SIZE) == -1) {
            perror("ftruncate");
            close(fd);
            continue;
        }

        void *addr = mmap(NULL, FILE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            perror("mmap");
            close(fd);
            continue;
        }

        add_file_mapping_entry(file_names[i], addr, fd);

        // Ghi nội dung khác nhau vào từng file
        snprintf((char *)addr, FILE_SIZE, "Hello from %s!", file_names[i]);
    }

    // Giải phóng ánh xạ và đóng file
    for (int i = 0; i < table_index; i++) {
        munmap(file_mapping_table[i].mapped_address, FILE_SIZE);
        close(file_mapping_table[i].fd);
    }

    printf("Đã ánh xạ và ghi nội dung vào %d file thành công.\n", table_index);
    return 0;
}
