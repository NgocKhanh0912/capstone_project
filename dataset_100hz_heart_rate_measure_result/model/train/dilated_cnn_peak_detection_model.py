import os
import numpy as np
import pandas as pd
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv1D, BatchNormalization, Activation, Flatten, Dense
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.callbacks import ReduceLROnPlateau, EarlyStopping
from tensorflow.keras import Input

# Create a CNN model with dilated convolutions
def create_dilated_cnn(input_shape):
    model = Sequential()
    model.add(Input(shape=input_shape))  # First layer specifying the input shape
    model.add(Conv1D(filters=4, kernel_size=3, dilation_rate=1, padding='same'))
    model.add(BatchNormalization())
    model.add(Activation('elu'))

    # Add subsequent convolution layers with increasing dilation rates
    dilation_rates = [2, 4, 8, 16, 32, 64]
    for rate in dilation_rates:
        model.add(Conv1D(filters=min(rate, 32), kernel_size=3, dilation_rate=rate, padding='same'))
        model.add(BatchNormalization())
        model.add(Activation('elu'))
    model.add(Conv1D(filters=1, kernel_size=1, activation='sigmoid'))
    model.add(Flatten())
    return model

# Prepare PPG data and labels
def process_ppg_file(file_path, label_path, segment_length=1500, normalization_range=(-1, 1)):
    """
    Read PPG signals and labels from files, normalize them, and divide into segments.
    """
    # Read PPG signal
    ppg_data = pd.read_csv(file_path, header=None).squeeze("columns").values
    ppg_min, ppg_max = np.min(ppg_data), np.max(ppg_data)
    norm_min, norm_max = normalization_range
    normalized_ppg = norm_min + (ppg_data - ppg_min) * (norm_max - norm_min) / (ppg_max - ppg_min)

    peaks_indices = pd.read_csv(label_path, header=None).squeeze("columns").values
    labels = []
    segments = []
    for i in range(0, len(normalized_ppg), segment_length):
        segment = normalized_ppg[i:i+segment_length]
        if len(segment) < segment_length:
            segment = np.pad(segment, (0, segment_length - len(segment)), 'constant', constant_values=0)
        segments.append(segment)
        label = np.zeros(segment_length)
        for peak in peaks_indices:
            if i <= peak < i + segment_length:
                peak_pos = peak - i
                label[max(0, peak_pos - 2):min(segment_length, peak_pos + 3)] = 1
        labels.append(label)
    return np.array(segments), np.array(labels)

# File paths
input_dir = 'I:/dilated_cnn_peak_detection_model_data/train/train_data/data'
label_dir = 'I:/dilated_cnn_peak_detection_model_data/train/train_data/label'
segment_length = 1500

# Create the model
input_shape = (segment_length, 1)
model = create_dilated_cnn(input_shape)

# Initialize optimizer with a low learning rate
initial_learning_rate = 1e-4
optimizer = Adam(learning_rate=initial_learning_rate)

# Compile the model
model.compile(optimizer=optimizer, loss='binary_crossentropy', metrics=['accuracy'])

# Callbacks to reduce learning rate and stop early
reduce_lr = ReduceLROnPlateau(monitor='loss', factor=0.5, patience=5, min_lr=1e-6, verbose=1)
early_stopping = EarlyStopping(monitor='loss', patience=10, verbose=1, restore_best_weights=True)

# Train the model with each file and calculate the global loss
global_loss = 0
file_count = 0

for file_name in os.listdir(input_dir):
    input_file = os.path.join(input_dir, file_name)
    label_file = os.path.join(label_dir, file_name.replace(".csv", "_labeled_peaks.csv"))
    if os.path.exists(label_file):
        print(f"Processing: {file_name}")
        ppg_segments, ppg_labels = process_ppg_file(input_file, label_file, segment_length)
        ppg_segments = np.expand_dims(ppg_segments, axis=-1)  # Format (samples, 1500, 1)

        # Train on the current file and retrieve the loss
        history = model.fit(
            ppg_segments,
            ppg_labels,
            epochs=100,  # Maximum epochs
            batch_size=32,
            verbose=1,
            callbacks=[reduce_lr, early_stopping]
        )
        file_loss = history.history['loss'][-1]  # Retrieve the loss from the last epoch
        print(f"Loss for file {file_name}: {file_loss:.4f}")

        # Accumulate the global loss
        global_loss += file_loss
        file_count += 1

# Calculate the average global loss
global_loss /= file_count
print(f"Global loss (average across all files): {global_loss:.4f}")

# Save the trained model
model.save("dataset_100hz_heart_rate_measure_result/model/dilated_cnn_peak_detection_model.h5")
print("Training complete, and the model has been saved.")
