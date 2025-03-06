import tensorflow as tf

model = tf.keras.models.load_model("heart_rate_measure/dataset_100hz_result/model/dilated_cnn_peak_detection_model.h5")

converter = tf.lite.TFLiteConverter.from_keras_model(model)

converter.optimizations = [tf.lite.Optimize.DEFAULT]

tflite_model = converter.convert()

with open("heart_rate_measure/dataset_100hz_result/model/dilated_cnn_peak_detection_model.tflite", "wb") as f:
    f.write(tflite_model)

print("Done! Model saved as .tflite")
