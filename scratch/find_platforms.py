import sys
from PIL import Image

def analyze():
    # Load the image
    img = Image.open('assets/atlas/escenario_namek.png')
    width, height = img.size
    print(f"Image dimensions: {width} x {height}")

    # Convert to RGBA
    img = img.convert('RGBA')
    pixels = img.load()

    # Create a binary grid where 1 = non-transparent (alpha > 50) and 0 = transparent
    grid = []
    for y in range(height):
        row = []
        for x in range(width):
            r, g, b, a = pixels[x, y]
            # Consider it solid if alpha is high
            row.append(1 if a > 50 else 0)
        grid.append(row)

    # Connected component labeling (simple BFS/DFS) to find all solid clusters
    visited = [[False for _ in range(width)] for _ in range(height)]
    clusters = []

    for y in range(height):
        for x in range(width):
            if grid[y][x] == 1 and not visited[y][x]:
                # Found a new cluster!
                cluster_pixels = []
                queue = [(x, y)]
                visited[y][x] = True
                
                while queue:
                    cx, cy = queue.pop(0)
                    cluster_pixels.append((cx, cy))
                    
                    # 4-connectivity
                    for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                        nx, ny = cx + dx, cy + dy
                        if 0 <= nx < width and 0 <= ny < height:
                            if grid[ny][nx] == 1 and not visited[ny][nx]:
                                visited[ny][nx] = True
                                queue.append((nx, ny))
                
                # Analyze the cluster
                xs = [p[0] for p in cluster_pixels]
                ys = [p[1] for p in cluster_pixels]
                min_x, max_x = min(xs), max(xs)
                min_y, max_y = min(ys), max(ys)
                count = len(cluster_pixels)
                
                clusters.append({
                    'min_x': min_x, 'max_x': max_x,
                    'min_y': min_y, 'max_y': max_y,
                    'count': count,
                    'pixels': cluster_pixels
                })

    print(f"Found {len(clusters)} solid clusters:")
    # Filter and identify platforms vs ground
    # Floating platforms are usually smaller clusters and higher up (lower Y)
    for i, c in enumerate(clusters):
        w = c['max_x'] - c['min_x'] + 1
        h = c['max_y'] - c['min_y'] + 1
        cx = (c['min_x'] + c['max_x']) / 2.0
        cy = (c['min_y'] + c['max_y']) / 2.0
        print(f"Cluster {i}: bounding box X=[{c['min_x']}, {c['max_x']}] (w={w}), Y=[{c['min_y']}, {c['max_y']}] (h={h}), center=({cx:.2f}, {cy:.2f}), pixel count={c['count']}")

if __name__ == '__main__':
    analyze()
