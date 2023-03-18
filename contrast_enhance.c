#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_GRAY_LEVEL 255

void histogramEqualization(uint8_t *imageData, int32_t width, int32_t height) {
	int32_t pixelCount = width * height;
	int32_t histogram[MAX_GRAY_LEVEL + 1] = {0};
	float cdf[MAX_GRAY_LEVEL + 1] = {0};
	float equalizedHistogram[MAX_GRAY_LEVEL + 1] = {0};

	// Calculate the histogram
	for (int32_t i = 0; i < pixelCount; i++) {
		histogram[imageData[i]]++;
	}

	// Calculate the cumulative distribution function (CDF)
	cdf[0] = (float)histogram[0] / pixelCount;
	for (int32_t i = 1; i <= MAX_GRAY_LEVEL; i++) {
		cdf[i] = cdf[i - 1] + (float)histogram[i] / pixelCount;
	}

	// Calculate the equalized histogram
	for (int32_t i = 0; i <= MAX_GRAY_LEVEL; i++) {
		equalizedHistogram[i] = cdf[i] * MAX_GRAY_LEVEL;
	}

	// Map the original gray levels to the equalized gray levels
	for (int32_t i = 0; i < pixelCount; i++) {
		imageData[i] = (uint8_t)equalizedHistogram[imageData[i]];
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Error: no input file\n");
		return -1;
	}

	// Load image data from file or other source
	uint8_t *imageData;
	int32_t width, height;

	FILE *inputFile = fopen(argv[1], "rb");
	if (inputFile == NULL) {
		perror("Error");
		return -1;
	}

	// Read image dimensions from file header
	fread(&width, sizeof(int32_t), 1, inputFile);
	fread(&height, sizeof(int32_t), 1, inputFile);

	imageData = malloc(width * height * sizeof(uint8_t));

	fread(imageData, sizeof(uint8_t), width * height, inputFile);

	fclose(inputFile);

	// Perform histogram equalization
	histogramEqualization(imageData, width, height);

	FILE *outputFile = fopen(argv[2], "wb");
	if (outputFile == NULL) {
		printf("Error opening output file %s\n", argv[2]);
		return -1;
	}

	fwrite(&width, sizeof(int32_t), 1, outputFile);
	fwrite(&height, sizeof(int32_t), 1, outputFile);

	fwrite(imageData, sizeof(uint8_t), width * height, outputFile);

	fclose(outputFile);

	free(imageData);

	return 0;
}