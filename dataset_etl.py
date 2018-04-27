UCI_DATASET = "https://archive.ics.uci.edu/ml/machine-learning-databases/eeg-mld/"
SMALL_DATASET_FILENAME = "smni_eeg_data"
LARGE_DATASET_TRAINING_FILENAME = "SMNI_CMI_TRAIN"
LARGE_DATASET_TESTING_FILENAME = "SMNI_CMI_TEST"
FULL_DATASET_FILENAME = "eeg_full"
tar_gz_ext = "tar.gz"
tar_ext = "tar"

import tarfile
import urllib2
import os.path

def untar_file(filename):
	if (fname.endswith("tar.gz")):
		tar = tarfile.open(filename, "r:gz")
		tar.extractall()
		tar.close()

# downloads the the source fle to the destination (current dir)
def download_file(input_filename, output_filename, extension):
	file = urllib2.urlopen(UCI_DATASET + input_filename + "." + extension)
	with open(output_filename + "." + extension, "wb") as write_file:
		write_file.write(file.read())
	print("... downloaded and wrote to " + output_filename + "." + extension)
	return output_filename + "." + extension

if __name__ == "__main__":
	# main defines 
	output_filename = SMALL_DATASET_FILENAME
	extension = tar_gz_ext

	full_output_filename = output_filename + "." + extension
	if os.path.exists(full_output_filename):
		user_input = raw_input('''the dataset is already downloaded to the current directory
							   \nwould you like to redownload? (y or n)''')
		if user_input == "y":
			downloaded_filename = download_file(SMALL_DATASET_FILENAME, "small_dataset", tar_gz_ext)
			

