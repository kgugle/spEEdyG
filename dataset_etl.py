NUM_CHANNELS = 64
FREQUENCY_RATE = 256
UCI_DATASET = "https://archive.ics.uci.edu/ml/machine-learning-databases/eeg-mld/"
# The small data set (smni97_eeg_data.tar.gz) contains data for the 2 subjects, 
# alcoholic a_co2a0000364 and control c_co2c0000337
# 364 is the alcoholic
# 337 is the control
SMALL_DATASET_FILENAME = "smni_eeg_data"
LARGE_DATASET_TRAINING_FILENAME = "SMNI_CMI_TRAIN"
LARGE_DATASET_TESTING_FILENAME = "SMNI_CMI_TEST"
FULL_DATASET_FILENAME = "eeg_full"
tar_gz_ext = "tar.gz"
tar_ext = "tar"

import tarfile
import urllib2
import os
import gzip

def untar_file(filename, root):
	dir_name = filename.split(".")[0]
	# print(dir_name)
	if (filename.endswith("tar.gz")):
		tar = tarfile.open(filename, "r:gz")
		tar.extractall(root)
		print("... untarred @ " + dir_name)
		tar.close()
		return dir_name
	return "!!!"

def untar_directory(root, dir_files):
	for file in dir_files:
		untar_file(root + "/" + file, root)

# downloads the the source fle to the destination (current dir)
def download_file(input_filename, output_filename, extension):
	file = urllib2.urlopen(UCI_DATASET + input_filename + "." + extension)
	with open(output_filename + "." + extension, "wb") as write_file:
		write_file.write(file.read())
	# print("... downloaded @ " + output_filename + "." + extension)
	return output_filename + "." + extension

def transpose_data(file_content):
	file_content = file_content[4:]
	transposed = [[0.0 for x in range(FREQUENCY_RATE)] for y in range(NUM_CHANNELS)] 
	split_data = file_content.split("\n")
	current_channel = 0
	counter = 0
	for line in split_data:
		counter = counter + 1
		if line.startswith("#") and "chan" in line:
			splits = line.split(" ")
			current_channel = int(splits[3])
		else:
			try: 
				splits = line.split(" ")
				sensor = splits[1]
				time = int(splits[2])
				voltage = float(splits[3])
				transposed[current_channel][time] = voltage
			except Exception:
				continue
	# print(transposed)
	return transposed


if __name__ == "__main__":
	# main defines 
	input_filename = SMALL_DATASET_FILENAME
	output_filename = "small_dataset"
	extension = tar_gz_ext
	full_output_filename = output_filename + "." + extension
	output_dir = "test_data"

	# download and ETL
	user_input = "_"
	if os.path.exists(full_output_filename):
		user_input = raw_input("would you like to redownload the dataset? (y or n) : ")
	if user_input == "y" or not os.path.exists(full_output_filename):
		downloaded_data_file = download_file(input_filename, "small_dataset", tar_gz_ext)
		untarred_data_dir = untar_file(downloaded_data_file, "")

		for root, dirs, files in os.walk(input_filename):
			for subject_filename in files:
				# print(subject_filename)
				untar_file(root + "/" + subject_filename, root)
		# print("\ncombining and un-gzipping patient EEG files...")
		for root, dirs, files in os.walk(input_filename):
			for eeg_dir in dirs:
				for inner_root, inner_dirs, inner_files in os.walk(input_filename + "/" + eeg_dir):
					# print(str(inner_files))
					for eeg_data_file in inner_files:
						f = gzip.open(input_filename + "/" + eeg_dir + "/" + eeg_data_file, 'rb')
						file_content = f.read()
						transposed_content = transpose_data(file_content)
						# print(type(file_content))
						text_file = open(output_dir + "/" + eeg_data_file.split(".")[0] + ".txt", "w")
						text_file.write(transposed_content)
						text_file.close()
						f.close()
						# print(".. combined file @ " + output_dir + "/" + eeg_data_file.split(".")[0] + ".txt")
