%Important: I chose te to be a percentage for convenience.
function [EdgeImage]=canny(filename, te)
close all
I=imread(filename);
figure
imagesc(I);
I=double(I(:,:,1));
axis image
figure
colormap(gray);
imagesc(I), colorbar;
axis image
k=1/159*[2 4 5 4 2; 4 9 12 9 4; 5 12 15 12 5; 4 9 12 9 4; 2 4 5 4 2];
filtered=conv2(I, k, 'same');
figure
colormap(gray);
imagesc(filtered), colorbar;
axis image
sobel_vert=[-1 0 1; -2 0 2; -1 0 1];
sobel_horz=[-1 -2 -1;0 0 0; 1 2 1];
edge_vert=conv2(filtered, sobel_vert, 'same');
edge_horz=conv2(filtered, sobel_horz, 'same');
edge_mag=sqrt(edge_vert.*edge_vert+edge_horz.*edge_horz);
edge_angle=atan(edge_horz./edge_vert);
figure
colormap(gray);
imagesc(edge_mag), colorbar;
axis image
small=edge_angle/(pi/4);
small=round(small);
edge_angle=small*(45);
[M, N]=size(edge_mag);
for s=2:M-1
    for t=2:N-1
        if edge_angle(s, t)==0
            [~,Q]=max([edge_mag(s, t-1), edge_mag(s, t), edge_mag(s, t+1)]);
            if Q~=2
                edge_mag(s, t)=0;
            end
        elseif edge_angle(s, t)==-45
            [~,Q]=max([edge_mag(s+1, t-1), edge_mag(s, t), edge_mag(s+1, t-1)]);
            if Q~=2
                edge_mag(s, t)=0;
            end
        elseif edge_angle(s, t)==45
            [~,Q]=max([edge_mag(s+1, t+1), edge_mag(s, t), edge_mag(s-1, t-1)]);
            if Q~=2
                edge_mag(s, t)=0;
            end
        elseif edge_angle(s, t)==-90||edge_angle(s, t)==90
            [~,Q]=max([edge_mag(s-1, t), edge_mag(s, t), edge_mag(s+1, t)]);
            if Q~=2
                edge_mag(s, t)=0;
            end
        end       
    end
end
figure
colormap(gray);
imagesc(edge_mag), colorbar;
axis image
maximum_edge=max(max(edge_mag));
threshold=te/100*maximum_edge;
[M, N]=size(edge_mag);%could be redundant, but I included it just in case I would change earlier code.
thresh_mag=zeros(M, N);
for s=1:M
    for t=1:N
        if edge_mag(s, t)<threshold
            thresh_mag(s, t)=0;
        else
            thresh_mag(s, t)=1;
        end
    end
end
figure
colormap(gray);
imagesc(thresh_mag), colorbar;
axis image
EdgeImage=edge_mag;
end