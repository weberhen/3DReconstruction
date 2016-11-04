%function [point_cloud] =  getPointCloud(indexed_stripes_img, Dp, W, Ds, P, k)
function [point_cloud] =  getPointCloud() 
% Dp: the distance between the projector and the system origin  % W:  the width between successive stripes on the calibration plane  
% Ds: the distance between the camera and the projector  
% P: the pixel size on the sensor plane of the camera  
% k: the radial lens distortion coefficient 
% getPointCloud(indexed_stripes_img)  given the image with the indexed stripes, do the triangulation to recover the 3D location of each point in the image

%get stripe indexes

%Get coordinates v & h

%Get coordinates x , y,z of the object surface

a = imread('../data/face/indexed_stripes_img.png');
b = imread('../data/face/binarystripes.png');
b = b/255;
c = a.*repmat(b,[1,1,3]);

%filtering the image to get only the yellow colour
channel1Min = 200;
channel1Max = 230;
channel2Min = 200;
channel2Max = 240;
channel3Min = 60;
channel3Max = 160;

yellow_mask = ( (c(:,:,1) >= channel1Min) & (c(:,:,1) <= channel1Max) ) & ...
    (c(:,:,2) >= channel2Min ) & (c(:,:,2) <= channel2Max) & ...
    (c(:,:,3) >= channel3Min ) & (c(:,:,3) <= channel3Max);
yellow_mask = uint8(yellow_mask);

yellow_img = c.*repmat(yellow_mask,[1,1,3]);
ind_stripe_index1= zeros(size(yellow_img));
ind_stripe_index2= zeros(size(yellow_img));
ind_stripe_index3= zeros(size(yellow_img));
ind_stripe_index4= zeros(size(yellow_img));
ind_stripe_index5= zeros(size(yellow_img));
ind_stripe_index6= zeros(size(yellow_img));
ind_stripe_index7= zeros(size(yellow_img));
ind_stripe_index8= zeros(size(yellow_img));
ind_stripe_index9= zeros(size(yellow_img));
ind_stripe_index10= zeros(size(yellow_img));
ind_stripe_index11= zeros(size(yellow_img));

ind_stripe_index1(1:38,:,:) = 1; 
ind_stripe_index2(38:74,:,:) = 1; 
ind_stripe_index3(74:108,:,:) = 1; 
ind_stripe_index4(108:135,:,:) = 1; 
ind_stripe_index5(135:167,:,:) = 1; 
ind_stripe_index6(167:201,:,:) = 1; 
ind_stripe_index7(201:225,:,:) = 1; 
ind_stripe_index8(225:255,:,:) = 1; 
ind_stripe_index9(255:283,:,:) = 1; 
ind_stripe_index10(283:309,:,:) = 1; 
ind_stripe_index11(309:331,:,:) = 1; 

array_ind_stripe_index={ind_stripe_index1,ind_stripe_index2,ind_stripe_index3,ind_stripe_index4,ind_stripe_index5,ind_stripe_index6,ind_stripe_index7,ind_stripe_index8,ind_stripe_index9,ind_stripe_index10,ind_stripe_index11};
    
final_index_image=zeros(size(yellow_img));
final_index_image = uint8(final_index_image);

for i=1:11
    array_ind_stripe_index{i}=uint8(array_ind_stripe_index{i});
    
    i_yellow_stripe = yellow_img.*array_ind_stripe_index{i};
    
    final_index_image(i_yellow_stripe~=0)=i;
   
end

imtool(final_index_image);

imwrite(final_index_image, 'our_index_stripes.png');

%ind_stripe_index1 = uint8(ind_stripe_index1);

%imshow(ind_stripe_index1*255);
%figure;
%first_yellow_stripe = yellow_img.*ind_stripe_index1;

%final_index_image=zeros(size(yellow_img));
%final_index_image(first_yellow_stripe(:,:,1)~=0)=1;
%imshow(final_index_image*255);

end