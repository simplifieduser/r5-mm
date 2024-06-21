import re
import os

def parse_header_file(file_path):
    error_messages = {}
    pattern = re.compile(r'#define\s+(\w+)\s+"([^"]+)"')
    extended_pattern = re.compile(r'#define\s+(\w+)\s+(\w+)\s*\+\s*"([^"]+)"')

    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.match(line)
            if match:
                name, message = match.groups()
                error_messages[name] = message
            else:
                extended_match = extended_pattern.match(line)
                if extended_match:
                    name, ref, message = extended_match.groups()
                    if ref in error_messages:
                        error_messages[name] = error_messages[ref] + message

    return error_messages

# Get the directory where the script is located
script_dir = os.path.dirname(__file__)

# Path to the header file, assuming it's in a parallel folder named "include"
header_file_path = os.path.join(script_dir, '..', 'src', 'messages.h')

# Resolve the full path
header_file_path = os.path.abspath(header_file_path)

# Parse the header file and store the messages
error_messages = parse_header_file(header_file_path)

#done by chatgpt with slight adjustments to make it work