# Motion-Detector
C++ algorithm which tries to detect motion between two images

This project takes two png images and a number of cores to compute with and the algorithm will attempt to detect motion between the two images and does the calculation using the specified number of cores.  The algorithm works by converting the images to grey-scale and then taking the differnce in the pixel values.  The resulting grey-scale and time-difference images are outputed as png images.  If the average pixel value of the grey-scale image is high enough then the algorithm determines that there is motion between the two images.  The program can be tested using img1.png, img2.png, img3.png, and compiled using the given makefile.

This program was used as my final project for Operating System using many concepts that I learned in class focusing on mult-thread management and protection of the critical section.

The project used the LodePNG library to decode the png images into raw pixel data for the algorithm to analyze and to decode the constructed pixel data for the grey-scale and time-difference into png images to output.
