function [stripes_img] = getStripes(input_img_name)
 input_img = imread(input_img_name);
 stripes_img = zeros(size(input_img));
 imshow(stripes_img);
end