# Gencode Algorithm 
- Page 564/465 ( dragon 1 (example 9.13))

## Labeling 
```pascal
if n is a leaf then 
    if n is leftmost child of its parent then
        label(n) := 1
    else label(n) := 0
else begin
    let n1, n2, ..., nk be the children of n ordered by label so label(n1 >= label(n2)>=...label(nk))
    label(n):=max(label(ni) + i-1)
end 
```



```pascal
procedure gencode(n);
begin 
/* case 0 */
if n is a left leaf representing operand name and n is the leftmost child of its parent then 
    print 'MOV' || name || ',' || top(rstack)

else if n is an interior node 
```


