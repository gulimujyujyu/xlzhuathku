function [ img_top_crop ] = findAndCrop( img_top )
figure(1)
imshow(img_top);
pt = ginput(2)  
cn = pt(1,:)
tl = pt(2,:)
%[(cn(1)-tl(1))*2 (cn(2)-tl(2))*2]
img_top_crop = imcrop(img_top, round([tl (cn(1)-tl(1))*2 (cn(2)-tl(2))*2]));
figure(2)
imshow(img_top_crop)
end

