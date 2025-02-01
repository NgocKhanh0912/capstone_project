import numpy as np

def normalize_ppg(ppg_data, normalization_range=(-1, 1)):
    """
    Normalize the PPG signal to a specified range.
    Parameters:
        - ppg_data (array): The input PPG signal data.
        - normalization_range (tuple): The range to normalize the data into.
    Returns:
        - array: Normalized PPG signal.
    """
    ppg_min, ppg_max = np.min(ppg_data), np.max(ppg_data)
    norm_min, norm_max = normalization_range
    return norm_min + (ppg_data - ppg_min) * (norm_max - norm_min) / (ppg_max - ppg_min)