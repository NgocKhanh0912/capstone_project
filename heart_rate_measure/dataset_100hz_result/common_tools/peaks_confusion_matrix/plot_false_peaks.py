import os
import matplotlib.pyplot as plt
import numpy as np

# Function to plot signal around false positives and false negatives
def plot_fp_fn(ppg_data, fp_list, fn_list, detected_peaks, file_name, save_path, window_size=500):
    """
    Plot PPG signal around false positives (FP) and false negatives (FN),
    and also mark detected peaks within the window.
    
    Args:
        ppg_data (array): Input PPG signal.
        fp_list (list): False Positive peak indices.
        fn_list (list): False Negative peak indices.
        detected_peaks (list): All detected peaks from the model.
        file_name (str): Name of the file being processed.
        save_path (str): Directory to save plots.
        window_size (int): Number of samples around each FP/FN to plot.
    """
    for idx, peak in enumerate(fp_list + fn_list):
        start = max(0, peak - window_size)
        end = min(len(ppg_data), peak + window_size)
        segment = ppg_data[start:end]
        
        local_detected_peaks = [p for p in detected_peaks if start <= p <= end]

        plt.figure(figsize=(10, 4))
        plt.plot(np.arange(start, end), segment, label="PPG Signal", color='gray')
        
        if peak in fp_list:
            plt.axvline(peak, color='r', linestyle='--', label="False Positive")
        if peak in fn_list:
            plt.axvline(peak, color='b', linestyle='--', label="False Negative")

        plt.plot(local_detected_peaks, ppg_data[local_detected_peaks], 'ko', label="Detected Peaks")

        plt.xlabel("Sample Index")
        plt.ylabel("Amplitude")
        plt.title(f"{'FP' if peak in fp_list else 'FN'} - {file_name} (Index: {peak})")
        plt.legend()
        plt.savefig(os.path.join(save_path, f"{file_name}_{'FP' if peak in fp_list else 'FN'}_{idx}.png"))
        plt.close()