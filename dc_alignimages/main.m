%% Convert images to reference images for Maya use
% align center points and bounding box
% Date: 20110510
% Author: Zhu Xiaolong
clear all
close all
%% globals
dirr = 'E:\DATA\[CDC4CV11]\BaseModelImage\zxl_3\'
img_top_name = 'top.JPG';
img_front_name = 'front.JPG';   
img_side_name = 'side.JPG';

%% read
img_top = imread([dirr img_top_name]);
img_frt = imread([dirr img_front_name]);
img_sid = imread([dirr img_side_name]);

%% find middle point
img_top_crop = findAndCrop(img_top);
img_frt_crop = findAndCrop(img_frt);
img_sid_crop = findAndCrop(img_sid);

%% save images
imwrite(img_top_crop, [dirr 'crop_' img_top_name]);
imwrite(img_frt_crop, [dirr 'crop_' img_front_name]);
imwrite(img_sid_crop, [dirr 'crop_' img_side_name]);


