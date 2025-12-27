# Aptitude Logo Images

This folder contains the Aptitude application logo and icon images.

## Required Images

You need to save two versions of the Aptitude logo in this folder:

### 1. `aptitude-logo.png`
**Purpose**: Full logo displayed in the login dialog and about screen
**Recommended Size**: 400x400 pixels or larger
**Format**: PNG with transparent background
**Content**: The full Aptitude logo with the hexagonal 3D design and "APTITUDE" text

**How to create:**
1. Save your original Aptitude logo image as `aptitude-logo.png` in this folder
2. The image should be high-resolution (at least 400x400px)
3. Transparent background is preferred
4. The logo will be automatically scaled to fit in the UI

### 2. `aptitude-icon.png`
**Purpose**: Application icon displayed in the taskbar, dock, and window title bar
**Recommended Sizes**:
- macOS: 512x512 pixels (will be auto-scaled)
- Windows: 256x256 pixels (will be auto-scaled)
- Linux: 256x256 pixels

**Format**: PNG with transparent background
**Content**: Just the hexagonal 3D icon part (without the "APTITUDE" text)

**How to create:**
1. Crop/extract just the hexagonal cube icon from your logo
2. Save it as `aptitude-icon.png` at 512x512 pixels
3. Transparent background is required
4. This will be used as the app icon in the OS

## After Adding Images

Once you've saved both images:

1. **Verify the files are in place:**
   ```bash
   ls -la resources/images/
   # You should see:
   # aptitude-logo.png
   # aptitude-icon.png
   ```

2. **Rebuild the application:**
   ```bash
   cd /path/to/cpp-skill-matrix
   cmake --build build
   ```

3. **Run and verify:**
   ```bash
   ./build/SkillMatrix
   ```

   You should see:
   - The logo on the login screen
   - "Aptitude" in the window title
   - Updated branding colors throughout the app

## Current Branding Colors

The app now uses these colors from the Aptitude logo:

- **Primary Blue**: `#60A5FA` - Light blue for buttons and accents
- **Cyan**: `#22D3EE` - Accent color for highlights
- **Green**: `#10B981` - Success states
- **Yellow**: `#FDE047` - Warning/info states
- **Orange**: `#FB923C` - Warning states
- **Red**: `#EF4444` - Error/danger states
- **Dark Blue**: `#1E3A8A` - Brand primary (backgrounds)

## Troubleshooting

**Logo not showing:**
1. Check that the image files exist in `resources/images/`
2. Verify filenames are exactly: `aptitude-logo.png` and `aptitude-icon.png` (lowercase)
3. Make sure you rebuilt the application after adding the images
4. Check the build output for any resource compilation errors

**Image quality issues:**
- Use high-resolution PNG images (at least 400x400 for logo, 512x512 for icon)
- Ensure transparent backgrounds for best results
- Use the original logo file at the highest resolution available

## Platform-Specific Icon Setup

### macOS
The `aptitude-icon.png` will be used to generate the `.icns` file for the app bundle.
Recommended size: 512x512 or 1024x1024 pixels.

### Windows
The icon will be converted to `.ico` format for Windows executables.
Recommended sizes in the same file: 16x16, 32x32, 48x48, 256x256 pixels.

### Linux
The PNG icon will be used directly.
Recommended size: 256x256 pixels.
