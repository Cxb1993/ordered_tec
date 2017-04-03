function i32 = s2i(s)
% convert cell(1*N) of string to a row integer array which stands for the
% string's ascii, and end with zero

if isa(s,'cell')
    i32 = cellfun(@(x)([int32(x),0]),s,'UniformOutput',false);
    i32 = reshape(i32,1,[]);
    i32 = cell2mat(i32);
else
    i32 = [int32(s),0];
end
