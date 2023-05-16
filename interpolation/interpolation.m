%% Interpolation
% find a polynomial function so that:
% bit_pos = func(angular_pos)
% angular_pos = array of angular postion
% bit_pos = array of bit position (corresponding position in bit)
angular_pos = [-90,-45,0,+45,+90]';
bit_pos = [110,215,305,395,480]';
[n,m]= size(angular_pos);
A = zeros(n,n);
for i = 1:1:n
    for j = 1:1:n
        A(i,j) = angular_pos(i)^(j-1);
    end
end
if det(A) ~= 0 coeff = A^(-1)*bit_pos;
else disp('error: A is singular');
end
%% bit_position extraction
character_pos_LEFT = zeros(8,1);
character_pos_MID = zeros(8,1);
character_pos_RIGHT = zeros(8,1);
LEFT = +30;     % +30°
RIGHT = -30;    % -30°
for i=1:1:8
    index = i-1;
    if index<3 
        angle = (360/16)*(7-(index+8));
    elseif index==3 
        angle = 0;
    else 
        angle = (360/16)*(7-index);
    end
    for j=1:1:n
        character_pos_MID(i) = character_pos_MID(i) + coeff(j)*angle^(j-1);
        character_pos_RIGHT(i) = character_pos_RIGHT(i) + coeff(j)*(angle+RIGHT)^(j-1);
        character_pos_LEFT(i) = character_pos_LEFT(i) + coeff(j)*(angle+LEFT)^(j-1);
    end  
end