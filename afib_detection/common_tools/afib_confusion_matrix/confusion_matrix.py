import numpy as np

def compute_afib_confusion_matrix(detected_afib, labeled_afib, total_duration):
    """
    Compute confusion matrix for AFib detection based on duration and episode.

    Parameters:
    - detected_afib (list of tuples): [(onset, offset), ...] AFib episodes detected.
    - labeled_afib (list of tuples): [(onset, offset), ...] Ground truth AFib episodes.
    - total_duration (int): Total duration of the recording.

    Returns:
    - dict: Confusion matrix including TP, FP, FN, TN durations and episode counts, 
            along with accuracy, precision, recall, DER, F1 score.
    """
    
    detected_arr = np.zeros(total_duration, dtype=int)
    labeled_arr = np.zeros(total_duration, dtype=int)

    # Mark detected AFib episodes
    for onset, offset in detected_afib:
        detected_arr[onset:offset] = 1

    # Mark ground truth AFib episodes
    for onset, offset in labeled_afib:
        labeled_arr[onset:offset] = 1

    # Compute duration-based metrics
    tp_duration = np.sum((detected_arr == 1) & (labeled_arr == 1))
    fp_duration = np.sum((detected_arr == 1) & (labeled_arr == 0))
    fn_duration = np.sum((detected_arr == 0) & (labeled_arr == 1))
    tn_duration = np.sum((detected_arr == 0) & (labeled_arr == 0))

    # Compute episode-based metrics
    tp_episode = sum(
        any(labeled_arr[onset:offset] == 1) for onset, offset in detected_afib
    )
    fp_episode = sum(
        all(labeled_arr[onset:offset] == 0) for onset, offset in detected_afib
    )
    fn_episode = sum(
        all(detected_arr[onset:offset] == 0) for onset, offset in labeled_afib
    )

    # Calculate metrics for duration
    accuracy_duration = (tp_duration + tn_duration) / (tp_duration + fp_duration + fn_duration + tn_duration)
    precision_duration = tp_duration / (tp_duration + fp_duration) if (tp_duration + fp_duration) > 0 else 0
    recall_duration = tp_duration / (tp_duration + fn_duration) if (tp_duration + fn_duration) > 0 else 0
    der_duration = (fp_duration + fn_duration) / tp_duration if tp_duration > 0 else 0
    f1_score_duration = 2 * (precision_duration * recall_duration) / (precision_duration + recall_duration) if (precision_duration + recall_duration) > 0 else 0

    # Calculate metrics for episode
    accuracy_episode = (tp_episode + (len(labeled_afib) - fn_episode)) / len(labeled_afib)
    precision_episode = tp_episode / (tp_episode + fp_episode) if (tp_episode + fp_episode) > 0 else 0
    recall_episode = tp_episode / (tp_episode + fn_episode) if (tp_episode + fn_episode) > 0 else 0
    der_episode = (fp_episode + fn_episode) / tp_episode if tp_episode > 0 else 0
    f1_score_episode = 2 * (precision_episode * recall_episode) / (precision_episode + recall_episode) if (precision_episode + recall_episode) > 0 else 0

    return {
        "duration_metrics": {
            "tp_duration": tp_duration,
            "fp_duration": fp_duration,
            "fn_duration": fn_duration,
            "tn_duration": tn_duration,
            "accuracy_duration": accuracy_duration,
            "precision_duration": precision_duration,
            "recall_duration": recall_duration,
            "der_duration": der_duration,
            "f1_score_duration": f1_score_duration,
        },
        "episode_metrics": {
            "tp_episode": tp_episode,
            "fp_episode": fp_episode,
            "fn_episode": fn_episode,
            "accuracy_episode": accuracy_episode,
            "precision_episode": precision_episode,
            "recall_episode": recall_episode,
            "der_episode": der_episode,
            "f1_score_episode": f1_score_episode,
        },
    }
