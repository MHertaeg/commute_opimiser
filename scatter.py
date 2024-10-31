import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import griddata
from mpl_toolkits.mplot3d import Axes3D
from PIL import Image
from scipy.ndimage import zoom
#updated


# Load the CSV file and parse coordinates
file_path = "coordinates.csv"  # Replace with your actual file path
data = pd.read_csv(file_path)
data[['latitude', 'longitude']] = data['coordinates'].str.extract(r'(-?\d+\.\d+),\s*(-?\d+\.\d+)').astype(float)

# Convert latitude and longitude to Cartesian system (approximate km)
x = data['longitude'] * 111.32
y = data['latitude'] * 110.574
z = data['TT min']  # Using 'TT min' as example

top_left = (145.0, -37.7)    # Replace with the actual top-left corner coordinates
top_right = (145.3, -37.7)   # Replace with the actual top-right corner coordinates
bottom_left = (145.0, -37.9) # Replace with the actual bottom-left corner coordinates
bottom_right = (145.3, -37.9) # Replace with the actual bottom-right corner coordinates

# Convert to approximate Cartesian coordinates
x_min = top_left[0] * 111.32
x_max = top_right[0] * 111.32
y_min = bottom_left[1] * 110.574
y_max = top_left[1] * 110.574


# Create a grid for interpolation
grid_x, grid_y = np.mgrid[x.min():x.max():100j, y.min():y.max():100j]
grid_z = griddata((x, y), z, (grid_x, grid_y), method='cubic')

# Load and transform the map image to fit the grid
#map_img = Image.open("map.png")  # Load your map image
map_img = Image.open("map.png").convert("RGB")  # Convert to RGB to ensure 3 channels


from scipy.ndimage import zoom

# Resize map_img to match the grid shape of `grid_x` and `grid_y`
map_img_resized = zoom(map_img, (grid_x.shape[0] / map_img.shape[0], grid_x.shape[1] / map_img.shape[1], 1))

# Plotting
fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')

# Map image as a surface at a fixed z level (slightly below grid_z)
z_map = np.full_like(grid_x, np.nanmin(grid_z) - 1)  # Offset for visibility
ax.plot_surface(grid_x, grid_y, z_map, rstride=1, cstride=1, facecolors=map_img_resized, shade=False)

# Interpolated surface on top
surf = ax.plot_surface(grid_x, grid_y, grid_z, cmap='viridis', edgecolor='none', alpha=0.7)

# Scatter plot for reference points
ax.scatter(x, y, z, color='r', marker='o', s=20, label="Data Points")

# Labels and color bar
ax.set_xlabel("Longitude (approx. km)")
ax.set_ylabel("Latitude (approx. km)")
ax.set_zlabel("Distance (TT min)")
fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5, label="Interpolated Distance (TT min)")
plt.legend()

plt.show()








"""


map_img = map_img.resize((100, 100))  # Resize to match the grid resolution
map_img = np.array(map_img) / 255.0  # Normalize the image for color mapping
#print(map_img)
# Plotting
fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')

# Map image as a surface at a fixed z level (e.g., min z value)

z_map = np.full_like(grid_x, np.nanmin(grid_z) - 1)  # Offset for visibility

print(z_map)
ax.plot_surface(grid_x, grid_y, z_map, rstride=1, cstride=1, facecolors=map_img, extent=[x_min, x_max, y_min, y_max], shade=False)

#ax.plot_surface(grid_x, grid_y, z_map, rstride=1, cstride=1, facecolors=map_img, shade=False)

# Interpolated surface on top
surf = ax.plot_surface(grid_x, grid_y, grid_z, cmap='viridis', edgecolor='none', alpha=0.7)

# Scatter plot for reference points
ax.scatter(x, y, z, color='r', marker='o', s=20, label="Data Points")

# Labels and color bar
ax.set_xlabel("Longitude (approx. km)")
ax.set_ylabel("Latitude (approx. km)")
ax.set_zlabel("Distance (TT min)")
fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5, label="Interpolated Distance (TT min)")
plt.legend()

plt.show()
"""
