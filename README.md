# A novel technique to draw antialiased circles without floating point math nor square root
## Juan Ramón Vadillo (Versa Design S.L.) – May 28 th, 2023

Many resource-limited processors such as microcontrollers, do not have any floating-point processing unit and square root calculation is done by fixed point iterative algorithms that consume a large amount of computing power. However. they are still used for controlling small displays with limited resolutions where antialiasing techniques can provide better visual renderings.

The typical non-antialiased filled circle algorithm removes the need of calculating the distance to the center of the circle by comparing squared distance to squared radius.

Squared radius is normally precomputed and a simple comparison x2+y2 < r2 is used to determine whether a pixel is inside the circle or not.

If we want to drawn an antialiased edge, we need to variate linearly the intensity of the pixels in the edge between the radius minus a half of pixel distance and the radius plus a half of pixel distance.

Since we have to run our algorithm without using floating point arithmetic, we can pre-calculate the inner and outer half pixel squared radius as follows:

```
(R𝑖𝑛𝑛𝑒𝑟)^2 =(𝑟− 0. 5 )^2 =𝑟^2 − 2 ∙𝑟∙ 0. 5 + 0. 25 ≅𝑟^2 −𝑟
(R𝑜𝑢𝑡𝑒𝑟)^2 =(𝑟+ 0. 5 )^2 =𝑟^2 + 2 ∙𝑟∙ 0. 5 + 0. 25 ≅𝑟^2 +𝑟
```

