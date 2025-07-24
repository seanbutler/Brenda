#!/usr/bin/env python3
import json
import re

# Read the environment file
with open('environment.json', 'r') as f:
    data = f.read()

# Remove comments (simple approach for JSON with comments)
# Remove /* */ style comments
data = re.sub(r'/\*.*?\*/', '', data, flags=re.DOTALL)
# Remove // style comments
data = re.sub(r'//.*', '', data)

# Parse JSON
env = json.loads(data)

# Get all existing tile coordinates
existing_coords = set()
for tile in env['tiles']:
    existing_coords.add((tile['x'], tile['y']))

print(f'Grid size: {env["width"]}x{env["height"]}')
print(f'Existing tiles: {len(existing_coords)}')

# Generate missing coordinates
missing_coords = []
total_tiles = env['width'] * env['height']
for y in range(env['height']):
    for x in range(env['width']):
        if (x, y) not in existing_coords:
            missing_coords.append((x, y))

print(f'Missing tiles: {len(missing_coords)} out of {total_tiles}')

# Generate Grass tiles for all missing coordinates
grass_tiles = []
for x, y in missing_coords:
    grass_tiles.append(f'    {{ "x": {x}, "y": {y}, "type": "Grass" }}')

print(f'\nGenerated {len(grass_tiles)} Grass tiles')

# Write to a separate file for review
with open('grass_tiles.txt', 'w') as f:
    f.write(',\n'.join(grass_tiles))

print("Grass tiles written to grass_tiles.txt")
