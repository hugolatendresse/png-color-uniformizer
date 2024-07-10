
## PNG Color Uniformizer

This tool helps turn this:
<br>
<img src="assets/gift.png" alt="Description" width="200" height="200">

Into this:
<br>
<img src="assets/gift_3colors.png" alt="Description" width="200" height="200">

Or this:
<br>
<img src="assets/gift_2colors.png" alt="Description" width="200" height="200">


png-color-uniformizer is a tool designed to simplify the color palette of 
PNG images. It uses k-modes and k-means clustering algorithms to reduce the 
number of colors used in an image, making it more uniform. This is particularly 
useful for creating graphics or reducing file sizes (the first image above is 674kB 
while the third one is 13kB).


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

You will need libpng and SDL2 installed on your system. Assuming Ubuntu or other Debian-based system, you can install them with the following commands:
  ```sh
  sudo apt update
  sudo apt install libpng-dev
  sudo apt install libsdl2-dev
  ```

### Running the tool 

  ```sh
  git clone https://github.com/hugolatendresse/png-color-uniformizer.git
  cd png-color-uniformizer
  mkdir build
  cd build
  cmake ..
  make pcu
  ./pcu <file_in> <file_out> [options]
  ```

Example:<br>
    ```
    ./pcu ../assets/gift.png gift_2colors.png -d -c 2 -r 255 -b 150 -g 155 -a 200 --kmeans
    ```


<!-- USAGE EXAMPLES -->
## Usage

`./pcu <file_in> <file_out> [options]`

Optional arguments description:
<br>file_in Path to input file (the PNG image to change)
<br>file_out Path to output file (where to save)
<br>-h Display this message. Other arguments are ignored
<br>-c Number of groups for k-modes algorithm. Default is 2
<br>-r RGBA value for Red
<br>-g RGBA value for Green
<br>-b RGBA value for Blue
<br>-a RGBA value for Alpha
<br>-d To display image once its transformed
<br>-s set the seed. Picked at random if not provided
<br>--kmeans will use k-means instead of k-modes. The default is k-modes.   
--alpha will treat alpha like other color components and change it. The default behavior is to keep alpha the same for all pixels.

<br>If nothing is supplied, k-modes will be used with two groups. For example:<br>
    ```
    ./pcu icon.png icon_new.png
    ```

<br>Using k-means with 5 colors:<br>
    ```
    ./pcu icon.png icon_new.png -c 5 --kmeans
    ```
<br>
<br>If a set of RGBA values are supplied, one of the clusters will be forced to be that color
<br>Example below will use k-modes (the default) with three colors, one of which will be (255,100,0,255)
<br>The other two colors will be picked by the k-modes algorithm.<br>
    ```
    ./pcu icon.png icon_new.png -c 3 -r 255 -g 100 -b 0 -a 255
    ```

To get the pink image above, you can run:<br>
`./pcu assets/gift.png gift_2colors.png -d -c 2 -r 255 -b 150 -g 155 -a 200 --kmeans`

To get the version with 3 colors (white gold black) above, you can run:<br>
`./pcu assets/gift.png gift_3colors.png -c 3 -d`



## Features
Color Reduction: Reduce the number of colors in a PNG file using k-modes (default) or k-means clustering algorithms.
<Br>Alpha Channel Handling: Option to include or exclude the alpha channel in the color reduction process.
<Br>Custom Color Forcing: Force the image to include specific colors by providing RGBA values.
<Br>Seed Customization: Ability to specify a seed for reproducibility of the color reduction process.
<Br>Image Display: Option to display the transformed image directly after processing.
