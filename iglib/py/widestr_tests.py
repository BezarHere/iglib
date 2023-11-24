from folder import current_folder

if __name__ == "__main__":
	with open(current_folder().joinpath('widestrings.txt'), 'wb') as f:
		for i in range(20, 1 << 16):
			f.write(f"{i} \"".encode())

