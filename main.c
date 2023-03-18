#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct header_file {
	char chunk_id[4];
	int chunk_size;
	char format[4];
	char sub_chunk1_id[4];
	int sub_chunk1_size;
	short int audio_format;
	short int num_channels;
	int sample_rate;
	int byte_rate;
	short int block_align;
	short int bits_per_sample;
} header;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("No file specified\n");
		return 1;
	}

	FILE *infile = fopen(argv[1], "rb");

	if (infile == NULL) {
		perror("Error");
		return 1;
	}

	FILE *outfile = fopen("reversed.wav", "wb");

	// read header
	header *meta = (header *)malloc(sizeof(header));
	fread(meta, 1, sizeof(header), infile);

	// skip to data
	char read[3], state = 0; // two bytes at a time
	for (size_t i = 0; i < meta->chunk_size - 36; i++) {
		fread(read, 1, 2 * sizeof(char), infile);
		switch (state) {
		case 0:
			if (strcmp(read, "da") == 0)
				state = 1;
			break;
		case 1:
			if (strcmp(read, "ta") == 0)
				goto exit;
			else
				state = 0;
		}
	}
exit:
	// get data size
	int data_size;
	fread(&data_size, 1, sizeof(int), infile);

	// print header info
	printf("chunk_id: \t\t%c%c%c%c\n", meta->chunk_id[0], meta->chunk_id[1], meta->chunk_id[2], meta->chunk_id[3]);
	printf("chunk_size: \t\t%d\n", meta->chunk_size);
	printf("format:  \t\t%c%c%c%c\n", meta->format[0], meta->format[1], meta->format[2], meta->format[3]);
	printf("sub_chunk1_id: \t\t%s\n", meta->sub_chunk1_id);
	printf("sub_chunk1_size: \t%d\n", meta->sub_chunk1_size);
	printf("audio_format: \t\t%d\n", meta->audio_format);
	printf("num_channels: \t\t%d\n", meta->num_channels);
	printf("sample_rate: \t\t%d\n", meta->sample_rate);
	printf("byte_rate: \t\t%d\n", meta->byte_rate);
	printf("block_align: \t\t%d\n", meta->block_align);
	printf("bits_per_sample: \t%d\n", meta->bits_per_sample);

	// write header
	int len = ftell(infile) + 4;
	char *buffer = (char *)malloc(sizeof(char) * len); // copy until byte count len
	rewind(infile);
	fread(buffer, 1, len, infile);
	fwrite(buffer, 1, len, outfile);

	// read data after
	int num_samples = (meta->chunk_size - len) / 2;
	short int *data = (short int *)malloc(sizeof(short int) * num_samples);
	fread(data, sizeof(short int), num_samples, infile);

	// reverse data
	for (int i = 0; i < num_samples / 2; i++) {
		short int temp = data[i];
		data[i] = data[num_samples - i - 1];
		data[num_samples - i - 1] = temp;
	}

	// write reversed data
	fwrite(data, sizeof(short int), num_samples, outfile);
}