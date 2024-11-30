import os
import shutil

def copy_all_files(src_folder, dist_folder):
    # Ensure the source folder exists
    if not os.path.exists(src_folder):
        raise FileNotFoundError(f"Source folder '{src_folder}' does not exist.")

    # Walk through the source folder
    for root, dirs, files in os.walk(src_folder):
        # Filter for .py files in the current directory
        py_files = [f for f in files if f.endswith(".py")]

        # If there are .py files in the current folder, process it
        if py_files:
            # Calculate the relative path and recreate the folder in the destination
            relative_path = os.path.relpath(root, src_folder)
            dest_path = os.path.join(dist_folder, relative_path)
            os.makedirs(dest_path, exist_ok=True)

            # Copy the .py files
            for file_name in py_files:
                src_file = os.path.join(root, file_name)
                dest_file = os.path.join(dest_path, file_name)
                shutil.copy2(src_file, dest_file)

src = "src"
dist = "dist"
copy_all_files(src, dist)