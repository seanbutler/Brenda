#!/usr/bin/env python3
import json
import re

# Read the grass tiles
with open('grass_tiles.txt', 'r') as f:
    grass_content = f.read().strip()

# Read the environment file
with open('environment.json', 'r') as f:
    env_content = f.read()

# Find the position right after the "// --- GRASS (fill all undefined tiles) ---" comment
# and before the closing bracket of the tiles array
grass_comment = "    // --- GRASS (fill all undefined tiles) ---"
tiles_end = "  ],"

# Split the content to insert grass tiles
lines = env_content.split('\n')
result_lines = []
inserted = False

for i, line in enumerate(lines):
    result_lines.append(line)
    
    # If we find the grass comment, insert the grass tiles after it
    if grass_comment in line and not inserted:
        result_lines.append(grass_content)
        inserted = True

# Write back to environment.json
with open('environment.json', 'w') as f:
    f.write('\n'.join(result_lines))

print("Successfully added Grass tiles to environment.json")
