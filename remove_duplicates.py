import json
import re

def remove_duplicate_tiles(filename):
    # Read the file with comments
    with open(filename, 'r') as f:
        data = f.read()
    
    # Remove comments for parsing (same approach as your other scripts)
    clean_data = re.sub(r'/\*.*?\*/', '', data, flags=re.DOTALL)
    clean_data = re.sub(r'//.*', '', clean_data)
    
    # Parse JSON
    env = json.loads(clean_data)
    
    seen_coordinates = set()
    unique_tiles = []
    
    for tile in env['tiles']:
        coord = (tile['x'], tile['y'])
        if coord not in seen_coordinates:
            unique_tiles.append(tile)
            seen_coordinates.add(coord)
        else:
            print(f"Removing duplicate tile at ({tile['x']}, {tile['y']}) with type '{tile['type']}'")
    
    print(f"Original tiles: {len(env['tiles'])}")
    print(f"Unique tiles: {len(unique_tiles)}")
    print(f"Duplicates removed: {len(env['tiles']) - len(unique_tiles)}")
    
    env['tiles'] = unique_tiles
    
    # Write back to file (preserving original format structure)
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
    with open(filename, 'w') as f:
        f.write('\n'.join(output_lines))
    
    return env

# Run the script
remove_duplicate_tiles('environment.json')