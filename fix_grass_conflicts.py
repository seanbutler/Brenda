#!/usr/bin/env python3
import json
import re

# Read the environment file
with open('environment.json', 'r') as f:
    data = f.read()

# Remove comments for parsing
clean_data = re.sub(r'/\*.*?\*/', '', data, flags=re.DOTALL)
clean_data = re.sub(r'//.*', '', clean_data)

# Parse JSON
env = json.loads(clean_data)

# Get all Path tile coordinates
path_coords = set()
for tile in env['tiles']:
    if tile['type'] == 'Path':
        path_coords.add((tile['x'], tile['y']))

print(f"Found {len(path_coords)} Path tiles at coordinates: {sorted(path_coords)}")

# Remove Grass tiles that conflict with Path tiles
original_count = len(env['tiles'])
env['tiles'] = [tile for tile in env['tiles'] 
                if not (tile['type'] == 'Grass' and (tile['x'], tile['y']) in path_coords)]

removed_count = original_count - len(env['tiles'])
print(f"Removed {removed_count} conflicting Grass tiles")
print(f"Total tiles now: {len(env['tiles'])}")

# Write back to file (preserving original format with comments)
# We need to reconstruct the original structure
output_lines = []
in_tiles_section = False
tiles_written = False

for line in data.split('\n'):
    # Check if we're entering the tiles section
    if '"tiles": [' in line:
        in_tiles_section = True
        output_lines.append(line)
        continue
    
    # Check if we're leaving the tiles section
    if in_tiles_section and line.strip() == '],':
        if not tiles_written:
            # Write all the cleaned tiles
            for i, tile in enumerate(env['tiles']):
                comma = ',' if i < len(env['tiles']) - 1 else ''
                output_lines.append(f'    {{ "x": {tile["x"]}, "y": {tile["y"]}, "type": "{tile["type"]}" }}{comma}')
            tiles_written = True
        output_lines.append(line)
        in_tiles_section = False
        continue
    
    # Skip original tile lines when in tiles section
    if in_tiles_section:
        continue
    
    # Copy all other lines as-is
    output_lines.append(line)

# Write the cleaned file
with open('environment.json', 'w') as f:
    f.write('\n'.join(output_lines))

print("Successfully removed conflicting Grass tiles from environment.json")
