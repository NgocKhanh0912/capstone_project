function signal_labeling_ui(signal, output_filepath)
    % Parameters
    window_size = 5000; % Points per view
    current_start = 1; % Starting point
    labeled_peaks = []; % Labeled points
    handles = []; % Store point handles
    
    % Detect initial peaks using TERMA
    fs = 100;  % Sampling frequency (Hz)
    w_cycle = 55;
    w_evt = 9;
    beta = 0.095;
    peak_indices = terma_detect_peaks(signal, fs, w_cycle, w_evt, beta);

    % Create UI figure
    fig = figure('Name', 'Signal Labeling Tool', 'NumberTitle', 'off', ...
                 'Position', [100, 100, 800, 600]);

    % Axes for plotting signal
    ax = axes('Parent', fig, 'Position', [0.1, 0.2, 0.8, 0.7]);
    plot(ax, signal(1:window_size), 'b');
    hold(ax, 'on');
    title(ax, 'Signal Labeling');
    xlabel(ax, 'Index');
    ylabel(ax, 'Amplitude');

    % Plot initial peaks
    plot_peaks_in_window(current_start, peak_indices, ax);

    % Add button: Next
    btn_next = uicontrol('Parent', fig, 'Style', 'pushbutton', 'String', 'Next', ...
                         'Position', [400, 50, 80, 30], ...
                         'Callback', @next_callback);

    % Add button: Save
    btn_save = uicontrol('Parent', fig, 'Style', 'pushbutton', 'String', 'Save', ...
                         'Position', [500, 50, 80, 30], ...
                         'Callback', @save_callback);

    % Add button: Add Point
    btn_add = uicontrol('Parent', fig, 'Style', 'pushbutton', 'String', 'Add Point', ...
                        'Position', [600, 50, 80, 30], ...
                        'Callback', @add_point_callback);

    % Callback: Next
    function next_callback(~, ~)
        % Clear old handles
        delete(handles);
        handles = [];
        
        % Update the current window's starting index
        current_start = current_start + window_size;
        
        % Check if the end of the signal is reached
        if current_start > length(signal)
            msgbox('End of signal reached!', 'Info');
            return;
        end
        
        % Determine the current window's end index
        current_end = min(current_start + window_size - 1, length(signal));
        
        % Clear axes and plot the current window
        cla(ax);
        plot(ax, signal(current_start:current_end), 'b');
        
        % Re-plot detected peaks within the current window
        plot_peaks_in_window(current_start, peak_indices, ax);
    end

    % Callback: Save
    function save_callback(~, ~)
        % Collect labeled peaks from the current window
        labeled_peaks = [];
        for i = 1:numel(handles)
            if isvalid(handles(i)) % Only save valid handles
                global_index = handles(i).UserData; % Global index
                labeled_peaks = [labeled_peaks; global_index];
            end
        end
        labeled_peaks = unique(labeled_peaks); % Remove duplicates
        
        % Append labeled peaks to the file
        if ~isempty(labeled_peaks)
            file_id = fopen(output_filepath, 'a'); % Open file in append mode
            fprintf(file_id, '%d\n', labeled_peaks); % Write data
            fclose(file_id); % Close the file
            msgbox('Labeled peaks appended', 'Info');
        else
            msgbox('No labeled peaks to save', 'Warning');
        end
    end

    % Callback: Add Point
    function add_point_callback(~, ~)
        % Allow the user to manually add a point to the signal
        click_pos = ginput(1); % Get mouse input
        if ~isempty(click_pos)
            % Round x to nearest index within the current window
            local_index = round(click_pos(1));

            % Ensure x is within the current window range
            if local_index >= 1 && local_index <= min(window_size, length(signal) - current_start + 1)
                global_index = current_start + local_index - 1; % Convert to global index
                
                % Ensure the global index is valid
                if global_index > 0 && global_index <= length(signal)
                    % Force the y-coordinate to align with the signal value
                    aligned_y = signal(global_index);

                    % Create a point that snaps to the signal
                    h = drawpoint(ax, 'Position', [local_index, aligned_y], ...
                                  'InteractionsAllowed', 'translate');
                    h.UserData = global_index; % Store global index
                    handles = [handles; h];

                    % Add event listener to enforce point stays on signal
                    addlistener(h, 'MovingROI', @(src, evt) enforce_on_signal(src, evt));
                else
                    msgbox('Point is outside the signal range!', 'Warning');
                end
            else
                msgbox('Selected point is out of the current view!', 'Warning');
            end
        end
    end

    % Function to enforce points stay on the signal
    function enforce_on_signal(src, ~)
        % Get the current position of the point
        current_pos = src.Position;

        % Round x to nearest index within the window
        local_index = round(current_pos(1));

        % Ensure x stays within the current window range
        if local_index >= 1 && local_index <= min(window_size, length(signal) - current_start + 1)
            global_index = current_start + local_index - 1; % Convert to global index

            % Ensure global index is valid
            if global_index > 0 && global_index <= length(signal)
                % Snap y to the signal value at the corresponding x
                aligned_y = signal(global_index);
                src.Position = [local_index, aligned_y];
            end
        end
    end

    % Function to plot peaks in the current window
    function plot_peaks_in_window(start_idx, peak_indices, ax)
        for i = 1:length(peak_indices)
            if peak_indices(i) >= start_idx && peak_indices(i) < start_idx + window_size
                idx = peak_indices(i) - start_idx + 1;
                h = drawpoint(ax, 'Position', [idx, signal(peak_indices(i))], ...
                              'InteractionsAllowed', 'translate');
                h.UserData = peak_indices(i); % Store global index
                handles = [handles; h];
            end
        end
    end
end
