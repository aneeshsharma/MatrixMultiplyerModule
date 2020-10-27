def write_matrices(device, matrix1, matrix2):
	dim = len(matrix1)
	device.write(f'I{dim}')
	for row in matrix1:
		for col in row:
			device.write(f' {col}')
	for row in matrix2:
		for col in row:
			device.write(f' {col}')
	device.flush()


size = 5
matrix = [[i + 1 for j in range(size)] for i in range(size)]

device = open('/dev/matrix-multiplyer', 'w')

print('Input matrix - ')
print(matrix)

write_matrices(device, matrix, matrix)

device.close()
