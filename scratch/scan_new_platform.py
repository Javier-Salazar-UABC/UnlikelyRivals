from PIL import Image

def scan_new_platform():
    img = Image.open('assets/atlas/escenario_namek.png').convert('RGBA')
    pixels = img.load()
    
    print("Checking new platform region:")
    min_x, max_x = 210, 254
    min_y, max_y = 67, 79
    
    for y in range(min_y - 2, max_y + 3):
        row_solid = []
        for x in range(min_x - 5, max_x + 6):
            r, g, b, a = pixels[x, y]
            if a > 50:
                row_solid.append('#')
            else:
                row_solid.append('.')
        print(f"Y={y:03d}: " + "".join(row_solid))

if __name__ == '__main__':
    scan_new_platform()
