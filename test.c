#include <stdio.h>

#define SIZE 5

int matrix_1[5][5] =	{
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5}
						};
int matrix_2[5][5] =	{
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5},
							{ 1, 2, 3, 4, 5}
						};
int main() {
	FILE* device = fopen("/dev/matrix-multiplyer", "w");
	if (!device) {
		printf("No device file found!\nMake sure that you run - make install before make test\n");
		return -1;
	}
	char c;
	int output[SIZE][SIZE];
	int output_size;
	// write input to device file
	fprintf(device, "I%d", SIZE);
	
	printf("Running test...\n");
	printf("Input matrix size - %d x %d\n", SIZE, SIZE);
	printf("Matrix 1 -\n");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", matrix_1[i][j]);
			fprintf(device, " %d", matrix_1[i][j]);
		}
		printf("\n");
	}

	printf("Matrix 2 -\n");
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%d ", matrix_2[i][j]);
			fprintf(device, " %d", matrix_2[i][j]);
		}
		printf("\n");
	}
	fclose(device);
	
	// read output from device file
	device = fopen("/dev/matrix-multiplyer", "r");

	fscanf(device, "%c%d", &c, &output_size);
	printf("Output matrix size - %d x %d", output_size, output_size);

	for (int i = 0; i < output_size; i++) {
		for (int j = 0; j < output_size; j++) {
			fscanf(device, "%d", &output[i][j]);
		}
	}
	
	printf("Output matrix -\n");
	for (int i = 0; i < output_size; i++) {
		for (int j = 0; j < output_size; j++) {
			printf("%d ", output[i][j]);
		}
		printf("\n");
	}

	int flag = 1;
	int sum;
	printf("Expected output -\n");
	for (int i = 0; i < output_size; i++) {
		for (int j = 0; j < output_size; j++) {
			sum = 0;
			for (int k = 0; k < output_size; k++) {
				sum += matrix_1[i][k] * matrix_2[k][j];
			}
			printf("%d ", sum);
			if (sum != output[i][j])
				flag = 0;
		}
		printf("\n");
	}

	printf("Result - ");
	if (flag)
		printf("PASSED\n");
	else
		printf("FAILED\n");

	fclose(device);
	return 0;
}
