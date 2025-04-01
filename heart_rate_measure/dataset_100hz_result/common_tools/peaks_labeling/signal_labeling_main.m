input_folder = 'E:/mimic_dataset/data/';
output_folder = 'E:/mimic_dataset/labeled_peaks/';

input_files = dir(fullfile(input_folder, '*.csv'));

for i = 1:length(input_files)
    input_filepath = fullfile(input_folder, input_files(i).name);
    output_filepath = fullfile(output_folder, strcat(input_files(i).name(1:end-4), '_labeled_peaks.csv'));

    fileID = fopen(input_filepath, 'r');
    sample = 0;
    while ~feof(fileID)
        fgetl(fileID);
        sample = sample + 1;
    end
    fclose(fileID);

    ppg_data = csvread(input_filepath, 0, 0, [0, 0, sample-1, 0]);
    signal = double(ppg_data);
    signal_labeling_ui(signal, output_filepath);
    
    disp(['Labeled file completed: ' input_files(i).name]);
end

disp('Labeling process completed!');
