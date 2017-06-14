# simple vector
# 1a simple list 1 to 20
x <- c(1:20)
x
# 1b simple list, descending order
rev_x <- rev(x)
#x <- c(20:1)
rev_x
# 1c combine two simple vectors but with extra trick to eliminate 20 from second list
comb <- c(x, rev_x[rev_x < 20])

# 1d replication exercises
tmp <- c(4,6,3)
# 1e replicate tmp 10 times
tmp_4 <- rep(tmp, 10)
# 1f replicate vector 10 times but 4 is repeated an extra time
tmp_4mk2 <- c(tmp_4, 4)
# better way
tmp_4mk2 <- rep(tmp_4, 1, len = 31 ) 
tmp_4mk2 <- rep(tmp, 10, len = 31)
# 1g replicate tmp individual elements 10, 20, 30 times respectively
tmp_ascending <- rep(tmp, c(10, 20, 30))

# 2 create sequence of exponential function results
x_range <- seq(3, 6, by = 0.1)
# R computes trig in radians rather than degrees. 
e_func <- sapply(x_range, function(x) exp(x) * cos(x*pi/180)) 

# 3a series powers grow as follows: x = 3 * index (index = 1:12)
#                                   y = 1 + (3 * index) where index 0:11

func1 <- function(position, elem) {
  (elem ** (3 * position)) * 
  ((elem + 0.1) ** (1 + (3 * (position - 1))))
}

v1 <- c(0.1)
sapply(1:12, func1, v1)

# 3b similar to above were numerator power increases on par with vector index

# 4a compute series from 10 to 100 of i^3 + 4i^2
sum(sapply(10:100, function(x) (x**3) + (4 * x**2)))

# 4b similar principle

# 5a create vector of labels with space
paste('label', 1:30)
# 5b create vector of lables with no space
paste0("fn", 1:30)

# 6
set.seed(50)
xVec <- sample(0:999, 250, replace = T)
yVec <- sample(0:999, 250, replace = T)

mapply(function(x,y)  y - x,
       xVec,  # names from first
       yVec)

# alternatively yVec - xVec works nicely
yVec - xVec

#6a compute vector for y n - x (n-1)
rng <- 2:250
yVec[rng] - xVec[rng-1]

#6c x (n -2) + 2x (n - 1) - x n 
rng <- 1:248
xVec[rng] + (2 * xVec[rng + 1]) - xVec[rng + 2]

# 7a slicing vectors
# get actual results
yVec[yVec > 600]
# 7b get indices
which(yVec > 600)

# 7c get values of xVec corresponding to the same indices
xVec[which(yVec > 600)]

# 7d compute SQRT of absolute difference between members of x and mean of x
mean_x <- mean(xVec)
sqrt(abs(xVec - mean_x))

# 7e how many elements are within 200 of max of yVec
max_y <- max(yVec)
length(yVec[max_y - yVec <= 200])

# 7f how many elements are divisible by 2
length(xVec[xVec %% 2 == 0])

# 7h pick out elements at 1, 4, 7, ...
yVec[seq(1, 250, by = 3)]

# 8 use cumulative product
cumprod(1:5)
1 + sum(cumprod(seq(2,38, by = 2)/seq(3,39, by = 2)))
