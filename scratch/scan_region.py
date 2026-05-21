from PIL import Image

def scan_region():
    img = Image.open('assets/atlas/escenario_namek.png').convert('RGBA')
    pixels = img.load()
    
    # We want to check the region where the original platform was:
    # X in pixel coordinates was ~[159, 196]
    # Y in pixel coordinates was ~[95, 101]
    
    print("Checking original platform region:")
    found_solid = False
    min_x, max_x = 999, -1
    min_y, max_y = 999, -1
    
    for y in range(85, 120):
        row_solid = []
        for x in range(140, 210):
            r, g, b, a = pixels[x, y]
            if a > 50:
                row_solid.append('#')
                found_solid = True
                min_x = min(min_x, x)
                max_x = max(max_x, x)
                min_y = min(min_y, y)
                max_y = max(max_y, y)
            else:
                row_solid.append('.')
        
        # Only print rows that contain some solid pixels
        if '#' in row_solid:
            print(f"Y={y:03d}: " + "".join(row_solid))
            
    if found_solid:
        print(f"Solid pixels found in Y=[85, 120], X=[140, 210]:")
        print(f"Bounding Box: X=[{min_x}, {max_x}] (w={max_x - min_x + 1}), Y=[{min_y}, {max_y}] (h={max_y - min_y + 1})")
        print(f"Center: ({(min_x + max_x)/2:.2f}, {(min_y + max_y)/2:.2f})")
    else:
        print("No solid pixels found in this region.")

if __name__ == '__main__':
    scan_region()
