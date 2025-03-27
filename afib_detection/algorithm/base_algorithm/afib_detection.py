import math  
import numpy as np  
from collections import deque

# Constants definition  
WORD_VALUE_WINDOW_LENGTH = 3   

class AFib_Detection:  
    def __init__(self, sequence_length, shannon_threshold, quanti_step, word_shift):  
        """  
        Initialize AFib detection object with default parameters  
        """
        self.sequence_length = sequence_length
        self.shannon_threshold = shannon_threshold
        self.quanti_step = quanti_step
        self.word_shift = word_shift

        self.peak_location_list = deque(maxlen=self.sequence_length)
        self.peak_peak_sequence = deque(maxlen=self.sequence_length) 
        self.word_value_window = deque([0] * WORD_VALUE_WINDOW_LENGTH, maxlen=WORD_VALUE_WINDOW_LENGTH)
        
        self.prediction_result = False  
        self.peak_count = 0  

    def get_symbolic_dynamic(self, heart_rate, heart_rate_max=240):  
        """  
        Convert heart rate to symbolic dynamic value  
        """  
        return min(heart_rate // self.quanti_step, heart_rate_max // self.quanti_step)  

    def get_word_value(self, symbolic_dynamic):
        """
        Calculate word value from symbolic dynamic window
        """
        symbolic_dynamic = [int(x) for x in symbolic_dynamic]
        return sum(value << (i * self.word_shift) for i, value in enumerate(reversed(symbolic_dynamic)))

    def count_unique_element(self):  
        """  
        Sort and count number of unique elements and its occurences from sequence  
        """ 
        unique_elements, occurrences = np.unique(self.peak_peak_sequence, return_counts=True)
        return unique_elements.tolist(), occurrences.tolist()

    def get_shannon_entropy(self):  
        """  
        Calculate Shannon entropy based on the occurrences of unique words  
        """  
        unique_elements, occurrences = self.count_unique_element()

        total_probability = 0.0  
        for i in range(len(unique_elements)):  
            probability = occurrences[i] / self.sequence_length  
            
            if probability > 0:  
                probability *= math.log2(probability)

            total_probability += probability  

        return -(len(unique_elements) / (self.sequence_length * math.log2(self.sequence_length))) * total_probability  

    def process(self, current_peak_location, heart_rate):  
        """  
        Process the current peak location for AFib detection  
        """  
        self.peak_location_list.append(current_peak_location)
        self.peak_count += 1  

        if self.peak_count > 1:  
            # Update word value window   
            self.word_value_window.append(self.get_symbolic_dynamic(heart_rate))

            # Update peak peak sequence 
            self.peak_peak_sequence.append(self.get_word_value(self.word_value_window))

            # Check if the entropy exceeds the threshold  
            if self.peak_count > self.sequence_length: 
                if self.get_shannon_entropy() >= self.shannon_threshold:
                    self.prediction_result = True
                    return self.prediction_result, self.peak_location_list[1] 

        self.prediction_result = False  
        return (self.prediction_result, None)  