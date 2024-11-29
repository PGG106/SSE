import psutil
import sys
import time

def format_bytes(size):
    # Convert bytes to a human-readable format
    for unit in ['bytes','KB','MB','GB','TB']:
        if size < 1024:
            return f"{size:.1f} {unit}"
            size /= 1024
        size /= 1024
    return f"{size:.1f} PB"

# Get the process name from the command-line argument, default to 'Alexandria'
process_name = sys.argv[1] if len(sys.argv) > 1 else 'Alexandria'

process_info = {}
prev_printed_lines = 0  # Number of lines printed in the previous iteration
max_observed_memory = 0  # Variable to track the max of maxes

try:
    while True:
        # Get current processes with the specified name
        current_processes = {p.pid: p for p in psutil.process_iter(['pid', 'name']) if p.info['name'] == process_name}

        # Update process_info
        # Remove PIDs that are no longer active
        active_pids = set(current_processes.keys())
        tracked_pids = set(process_info.keys())

        # Remove ended processes
        for pid in tracked_pids - active_pids:
            del process_info[pid]

        # Add new processes
        for pid in active_pids - tracked_pids:
            process_info[pid] = {'process': current_processes[pid], 'max_memory': 0}

        # Update memory usage
        for pid, info in process_info.items():
            process = info['process']
            try:
                memory_usage = process.memory_info().rss  # in bytes
                if memory_usage > info['max_memory']:
                    info['max_memory'] = memory_usage

                    # Update the max observed memory
                    if info['max_memory'] > max_observed_memory:
                        max_observed_memory = info['max_memory']
                info['current_memory'] = memory_usage
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                # Process has ended or access is denied
                info['current_memory'] = 0

        # Move the cursor up to overwrite previous lines
        if prev_printed_lines > 0:
            print('\033[F' * prev_printed_lines, end='')  # Move cursor up to the start of the previous output

        # Calculate new printed lines
        printed_lines = 0

        # Display the process info
        for pid, info in process_info.items():
            # Clear the line before printing
            print('\033[2K\r', end='')  # Clear the entire line and return carriage
            current_memory = info.get('current_memory', 0)
            max_memory = info.get('max_memory', 0)
            print(f"PID: {pid}, Current Memory: {format_bytes(current_memory)}, Max Memory: {format_bytes(max_memory)}")
            printed_lines += 1

        # If there are no processes, still need to account for printed lines
        if not process_info:
            print('\033[2K\r', end='')  # Clear the line
            print(f"No '{process_name}' processes found.")
            printed_lines = 1

        # Display the max observed memory at the bottom
        print('\033[2K\r', end='')  # Clear the line
        print(f"Max observed memory: {format_bytes(max_observed_memory)}")
        printed_lines += 1  # Increment printed_lines to account for the new line

        # If the new printed lines are less than prev_printed_lines, we need to clear extra lines
        if printed_lines < prev_printed_lines:
            # Clear the remaining lines
            for _ in range(prev_printed_lines - printed_lines):
                print('\033[2K\r', end='')  # Clear the entire line
                print('\033[F', end='')    # Move cursor up one line

            # Move cursor back down to the correct position
            if prev_printed_lines - printed_lines > 0:
                print('\033[E' * (prev_printed_lines - printed_lines), end='')  # Move cursor down

        # Update prev_printed_lines
        prev_printed_lines = printed_lines

        # Sleep for 1000ms
        time.sleep(1)
except KeyboardInterrupt:
    pass
