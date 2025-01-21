% Path to the CSV file
mimic_afib_ppg_filepath = '../golden_data/mimic_afib_data_100hz.csv';

% Count the number of lines in the file
fileID = fopen(mimic_afib_ppg_filepath, 'r');
sample = 0;
while ~feof(fileID)
    fgetl(fileID); % Read each line from the file
    sample = sample + 1;
end
fclose(fileID);

mimic_afib_ppg_data = csvread(mimic_afib_ppg_filepath, 0, 0, [0, 0, sample-1, 0]);

signal = double(mimic_afib_ppg_data);

% Call the signal_labeling_ui function with the read signal
signal_labeling_ui(signal);
