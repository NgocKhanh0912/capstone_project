function peak_indices = terma_detect_peaks(signal, fs, w_cycle, w_evt, beta)
    min_signal = min(signal);
    if min_signal < 0
        signal = signal - min_signal;
    end
    
    signal = signal .^ 2;
    
    ma_cycle = zeros(size(signal));
    ma_evt = zeros(size(signal));
    mean_signal = mean(signal);
    mean_arr = ones(size(signal)) * mean_signal;
    threshold_1 = zeros(size(signal));
    block_of_interest = zeros(size(signal));
    
    for i = (floor(w_evt/2) + 1):(length(signal) - floor(w_evt/2))
        ma_evt(i) = mean(signal(i - floor(w_evt/2): i + floor(w_evt/2)));
    end
    
    for i = (floor(w_cycle/2) + 1):(length(signal) - floor(w_cycle/2))
        ma_cycle(i) = mean(signal(i - floor(w_cycle/2): i + floor(w_cycle/2)));
    end
    
    threshold_1 = ma_cycle + beta * mean_arr;
    
    block_of_interest(ma_evt > threshold_1 & ma_cycle ~= 0) = 1;
    
    peak_indices = [];
    start_block = 0;
    stop_block = 0;
    
    for i = 1:length(block_of_interest) - 1
        if (block_of_interest(i+1) - block_of_interest(i)) == 1
            start_block = i;
        end
        if (block_of_interest(i) - block_of_interest(i+1)) == 1
            stop_block = i;
            if (stop_block - start_block) >= w_evt
                peak = signal(start_block);
                peak_index = start_block;
                for j = start_block:stop_block
                    if signal(j) > peak
                        peak = signal(j);
                        peak_index = j;
                    end
                end
                peak_indices = [peak_indices, peak_index];
            end
        end
    end
end
