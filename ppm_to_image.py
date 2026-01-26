import os
from PIL import Image

def convert_ppm_to_png(ppm_filename, output_filename):
    try:
        # Open the PPM file and read text content
        with open(ppm_filename, 'r') as f:
            content = f.read()
        
        # Tokenize the content (remove comments and split by whitespace)
        tokens = []
        for line in content.splitlines():
            # Remove comments starting with #
            line = line.split('#')[0].strip()
            if line:
                tokens.extend(line.split())
        
        # Parse Header
        if not tokens or tokens[0] != 'P3':
            print("Error: Invalid PPM (P3) format")
            return

        width = int(tokens[1])
        height = int(tokens[2])
        max_val = int(tokens[3])
        
        # Parse Pixel Data
        data_values = [int(t) for t in tokens[4:]]
        
        # Create a new RGB image
        img = Image.new('RGB', (width, height))
        pixels = img.load()
        
        # Fill image with pixel data
        idx = 0
        for y in range(height):
            for x in range(width):
                if idx + 2 < len(data_values):
                    r = data_values[idx]
                    g = data_values[idx+1]
                    b = data_values[idx+2]
                    
                    # Scale values if max_val is not 255
                    if max_val != 255:
                        r = int(r * 255 / max_val)
                        g = int(g * 255 / max_val)
                        b = int(b * 255 / max_val)
                        
                    pixels[x, y] = (r, g, b)
                    idx += 3
        
        # Save output
        img.save(output_filename)
        print(f"Success! Image saved as {output_filename}")
        
    except Exception as e:
        print(f"An error occurred: {e}")

# Run the conversion
if __name__ == "__main__":
    convert_ppm_to_png('dream_hybrid.ppm', 'dream_hybrid.png')