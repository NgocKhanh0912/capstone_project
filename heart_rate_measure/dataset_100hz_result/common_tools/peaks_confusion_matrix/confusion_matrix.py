def compute_confusion_matrix(detected_peaks, labeled_peaks, fs=100, tolerance=0.075):
    """
    Compute the confusion matrix for peak detection.
    Parameters:
        - detected_peaks (list): Indices of detected peaks.
        - labeled_peaks (list): Ground truth labeled peaks.
        - fs (int): Sampling frequency of the signal.
        - tolerance (float): Tolerance in seconds for matching peaks.
    Returns:
        - dict: Confusion matrix metrics including TP, FP, FN, accuracy, precision, recall, DER.
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
    fp_list = [int(peak) for peak in detected_peaks if peak not in used_detected]
    fn_list = [int(peak) for peak in labeled_peaks if peak not in used_labeled]

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