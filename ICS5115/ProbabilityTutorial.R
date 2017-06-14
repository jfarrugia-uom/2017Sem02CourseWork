# Consider the range [1,100], what is the probability of a number (picked from this
# range) being divisible by 3 and 7 but not 2? Note: you can use set operations in R

# A = P(1:100 divisible by 3)
# B = P(1:100 divisible by 7)
# C = P(1:100 divisible by 2)

# (A intersection B) intersection C'

mult.3 <- which(c(1:100) %% 3 == 0) 
mult.7 <- which(c(1:100) %% 7 == 0)
mult.2 <- which(c(1:100) %% 2 == 0)

length(setdiff( intersect(mult.3, mult.7), mult.2 ) ) / 100

# ------------------------------------------------------------------
# Birthday problem
class.size <- 23
attempt.count <- 10000
attempts <- c();
for (i in 1:attempt.count) {
    # generate random birthdays for class of size class.size
    birth.days <- sample(1:365, class.size, replace=TRUE)
    # aggregate common birthdays in list containing distinct values and number of occurrences
    shared.birth.days <- rle(sort(birth.days))
    # if there is at least 1 pair, add success (1) flag to vector of attempts
    attempts <- c(attempts, ifelse(length(shared.birth.days$lengths[shared.birth.days$lengths >= 2]) >= 1, 1, 0))
}
rate.shared.bday <- sum(attempts) / attempt.count
rate.shared.bday


p.rain <- 55/365
p.norain <- 1 - (55/365)
p.prediction.rain <- 0.9 * p.rain
p.prediction.norain <- 0.15 * p.norain

p.prediction.rain <- p.prediction.rain / (p.prediction.rain + p.prediction.norain)