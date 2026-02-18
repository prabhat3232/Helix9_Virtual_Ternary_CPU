import sys

def hexdump(filename):
    with open(filename, 'rb') as f:
        content = f.read()
    
    # Locate "rec:"
    try:
        start_idx = content.find(b'rec:')
    except:
        print("rec: not found")
        return

    # Print 200 bytes around rec:
    chunk = content[start_idx : start_idx + 200]
    
    print(f"Hex Dump of {filename} around 'rec:':")
    for i in range(0, len(chunk), 16):
        hex_row = ' '.join(f'{b:02x}' for b in chunk[i:i+16])
        text_row = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk[i:i+16])
        print(f"{i:04x}: {hex_row:<48} | {text_row}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python debug_hexdump.py <file>")
    else:
        hexdump(sys.argv[1])
