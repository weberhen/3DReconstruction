function [stripes_img] = getStripes(input_img_name)
 
 %load input_image
 input_img = imread(input_img_name);
 
 %resize input_imgimage
 input_img = imresize(input_img,.4);
 
 %create stripes_img with the same size as the input
 stripes_img = zeros(size(input_img));
 
 %convert to grayscale
 input_img = rgb2gray(input_img);
 
 % function to recover the stripes
 % A(r, c) > A(r − 1,c) and A(r, c) > A(r + 1,c)
 f = @(x) (x(2,2) > x(1,2) && x(2,2) > x(3,2)) * 255;
 
 stripes_img  = nlfilter(input_img,[3 3],f);
   
 imshow(stripes_img);
 
 
end