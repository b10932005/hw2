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
	if (argc < 4) {
		printf("Usage: %s <raw file> <width> <height>\n", argv[0]);
		return -1;
	}

	// convert input arguments to integers, use strtol for error checking
	int32_t width, height;
	char *err;
	width = (int32_t)strtol(argv[2], &err, 10);
	if (*err) {
		printf("Error: width is not an integer\n");
		return -1;
	}
	height = (int32_t)strtol(argv[3], &err, 10);
	if (*err) {
		printf("Error: height is not an integer\n");
		return -1;
	}

	FILE *inputFile = fopen(argv[1], "rb");
	if (!inputFile) {
		perror("Error");
		return -1;
	}

	FILE *outputFile = fopen("out.data", "wb");
	if (!outputFile) {
		perror("Error");
		return -1;
	}

	// Read the image data
	uint8_t *imageData;
	imageData = malloc(width * height * sizeof(uint8_t));
	fread(imageData, sizeof(uint8_t), width * height, inputFile);
	fclose(inputFile);

	// Perform histogram equalization
	histogramEqualization(imageData, width, height);

	fwrite(imageData, sizeof(uint8_t), width * height, outputFile);
}