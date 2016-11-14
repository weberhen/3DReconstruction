%function [point_cloud] =  getPointCloud(indexed_stripes_img, Dp, W, Ds, P, k)
function [point_cloud] =  getPointCloud(Dp, W, Ds, P) 
% Dp: the distance between the projector and the system origin  % W:  the width between successive stripes on the calibration plane  
% Ds: the distance between the camera and the projector  
% P: the pixel size on the sensor plane of the camera  
% k: the radial lens distortion coefficient 
% getPointCloud(indexed_stripes_img)  given the image with the indexed stripes, do the triangulation to recover the 3D location of each point in the image

delete v_h.txt
delete face_3D_cloud.pcd

a = imread('../data/face/indexed_stripes_img_scaled.png');
b = imread('../data/face/binarystripes.png');

%a = imresize(a, [768 576]); 
%b = imresize(b, [768 576]); 
%imtool(a);
b = b/255;

%imtool(b*255);
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

% ind_stripe_index1(1:90,:,:) = 1; 
% ind_stripe_index2(91:157,:,:) = 1; 
% ind_stripe_index3(158:230,:,:) = 1; 
% ind_stripe_index4(231:298,:,:) = 1; 
% ind_stripe_index5(299:371,:,:) = 1; 
% ind_stripe_index6(372:445,:,:) = 1; 
% ind_stripe_index7(446:499,:,:) = 1; 
% ind_stripe_index8(500:565,:,:) = 1; 
% ind_stripe_index9(566:626,:,:) = 1; 
% ind_stripe_index10(627:686,:,:) = 1; 
% ind_stripe_index11(687:731,:,:) = 1; 

ind_stripe_index1(1:54,:,:) = 1; 
ind_stripe_index2(55:118,:,:) = 1; 
ind_stripe_index3(119:180,:,:) = 1; 
ind_stripe_index4(181:230,:,:) = 1; 
ind_stripe_index5(231:295,:,:) = 1; 
ind_stripe_index6(296:354,:,:) = 1; 
ind_stripe_index7(355:396,:,:) = 1; 
ind_stripe_index8(397:447,:,:) = 1; 
ind_stripe_index9(448:496,:,:) = 1; 
ind_stripe_index10(497:543,:,:) = 1; 
ind_stripe_index11(544:578,:,:) = 1; 


array_ind_stripe_index={ind_stripe_index1,ind_stripe_index2,ind_stripe_index3,ind_stripe_index4,ind_stripe_index5,ind_stripe_index6,ind_stripe_index7,ind_stripe_index8,ind_stripe_index9,ind_stripe_index10,ind_stripe_index11};
    
final_index_image=zeros(size(yellow_img));
final_index_image = uint8(final_index_image);

nStripes = 11;

for i=1:nStripes
    array_ind_stripe_index{i}=uint8(array_ind_stripe_index{i});
    
    i_yellow_stripe = yellow_img.*array_ind_stripe_index{i};
    
    final_index_image(i_yellow_stripe~=0)=i;
   
end

%imtool(final_index_image*255);

imwrite(final_index_image, 'our_index_stripes.png');

%using the real indexes
%real_index = [39 32 25 18 11 4 3 10 17 24 31];
real_index = [ 48 41 34 27 20 13 6 -1 -8 -15 -22 ];
N = size(final_index_image, 2); %column
M = size(final_index_image, 1); %line

fileID_v_u = fopen('v_h.txt','w');

nPoints = 0;
x = [];
y = [];
z = [];
%index(row,column)
for r=1:M
   for c=1:N
       if(final_index_image(r,c) ~= 0)
           Wn = real_index(final_index_image(r,c)) * W;
           u = r - 0.5*(M+1);
           h = c - 0.5*(N+1);
           
           %x(end+1) = Dp - (Dp*Ds)/(u*P*Dp + Wn);
           
           y(end+1) = (h*P*Dp*Ds)/(u*P*Dp + Wn);
           z(end+1) = (Wn*Ds)/(u*P*Dp + Wn);
           x(end+1) = Dp - (Dp * z(end))/Wn;
           
           fprintf(fileID_v_u,'%f %f %f %f\n',h,u,c,r);
           nPoints = nPoints + 1;
       end
   end
end

fclose(fileID_v_u);

%pcshow([x(:),y(:),z(:)]);

fileID = fopen('face_3D_cloud.pcd','w');
fprintf(fileID,'# .PCD v.7 - Point Cloud Data file format\n');
fprintf(fileID,'VERSION .7\n');
fprintf(fileID,'FIELDS x y z\n');
fprintf(fileID,'SIZE 4 4 4\n');
fprintf(fileID,'TYPE F F F\n');
fprintf(fileID,'COUNT 1 1 1\n');
fprintf(fileID,'WIDTH %d\n',nPoints);
fprintf(fileID,'HEIGHT 1\n');
fprintf(fileID,'VIEWPOINT 0 0 0 1 0 0 0\n');
fprintf(fileID,'POINTS %d\n',nPoints);
fprintf(fileID,'DATA ascii\n');

for i=1:nPoints
    fprintf(fileID,'%f %f %f\n',x(i),y(i),z(i));
end

fclose(fileID);

end