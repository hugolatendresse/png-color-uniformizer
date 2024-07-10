
// 3 color - ACTUALLY WORKED
./pcu assets/gift.png gift_3colors.png -c 3 -d

// pink only - ACTUALLY WORKED
./pcu assets/gift_3colors.png gift_2colors.png -d -c 2 -r 255 -b 150 -g 155 -a 200 --kmeans


# PNG Color Uniformizer
png-color-uniformizer is a tool designed to simplify the color palette of PNG images. It utilizes k-modes and k-means clustering algorithms to reduce the number of colors used in an image, making it more uniform. This is particularly useful for creating stylized graphics or reducing file sizes for web usage.

## Features
Color Reduction: Reduce the number of colors in a PNG file using k-modes (default) or k-means clustering algorithms.
Alpha Channel Handling: Option to include or exclude the alpha channel in the color reduction process.
Custom Color Forcing: Force the image to include specific colors by providing RGBA values.
Seed Customization: Ability to specify a seed for reproducibility of the color reduction process.
Image Display: Option to display the transformed image directly after processing.
Requirements
C Compiler (e.g., GCC, Clang)
libpng library installed
Installation
Clone the repository and build the project using a C compiler:

bash
Copy code
git clone [repository-url]
cd png-color-uniformizer
gcc -o pcu main.c -lpng
Usage
bash
Copy code
./pcu <input_file> <output_file> [options]
Options
-c <num>: Number of color groups (default is 2)
--kmeans: Use k-means algorithm instead of k-modes
--alpha: Treat alpha as a regular color component
-r <value>, -g <value>, -b <value>, -a <value>: Specify RGBA values for forced color inclusion
-s <seed>: Specify a seed for random number generation
-d: Display the image after transformation
-h: Display help information
Examples
Reduce colors using default settings (k-modes with 2 colors):

bash
Copy code
./pcu input.png output.png
Reduce colors to a specified RGB color, using k-means with 2 color groups:

bash
Copy code
./pcu input.png output.png -c 2 --kmeans -r 255 -g 100 -b 50 -a 255
Contributing
Contributions are welcome! Please feel free to submit pull requests or open issues to discuss proposed changes or enhancements.

License
Specify your licensing information here.