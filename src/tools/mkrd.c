//
// Created by Samuel Tebbs on 17/04/2018.
//

#include <printf.h>
#include <stdlib.h>
#include <string.h>

#include "../fs/initrd.h"

uint32_t write_header(char *filename, FILE *file, uint32_t offset, FILE *output_file);

void write_body(FILE *file, FILE *output_file);

void err(char* msg, int code) {
    printf("%s\n", msg);
    exit(code);
}

int main(int argc, char** argv) {
    if(argc < 2) err("Usage: mkrd <output> [files]", 1);

    char* output_filename = argv[1];
    FILE* output_file = fopen(output_filename, "w");
    if(!output_file) err("Couldn't open output file", 1);

    uint32_t num_files = (uint32_t) (argc - 2);
    FILE **files = malloc(sizeof(FILE *) * num_files);
    char** filenames = malloc(sizeof(char*) * num_files);
    if(num_files > 0) {
        // this is used to make sure all the files can be opened before writing anything to the output file
        for (int i = 2; i < argc; ++i) {
            char *filename = argv[i];
            filenames[i - 2] = filename;
            files[i - 2] = fopen(filename, "r");
            if(!files[i - 2] || strlen(filename) > INITRD_FILENAME_MAX) {
                free(files);
                free(filenames);
                fclose(output_file);
                err("Filename too long or couldn't open file", 1);
            }
        }
    }
    initrd_t* header = malloc(sizeof(initrd_t));
    header->n_nodes = num_files;
    fwrite(header, sizeof(initrd_t), 1, output_file);
    uint32_t offset = sizeof(initrd_t) + sizeof(initrd_file_t) * num_files;
    for (int j = 0; j < num_files; ++j) {
        offset = write_header(filenames[j], files[j], offset, output_file);
    }
    for (int j = 0; j < num_files; ++j) {
        write_body(files[j], output_file);
    }
    free(filenames);
    free(files);
    fclose(output_file);
}

void write_body(FILE *file, FILE *output_file) {
    char buff[1024];
    size_t len = 0;
    while ((len = fread(buff, sizeof(char), 1024, file)) > 0) {
        fwrite(buff, sizeof(char), len, output_file);
    }
}

uint32_t write_header(char *filename, FILE *file, uint32_t offset, FILE *output_file) {
    initrd_file_t* file_header = malloc(sizeof(initrd_file_t));
    strcpy(file_header->name, filename);
    file_header->offset = offset;
    fseek(file, 0, SEEK_END);
    file_header->length = (uint32_t) ftell(file);
    rewind(file);
    fwrite(file_header, sizeof(initrd_file_t), 1, output_file);
    return offset + file_header->length;
}