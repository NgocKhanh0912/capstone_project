import os
import pandas as pd
import numpy as np
from tensorflow.keras.models import load_model

# Function to normalize the PPG signal
def normalize_ppg(ppg_data, normalization_range=(-1, 1)):
    """
    Normalize the PPG signal to a specified range.
    Args:
        ppg_data (array): The input PPG signal data.
        normalization_range (tuple): The range to normalize the data into.
    Returns:
        array: Normalized PPG signal.
    """
    ppg_min, ppg_max = np.min(ppg_data), np.max(ppg_data)
    norm_min, norm_max = normalization_range
    return norm_min + (ppg_data - ppg_min) * (norm_max - norm_min) / (ppg_max - ppg_min)

# Function to detect peaks using a trained model
def detect_peaks_with_model(ppg_data, model, segment_length=1500, threshold=0.5):
    """
    Detect PPG peaks using a trained model.
    Args:
        ppg_data (array): Input PPG signal data.
        model (keras.Model): Pretrained peak detection model.
        segment_length (int): Length of each segment to process.
        threshold (float): Probability threshold for peak detection.
    Returns:
        list: Indices of detected peaks.
    """
    detected_peaks = []
    for i in range(0, len(ppg_data), segment_length):
        segment = ppg_data[i:i+segment_length]
        if len(segment) < segment_length:
            segment = np.pad(segment, (0, segment_length - len(segment)), 'constant', constant_values=0)
        segment = np.expand_dims(np.expand_dims(segment, axis=0), axis=-1)  # Reshape to (1, 1500, 1)
        predictions = model.predict(segment)
        predictions = predictions.flatten()  # Convert to 1D vector
        for idx, value in enumerate(predictions):
            if value > threshold:  # Peak detection threshold
                detected_peaks.append(i + idx)
    return detected_peaks

# Function to compute confusion matrix
def compute_confusion_matrix(detected_peaks, labeled_peaks, fs=100, tolerance=0.075):
    """
    Compute the confusion matrix for peak detection.
    Args:
        detected_peaks (list): Indices of detected peaks.
        labeled_peaks (list): Ground truth labeled peaks.
        fs (int): Sampling frequency of the signal.
        tolerance (float): Tolerance in seconds for matching peaks.
    Returns:
        dict: Confusion matrix metrics including TP, FP, FN, accuracy, precision, recall, DER.
    """
    tolerance = int(tolerance * fs)  # Convert tolerance to number of samples
    tp = 0
    used_detected = set()
    used_labeled = set()
    for labeled_peak in labeled_peaks:
        for detected_peak in detected_peaks:
            if (
                detected_peak not in used_detected
                and abs(detected_peak - labeled_peak) <= tolerance
            ):
                tp += 1
                used_detected.add(detected_peak)
                used_labeled.add(labeled_peak)
                break
    fp_list = [peak for peak in detected_peaks if peak not in used_detected]
    fn_list = [peak for peak in labeled_peaks if peak not in used_labeled]
    fp = len(fp_list)
    fn = len(fn_list)
    accuracy = tp / (tp + fp + fn) if (tp + fp + fn) > 0 else 0
    precision = tp / (tp + fp) if (tp + fp) > 0 else 0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0
    der = (fp + fn) / tp if tp > 0 else 0
    return {
        "tp": tp,
        "fp": fp,
        "fn": fn,
        "accuracy": accuracy,
        "precision": precision,
        "recall": recall,
        "der": der,
        "fp_list": fp_list,
        "fn_list": fn_list
    }

# Paths
test_data_dir = 'I:/dilated_cnn_peak_detection_model_data/test/test_data/data'
label_dir = 'I:/dilated_cnn_peak_detection_model_data/test/test_data/label'
output_dir = "I:/dilated_cnn_peak_detection_model_data/test/model_detected_peaks/"

# Create directory to save results
os.makedirs(output_dir, exist_ok=True)

# Load the model
model = load_model("dataset_100hz_heart_rate_measure_result/model/dilated_cnn_peak_detection_model.h5")

# Test and save results
for file_name in os.listdir(test_data_dir):
    input_file = os.path.join(test_data_dir, file_name)
    label_file = os.path.join(label_dir, file_name.replace(".csv", "_labeled_peaks.csv"))
    if os.path.exists(label_file):
        print(f"Processing: {file_name}")
        
        # Read and normalize data
        ppg_data = pd.read_csv(input_file, header=None).squeeze("columns").values
        ppg_data = normalize_ppg(ppg_data)
        labeled_peaks = pd.read_csv(label_file, header=None).squeeze("columns").values

        # Detect peaks
        detected_peaks = detect_peaks_with_model(ppg_data, model)

        # Save detected peaks
        detected_file = os.path.join(output_dir, file_name.replace(".csv", "_detected_peaks.csv"))
        pd.DataFrame(detected_peaks).to_csv(detected_file, header=False, index=False)

        # Compute confusion matrix
        results = compute_confusion_matrix(detected_peaks, labeled_peaks)
        print(f"File: {file_name}")
        print(f"True Positives (TP): {results['tp']}")
        print(f"False Positives (FP): {results['fp']}")
        print(f"False Negatives (FN): {results['fn']}")
        print(f"Accuracy: {results['accuracy']:.2f}")
        print(f"Precision: {results['precision']:.2f}")
        print(f"Recall: {results['recall']:.2f}")
        print(f"Detection Error Rate: {results['der']:.2f}")
        print("\nFalse Positives (FP) at indices:", results['fp_list'])
        print("False Negatives (FN) at indices:", results['fn_list'])
