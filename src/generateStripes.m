function img_stripes = gen_stripes(img_w,img_h,bar_orien,number_of_stripes)
img_stripes = 255 * ones(img_h, img_w, 'uint8');

size_stripes = floor(img_h/(number_of_stripes * 2));
half_size_stripes=floor(size_stripes/2);

% %plot black stripes middle to bottom
for l = (img_h/2)-(half_size_stripes):ceil(2.5*size_stripes):img_h
    img_stripes(l:l+size_stripes,:)=100;
end

% %plot gray stripes middle to bottom
for l = (img_h/2)-(half_size_stripes)+floor(6.7*size_stripes):8*size_stripes:img_h
    img_stripes(l:l+size_stripes,:)=150;
end

% %plot gray stripes middle to top
for l = (img_h/2)-(half_size_stripes)-floor(6.7*size_stripes):-8*size_stripes:0
    img_stripes(l:l+size_stripes,:)=150;
end

% %plot black stripes middle to top
for l = (img_h/2)-(half_size_stripes):-ceil(2.5*size_stripes):0
    img_stripes(l:l+size_stripes,:)=100;
end

%plot darker stripe in the middle of the pattern
img_stripes((img_h/2)-(half_size_stripes):(img_h/2)+(half_size_stripes),:)=0; 

imtool(img_stripes);

imwrite(img_stripes,['stripe',num2str(number_of_stripes),'.png']);

end