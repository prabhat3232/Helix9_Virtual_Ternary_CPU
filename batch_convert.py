from PIL import Image
import os
import glob

def convert_all():
    # Find all PPM files in helix9-cpp directory
    ppm_files = glob.glob('helix9-cpp/*.ppm')
    print(f"Found {len(ppm_files)} PPM files.")
    
    for ppm_path in ppm_files:
        try:
            with open(ppm_path, 'r') as f:
                # Basic PPM Parser for P3
                header = f.readline().strip()
                if header != 'P3':
                    print(f"Skipping {ppm_path}: Not P3 format")
                    continue
                    
                dims = f.readline().strip()
                while dims.startswith('#'): dims = f.readline().strip()
                w, h = map(int, dims.split())
                
                max_val = f.readline().strip()
                
                print(f"Converting {ppm_path} ({w}x{h})...")
                
                data = f.read().split()
                pixels = []
                for i in range(0, len(data), 3):
                    r = int(data[i])
                    g = int(data[i+1])
                    b = int(data[i+2])
                    pixels.append((r,g,b))
                
                img = Image.new('RGB', (w, h))
                img.putdata(pixels)
                
                png_path = ppm_path.replace('.ppm', '.png')
                img.save(png_path)
                print(f"Saved {png_path}")
        except Exception as e:
            print(f"Error converting {ppm_path}: {e}")

if __name__ == "__main__":
    convert_all()
