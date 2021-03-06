//
//  main.cpp
//  Coin Detector
//
//  Created by Elizabeth Kelly and William Wu on 11/14/16.
//  Copyright © 2016 PerspectiveProjection. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Coin.hpp"
#include "Template.hpp"
#include "ImageFunctions.hpp"

using namespace std;
using namespace cv;

Mat rotateImage(Mat orig_image, int angle);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    
    String image_file = argv[1]; //filename for the coin image
    Mat orig_image = imread(image_file); //read the file
    resize(orig_image, orig_image, cvSize(378, 504)); //resize the image to 1/8th it's original size
    //greyscale original image
    //cvtColor(orig_image, orig_image, COLOR_BGR2GRAY);
    
    //read in the 710x710 templates and store them in an array
    Mat quarter_template = imread("../../Images/Templates/Quarter_Face.jpg");
    Mat penny_template = imread("../../Images/Templates/Penny_Face.jpg");
    Mat dime_template = imread("../../Images/Templates/Dime_Face.jpg");
    Mat nickel_template = imread("../../Images/Templates/Nickel_Face.jpg");
    
    //vector of templates, in order of least amount to greatest amount
    vector<Mat> orig_templates;
    vector<string> template_names;
    vector<double> template_worth;
    orig_templates.push_back(penny_template);
    template_names.push_back("Penny");
    template_worth.push_back(0.01);
    orig_templates.push_back(nickel_template);
    template_names.push_back("Nickel");
    template_worth.push_back(0.05);
    orig_templates.push_back(dime_template);
    template_names.push_back("Dime");
    template_worth.push_back(0.10);
    orig_templates.push_back(quarter_template);
    template_names.push_back("Quarter");
    template_worth.push_back(0.25);
    
    //the vector of grayscale templates to be used for template matching
    vector<Template> templates;
    for (int i = 0; i < orig_templates.size(); i++) {
        Mat temp_gray_image;
        cvtColor(orig_templates[i], temp_gray_image, CV_BGR2GRAY);
        Template temp_template(template_names[i], temp_gray_image);
        temp_template.setWorth(template_worth[i]);
        templates.push_back(temp_template);
    }

    if (!orig_image.data) {
        printf("No image data \n");
        return -1;
    }
   
	//assign labels to coins on a binary image
    vector<Coin> coin_vector = setCoinAttributes(orig_image);
    
    //check
    
    for (int i = 0; i < coin_vector.size(); i++) {
        cout << "Coin " << i+1 << endl;
        cout << "Area " << coin_vector[i].getArea() << endl;
        cout << "Label " << coin_vector[i].getLabel() << endl;
        cout << "x center " << coin_vector[i].getCenterX() << endl;
        cout << "y center " << coin_vector[i].getCenterY() << endl;
        cout << endl;
    }

    //greyscale original image
    Mat gray_orig_image;
    cvtColor(orig_image, gray_orig_image, COLOR_BGR2GRAY);
    Mat transformed_image = circularHough(gray_orig_image, &coin_vector);
    
    imwrite("origImage.jpg", orig_image);

	for(int angle = 0; angle < 360; angle += 15) {
		Mat rotated_image = rotateImage(orig_image, angle);
		imwrite("rotated" + to_string(angle) + ".jpg", rotated_image);
	}
    
	string choice;

    templateMatch(orig_image, coin_vector, templates);
	
	return 0;
}

Mat rotateImage(Mat orig_image, int angle) { 
	Point2f center(orig_image.cols/2.0F, orig_image.rows/2.0F);
	Mat rotation_matrix = getRotationMatrix2D(center, angle, 1);
	Mat rotated_image;
	
	warpAffine(orig_image, rotated_image, rotation_matrix, orig_image.size());
	//imwrite("rotated" + to_string(i) + ".jpg", rotatedImage);

	return rotated_image;
}

