<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Best-README-Template</h3>

  <p align="center">
    An awesome README template to jumpstart your projects!
    <br />
    <a href="https://github.com/othneildrew/Best-README-Template"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/othneildrew/Best-README-Template">View Demo</a>
    ·
    <a href="https://github.com/othneildrew/Best-README-Template/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    ·
    <a href="https://github.com/othneildrew/Best-README-Template/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
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
  ./pcu <input_file> <output_file> [options]
  ```

Example:
    ```
    ./pcu ../assets/gift.png gift_2colors.png -d -c 2 -r 255 -b 150 -g 155 -a 200 --kmeans
    ```


<!-- USAGE EXAMPLES -->
## Usage

Usage: 
<br>
`./pcu file_in.png file_out.png [-c <c>] [--kmeans]`
<br>
`./pcu file_in.png file_out.png -r <r> -g <g> -b <b> -a <a> [-c <c>] [--kmeans]`

Arguments description:
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

<!-- ROADMAP -->
## Roadmap

- [x] Add Changelog
- [x] Add back to top links
- [ ] Add Additional Templates w/ Examples
- [ ] Add "components" document to easily copy & paste sections of the readme
- [ ] Multi-language Support
    - [ ] Chinese
    - [ ] Spanish

See the [open issues](https://github.com/othneildrew/Best-README-Template/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Your Name - [@your_twitter](https://twitter.com/your_username) - email@example.com

Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. I've included a few of my favorites to kick things off!

* [Choose an Open Source License](https://choosealicense.com)
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Malven's Flexbox Cheatsheet](https://flexbox.malven.co/)
* [Malven's Grid Cheatsheet](https://grid.malven.co/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)
* [Font Awesome](https://fontawesome.com)
* [React Icons](https://react-icons.github.io/react-icons/search)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=for-the-badge
[contributors-url]: https://github.com/othneildrew/Best-README-Template/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/othneildrew/Best-README-Template.svg?style=for-the-badge
[forks-url]: https://github.com/othneildrew/Best-README-Template/network/members
[stars-shield]: https://img.shields.io/github/stars/othneildrew/Best-README-Template.svg?style=for-the-badge
[stars-url]: https://github.com/othneildrew/Best-README-Template/stargazers
[issues-shield]: https://img.shields.io/github/issues/othneildrew/Best-README-Template.svg?style=for-the-badge
[issues-url]: https://github.com/othneildrew/Best-README-Template/issues
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/othneildrew
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 
